#include "bbque/pp/mango_platform_proxy.h"
#include "bbque/config.h"
#include "bbque/res/resource_path.h"
#include "bbque/utils/assert.h"
#include "bbque/pp/mango_platform_description.h"
#include "bbque/resource_mapping_validator.h"

#include <mango-hn/hn.h>

#define BBQUE_MANGOPP_PLATFORM_ID		"org.mango"

namespace bb = bbque;
namespace br = bbque::res;
namespace po = boost::program_options;


namespace bbque {
namespace pp {


MangoPlatformProxy * MangoPlatformProxy::GetInstance() {
	static MangoPlatformProxy * instance;
	if (instance == nullptr)
		instance = new MangoPlatformProxy();
	return instance;
}

MangoPlatformProxy::MangoPlatformProxy() :
	refreshMode(false)
{

	//---------- Get a logger module
	logger = bu::Logger::GetLogger(MANGO_PP_NAMESPACE);
	bbque_assert(logger);

	bbque_assert ( 0 == hn_initialize(MANGO_DEVICE_NAME) ); 

	ResourceMappingValidator &rmv = ResourceMappingValidator::GetInstance();
	rmv.RegisterSkimmer(std::make_shared<MangoPartitionSkimmer>() , 100);

}

MangoPlatformProxy::~MangoPlatformProxy() {

	bbque_assert(0 == hn_end());

}


const char* MangoPlatformProxy::GetPlatformID(int16_t system_id) const noexcept {
	UNUSED(system_id);
	return BBQUE_MANGOPP_PLATFORM_ID;
}

const char* MangoPlatformProxy::GetHardwareID(int16_t system_id) const noexcept {
	UNUSED(system_id);
	return BBQUE_TARGET_HARDWARE_ID;    // Defined in bbque.conf
}

bool MangoPlatformProxy::IsHighPerformance(
		bbque::res::ResourcePathPtr_t const & path) const {
	UNUSED(path);
	return false;
}

MangoPlatformProxy::ExitCode_t MangoPlatformProxy::Setup(AppPtr_t papp) noexcept {
	ExitCode_t result = PLATFORM_OK;
	UNUSED(papp);
	return result;
}

MangoPlatformProxy::ExitCode_t
MangoPlatformProxy::Release(AppPtr_t papp) noexcept {

	UNUSED(papp);
	return PLATFORM_OK;
}

MangoPlatformProxy::ExitCode_t
MangoPlatformProxy::ReclaimResources(AppPtr_t papp) noexcept {

	UNUSED(papp);
	return PLATFORM_OK;
}

MangoPlatformProxy::ExitCode_t
MangoPlatformProxy::MapResources(AppPtr_t papp, ResourceAssignmentMapPtr_t pres, bool excl) noexcept {

	UNUSED(papp);
	UNUSED(pres);
	UNUSED(excl);
	return PLATFORM_OK;
}

MangoPlatformProxy::ExitCode_t MangoPlatformProxy::Refresh() noexcept {
	refreshMode = true;

	return PLATFORM_OK;
}


MangoPlatformProxy::ExitCode_t
MangoPlatformProxy::LoadPlatformData() noexcept {

	// Get the number of tiles
	if ( HN_SUCCEEDED != hn_get_num_tiles(&this->num_tiles) ) {
		logger->Fatal("Unable to get the number of tiles.");
		return PLATFORM_INIT_FAILED;
	}

	// TODO Get the tiles topology

	// Get the number of VNs
	if ( HN_SUCCEEDED != hn_get_num_vns(&this->num_vns) ) {
		logger->Fatal("Unable to get the number of VNs.");
		return PLATFORM_INIT_FAILED;
	}

	logger->Info("Found a total of %d tiles and %d VNs.", this->num_tiles, this->num_vns);


	ExitCode_t err;

	// Now we have to register the tiles to the PlatformDescription
	err = RegisterTiles();
	if (PLATFORM_OK != err) {
		return err;
	}

	return PLATFORM_OK;
}

MangoPlatformProxy::ExitCode_t
MangoPlatformProxy::RegisterTiles() noexcept {

	PlatformDescription &pd = pli->getPlatformInfo();
	PlatformDescription::System &sys = pd.GetLocalSystem();

	for (uint32_t i=0; i < num_tiles; i++) {
		hn_st_tile_info tile_info;
		int err = hn_get_tile_info(i, &tile_info);
		if (HN_SUCCEEDED != err) {
			logger->Fatal("Unable to get the tile nr.%d [error=%d].", i, err);
			return PLATFORM_INIT_FAILED;
		}

		MangoTile mt((MangoTile::MangoTileType_t)(tile_info.tile_type));

		for (int i=0; i < MangoTile::GetCoreNr(mt.GetType()); i++) {
			typedef PlatformDescription pd_t;
			pd_t::ProcessingElement pe(i , 0, 100, pd_t::PartitionType_t::MDEV);
			mt.AddProcessingElement(pe);
		}

		sys.AddAccelerator(mt);
	}


	return PLATFORM_OK;
}

static hn_st_request_t FillReq(const TaskGraph &tg) {

	hn_st_request_t req;
	req.num_comp_rsc    = tg.TaskCount();
	req.num_mem_buffers = tg.BufferCount();
	

	for (auto t : tg.Tasks()) {
		// TODO
	}

	unsigned int i=0;
	for (auto b : tg.Buffers()) {
		bbque_assert(i < req.num_mem_buffers);
		req.mem_buffers_size[i++] = b.second->Size();
	}

	return req;
}

static Partition GetPartition(const TaskGraph &tg, hn_st_request_t req, hn_st_response_t *res,
							  int i) {

	auto it_task = tg.Tasks().begin();
	auto it_buff = tg.Buffers().begin();

	Partition part(res[i].partition_id);

	for (unsigned int j=0; j<req.num_comp_rsc; j++) {
		part.MapTask(it_task->second, res[i].comp_rsc_tiles[j]);
		it_task++;
	}
	bbque_assert(it_task == tg.Tasks().end());

	for (unsigned int j=0; j<req.num_mem_buffers; j++) {
		part.MapBuffer(it_buff->second, res[i].mem_buffers_tiles[j]);
		it_buff++;
	}
	bbque_assert(it_buff == tg.Buffers().end());

	return part;
}

MangoPlatformProxy::MangoPartitionSkimmer::ExitCode_t
MangoPlatformProxy::MangoPartitionSkimmer::Skim(const TaskGraph &tg,
														std::list<Partition>&part_list) noexcept {
	hn_st_request_t req;
	hn_st_response_t res[MANGO_BASE_NUM_PARTITIONS];
	uint32 num_parts = MANGO_BASE_NUM_PARTITIONS;

	bbque_assert(part_list.empty());

	req = FillReq(tg);

	if ( (hn_get_partitions(&req, res, &num_parts)) ) {
		return SK_GENERIC_ERROR;
	}

	// No feasible partition found
	if ( num_parts == 0 ) {
		return SK_NO_PARTITION;
	}

	// Fill the partition vector with the partitions returned by HN library
	for (unsigned int i=0; i < num_parts; i++) {
		Partition part = GetPartition(tg, req, res, i);
		part_list.push_back(part);
	}
	
	return SK_OK;

}

}	// namespace pp
}	// namespace bbque