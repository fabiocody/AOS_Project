/*
 * Copyright (C) 2012  Politecnico di Milano
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BBQUE_APPLICATION_PROXY_H_
#define BBQUE_APPLICATION_PROXY_H_

#include "bbque/app/application.h"
#include "bbque/utils/worker.h"
#include "bbque/utils/logging/logger.h"
#include "bbque/plugins/rpc_channel.h"
#include "bbque/rtlib/rpc_messages.h"
#include "bbque/cpp11/thread.h"
#include "bbque/cpp11/future.h"

#include <map>
#include <memory>

#define APPLICATION_PROXY_NAMESPACE "bq.ap"

namespace ba = bbque::app;
namespace bl = bbque::rtlib;
namespace bp = bbque::plugins;
namespace bu = bbque::utils;

namespace bbque {

#ifdef CONFIG_BBQUE_OPENCL
class OpenCLProxy;
#endif


/**
 * @brief Interface to access application specific data and functionalities.
 *
 * The class used to communicate with Barbeque managed applications. From the
 * RTRM prespective, each class exposes a set of functionalities which could
 * be accessed using methods defined by this proxy. Each call requires to
 * specify the application to witch it is addressed and the actual parameters.
 */
class ApplicationProxy : public bu::Worker {

private:

	plugins::RPCChannelIF *rpc;

	typedef struct snCtx {
		std::thread exe;
		ba::AppPid_t pid;
	} snCtx_t;

	typedef std::promise<RTLIB_ExitCode> resp_prm_t;

	typedef std::future<RTLIB_ExitCode> resp_ftr_t;

	typedef std::shared_ptr<resp_ftr_t> prespFtr_t;

	typedef bp::RPCChannelIF::rpc_msg_ptr_t pchMsg_t;

	typedef struct cmdSn : public snCtx_t {
		ba::AppPtr_t papp;
		resp_prm_t resp_prm;
		resp_ftr_t resp_ftr;
		std::mutex resp_mtx;
		std::condition_variable resp_cv;
		pchMsg_t pmsg;
	} cmdSn_t;

	typedef std::shared_ptr<cmdSn_t> pcmdSn_t;

	typedef struct cmdRsp {
		RTLIB_ExitCode result;
		// The comand session to handler this command
		pcmdSn_t pcs;
	} cmdRsp_t;


#ifdef CONFIG_BBQUE_OPENCL
	OpenCLProxy & oclProxy;
#endif

public:

	static ApplicationProxy & GetInstance();

	~ApplicationProxy();

/*******************************************************************************
 * Command Sessions
 ******************************************************************************/

	RTLIB_ExitCode StopExecution(ba::AppPtr_t papp);

	RTLIB_ExitCode StopExecutionSync(ba::AppPtr_t papp);

/*******************************************************************************
 * Runtime profiling
 ******************************************************************************/

	/**
	 * @brief Get the runtime profiling of an OpenCL application
	 */
	RTLIB_ExitCode_t Prof_GetRuntimeData(ba::AppPtr_t papp);

/*******************************************************************************
 * Synchronization Protocol
 ******************************************************************************/

//----- PreChange

	/** The response to a PreChange command */
	typedef struct preChangeRsp : public cmdRsp_t {
		uint32_t syncLatency; ///> [ms] estimation of next sync point
	} preChangeRsp_t;

	/** A pointer to a response generated by a PreChange */
	typedef std::shared_ptr<preChangeRsp_t> pPreChangeRsp_t;

	/**
	 * @brief Synchronous PreChange
	 */
	RTLIB_ExitCode SyncP_PreChange(ba::AppPtr_t papp, pPreChangeRsp_t presp);

	/**
	 * @brief Get the result of an issued Asynchronous PreChange
	 */
	RTLIB_ExitCode SyncP_PreChange_GetResult(pPreChangeRsp_t presp);

//----- SyncChange

	/** The response to a SyncChange command */
	typedef struct syncChangeRsp : public cmdRsp_t {
	} syncChangeRsp_t;

	/** A pointer to a response generated by a SyncChange */
	typedef std::shared_ptr<syncChangeRsp_t> pSyncChangeRsp_t;

	/**
	 * @brief Synchronous SyncChange
	 */
	RTLIB_ExitCode SyncP_SyncChange(ba::AppPtr_t papp, pSyncChangeRsp_t presp);

	/**
	 * @brief Get the result of an issued Asynchronous PreChange
	 */
	RTLIB_ExitCode SyncP_SyncChange_GetResult(pSyncChangeRsp_t presp);

//----- DoChange

	/**
	 * @brief Synchronous DoChange
	 */
	RTLIB_ExitCode SyncP_DoChange(ba::AppPtr_t papp);


//----- PostChange

	/** The response to a PostChange command */
	typedef struct postChangeRsp : public cmdRsp_t {
	} postChangeRsp_t;

	/** A pointer to a response generated by a PostChange */
	typedef std::shared_ptr<postChangeRsp_t> pPostChangeRsp_t;

	/**
	 * @brief Synchronous PostChange
	 */
	RTLIB_ExitCode SyncP_PostChange(ba::AppPtr_t papp, pPostChangeRsp_t presp);


private:

	typedef std::shared_ptr<snCtx_t> psnCtx_t;

	typedef std::map<uint8_t, psnCtx_t> snCtxMap_t;

	snCtxMap_t snCtxMap;

	std::mutex snCtxMap_mtx;

	typedef struct conCtx {
		/** The applicaiton PID */
		ba::AppPid_t app_pid;
		/** The application name */
		char app_name[RTLIB_APP_NAME_LENGTH];
		/** The communication channel data to connect the applicaton */
		bp::RPCChannelIF::plugin_data_t pd;
	} conCtx_t;

	typedef std::shared_ptr<conCtx_t> pconCtx_t;

	typedef std::map<ba::AppPid_t, pconCtx_t> conCtxMap_t;

	conCtxMap_t conCtxMap;

	std::mutex conCtxMap_mtx;

	typedef struct rqsSn : public snCtx_t {
		pchMsg_t pmsg;
	} rqsSn_t;

	typedef std::shared_ptr<rqsSn_t> prqsSn_t;


	/**
	 * @brief	A multimap to track active Command Sessions.
	 *
	 * This multimap maps command session threads ID on the session data.
	 * @param AppPid_t the command session thread ID
	 * @param pcmdSn_t the command session handler
	 */
	typedef std::map<bl::rpc_msg_token_t, pcmdSn_t> cmdSnMap_t;

	cmdSnMap_t cmdSnMap;

	std::mutex cmdSnMap_mtx;



	typedef std::shared_ptr<cmdRsp_t> pcmdRsp_t;


	ApplicationProxy();

	bl::rpc_msg_type_t GetNextMessage(pchMsg_t & pmsg);


/*******************************************************************************
 * Command Sessions
 ******************************************************************************/

	inline pcmdSn_t SetupCmdSession(ba::AppPtr_t papp) const;

	/**
	 * @brief Enqueue a command session for response processing
	 *
	 * Since Barbeque has a single input RPC channel for each application,
	 * each response received from an applications should be dispatched to the
	 * thread which generated the command. Thus, the execution context which
	 * has generated a command must save a reference to itself for the proper
	 * dispatching of resposes.
	 *
	 * @param pcs command session handler which is waiting for a response
	 *
	 * @note This method must be called from within the session execution
	 * context, i.e. the command processing thread for asynchronous commands.
	 */
	inline void EnqueueHandler(pcmdSn_t pcs);

	void StopExecutionTrd(pcmdSn_t pcs);

	pcmdSn_t GetCommandSession(rpc_msg_header_t *pmsg_hdr);

	void ReleaseCommandSession(pcmdSn_t pcs);

	void CompleteTransaction(pchMsg_t & pmsg);

/*******************************************************************************
 * Runtime profiling
 ******************************************************************************/

#ifdef CONFIG_BBQUE_OPENCL

	/**
	 * @brief Thread function for the runtime data collection
	 */
	RTLIB_ExitCode_t Prof_GetRuntimeDataTrd(pcmdSn_t pcs);

	RTLIB_ExitCode_t Prof_GetRuntimeDataSend(ba::AppPtr_t papp);

	RTLIB_ExitCode_t Prof_GetRuntimeDataRecv(pcmdSn_t pcs);

#endif

/*******************************************************************************
 * Synchronization Protocol
 ******************************************************************************/

//----- PreChange

	RTLIB_ExitCode SyncP_PreChangeSend(pcmdSn_t pcs);

	RTLIB_ExitCode SyncP_PreChangeRecv(pcmdSn_t pcs, pPreChangeRsp_t preps);

	RTLIB_ExitCode SyncP_PreChange(pcmdSn_t pcs, pPreChangeRsp_t presp);

	void SyncP_PreChangeTrd(pPreChangeRsp_t presp);

//----- SyncChange

	RTLIB_ExitCode SyncP_SyncChangeSend(pcmdSn_t pcs);

	RTLIB_ExitCode SyncP_SyncChangeRecv(pcmdSn_t pcs, pSyncChangeRsp_t preps);

	RTLIB_ExitCode SyncP_SyncChange(pcmdSn_t pcs, pSyncChangeRsp_t presp);

	void SyncP_SyncChangeTrd(pSyncChangeRsp_t presp);

//----- DoChange

	/** The response to a SyncChange command */
	typedef struct doChangeRsp : public cmdRsp_t {
	} doChangeRsp_t;

	/** A pointer to a response generated by a SyncChange */
	typedef std::shared_ptr<doChangeRsp_t> pDoChangeRsp_t;

	RTLIB_ExitCode SyncP_DoChangeSend(pcmdSn_t pcs);

	RTLIB_ExitCode SyncP_DoChange(pcmdSn_t pcs, pDoChangeRsp_t presp);

//----- PostChange

	RTLIB_ExitCode SyncP_PostChangeSend(pcmdSn_t pcs);

	RTLIB_ExitCode SyncP_PostChangeRecv(pcmdSn_t pcs, pPostChangeRsp_t preps);

	RTLIB_ExitCode SyncP_PostChange(pcmdSn_t pcs, pPostChangeRsp_t presp);


/*******************************************************************************
 * Request Sessions
 ******************************************************************************/

	void RpcExcRegister(prqsSn_t prqs);

	void RpcExcUnregister(prqsSn_t prqs);


	void RpcExcSet(prqsSn_t prqs);

	void RpcExcClear(prqsSn_t prqs);

	void RpcExcGoalGap(prqsSn_t prqs);


	void RpcExcStart(prqsSn_t prqs);

	void RpcExcStop(prqsSn_t prqs);


	void RpcExcSchedule(prqsSn_t prqs);


	void RpcAppPair(prqsSn_t prqs);

	void RpcAppExit(prqsSn_t prqs);


	pconCtx_t GetConnectionContext(rpc_msg_header_t *pmsg_hdr);

	void RpcACK(pconCtx_t pcon, rpc_msg_header_t *pmsg_hdr,
			bl::rpc_msg_type_t type);

	void RpcNAK(pconCtx_t pcon, rpc_msg_header_t * pmsg_hdr,
			bl::rpc_msg_type_t type,
			RTLIB_ExitCode error);

	void RequestExecutor(prqsSn_t prqs);

	void ProcessRequest(pchMsg_t & pmsg);


	/**
	 * @brief The command dispatching thread.
	 */
	void Task();

};

} // namespace bbque

#endif /* end of include guard: BBQUE_APPLICATION_PROXY_H_ */
