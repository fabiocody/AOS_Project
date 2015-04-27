/*
 * Copyright (C) 2015  Politecnico di Milano
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

#include "tempura_schedpol.h"

#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "bbque/modules_factory.h"
#include "bbque/utils/logging/logger.h"

#include "bbque/app/working_mode.h"
#include "bbque/res/binder.h"
#include "bbque/res/identifier.h"
#include "bbque/res/usage.h"
#include "bbque/res/resource_path.h"

#define MODULE_CONFIG SCHEDULER_POLICY_CONFIG "." SCHEDULER_POLICY_NAME

namespace br = bbque::res;
namespace bu = bbque::utils;
namespace bw = bbque::pm;
namespace po = boost::program_options;

namespace bbque { namespace plugins {

// :::::::::::::::::::::: Static plugin interface ::::::::::::::::::::::::::::

void * TempuraSchedPol::Create(PF_ObjectParams *) {
	return new TempuraSchedPol();
}

int32_t TempuraSchedPol::Destroy(void * plugin) {
	if (!plugin)
		return -1;
	delete (TempuraSchedPol *)plugin;
	return 0;
}

// ::::::::::::::::::::: Scheduler policy module interface :::::::::::::::::::

char const * TempuraSchedPol::Name() {
	return SCHEDULER_POLICY_NAME;
}

TempuraSchedPol::TempuraSchedPol():
		cm(ConfigurationManager::GetInstance()),
		ra(ResourceAccounter::GetInstance()),
		mm(bw::ModelManager::GetInstance())
{
	// Logger instance
	logger = bu::Logger::GetLogger(MODULE_NAMESPACE);
	assert(logger);

	if (logger)
		logger->Info("tempura: Built a new dynamic object[%p]", this);
	else
		fprintf(stderr,
				FI("tempura: Built new dynamic object [%p]\n"), (void *)this);
}


TempuraSchedPol::~TempuraSchedPol() {

}

SchedulerPolicyIF::ExitCode_t TempuraSchedPol::Init() {
	ExitCode_t result = SCHED_OK;

	// Resource state view
	result = InitResourceStateView();
	if (result != SCHED_OK) {
		logger->Fatal("Init: Cannot get a resource state view");
		return result;
	}

	// Application slots
	InitSlots();

	// Power budgets data structures
	if (!power_budgets.empty()) {
		logger->Debug("Init: Power budgets initialized");
		return SCHED_OK;
	}

	result = InitBudgets();
	if (result != SCHED_OK) {
		logger->Fatal("Init: Power budgets initialization failed");
		return result;
	}

	return result;
}

SchedulerPolicyIF::ExitCode_t
TempuraSchedPol::InitResourceStateView() {
	ResourceAccounterStatusIF::ExitCode_t ra_result;
	ExitCode_t result = SCHED_OK;

	// Build a string path for the resource state view
	snprintf(status_view_id, 30, "%s%d", MODULE_NAMESPACE, ++sched_count);;

	// Get a fresh resource status view
	logger->Debug("Init: Require a new resource state view [%s]", status_view_id);
	ra_result = ra.GetView(status_view_id, sched_status_view);
	if (ra_result != ResourceAccounterStatusIF::RA_SUCCESS)
		return SCHED_ERROR_VIEW;
	logger->Info("Init: Resources state view token: %d", sched_status_view);

	return result;
}

SchedulerPolicyIF::ExitCode_t
TempuraSchedPol::InitBudgets() {

	// Binding type (CPU, GPU,...))
	BindingMap_t & bindings(ra.GetBindingOptions());
	for (auto & bd_entry: bindings) {
		BindingInfo_t const & bd_info(*(bd_entry.second));

		// Resource path e.g., "sys0.cpu[0..n].XX"
		for (br::ResourcePtr_t const & rsrc: bd_info.rsrcs) {
			br::ResourcePathPtr_t r_path(new br::ResourcePath(rsrc->Path()));
			r_path->AppendString("pe");
			// Budget object (path + budget value)
			br::ResourcePtrList_t r_list(ra.GetResources(r_path));
			br::UsagePtr_t pbudget(new br::Usage(0));
			br::UsagePtr_t rbudget(new br::Usage(0));
			pbudget->SetResourcesList(r_list);
			rbudget->SetResourcesList(r_list);

			// Add to the power budgets map
			power_budgets.insert(
					std::pair<br::ResourcePathPtr_t, br::UsagePtr_t>(
						r_path, pbudget));
			// Add to the resource budgets map
			resource_budgets.insert(
					std::pair<br::ResourcePathPtr_t, br::UsagePtr_t>(
						r_path, rbudget));
			// Add into models identifiers map
			std::string model_id;
			if (!r_list.empty())
				model_id = r_list.front()->Model();
			model_ids.insert(
					std::pair<br::ResourcePathPtr_t, std::string>(
						r_path, model_id));
			logger->Debug("Init: Budgeting on '%s' [Model: %s]",
					r_path->ToString().c_str(), model_id.c_str());
		}
	}

	return SCHED_OK;
}

SchedulerPolicyIF::ExitCode_t
TempuraSchedPol::InitSlots() {
	slots = 0;
	for (AppPrio_t p = 0; p <= sys->ApplicationLowestPriority(); p++) {
		slots += (sys->ApplicationLowestPriority()+1 - p) * sys->ApplicationsCount(p);
		logger->Debug("Init: Slots for prio %d [c=%d] = %d",
				p, sys->ApplicationsCount(p), slots);
	}
	logger->Info("Init: Slots for partitioning = %d", slots);

	return SCHED_OK;
}

SchedulerPolicyIF::ExitCode_t
TempuraSchedPol::Schedule(
		System & system,
		RViewToken_t & status_view) {
	SchedulerPolicyIF::ExitCode_t result;
	sys = &system;

	result = Init();
	if (result != SCHED_OK) {
		logger->Fatal("Schedule: Policy initialization failed");
		goto error;
	}

	result = ComputeBudgets();
	if (result != SCHED_OK) {
		logger->Fatal("Schedule: Budgets cannot be computed");
		goto error;
	}

	result = DoResourcePartitioning();
	if (result != SCHED_OK) {
		logger->Fatal("Schedule: Resource partitioning failed");
		goto error;
	}

	result = DoScheduling();
	if (result != SCHED_OK) {
		logger->Fatal("Schedule: Scheduling failed");
		goto error;
	}

	// Return the new resource status view according to the new resource
	// allocation performed
	status_view = sched_status_view;
	return SCHED_DONE;

error:
	ra.PutView(status_view);
	return SCHED_ERROR;

}

SchedulerPolicyIF::ExitCode_t TempuraSchedPol::ComputeBudgets() {

	for (auto & pb_entry: power_budgets) {
		br::ResourcePathPtr_t const & r_path(pb_entry.first);
		br::UsagePtr_t & budget(pb_entry.second);

		uint32_t p_budget = GetPowerBudget(r_path);
		budget->SetAmount(p_budget);

		uint64_t r_budget = GetResourceBudget(r_path);
		resource_budgets[r_path]->SetAmount(r_budget);
		logger->Info("Budget: [%s] has a budget of %" PRIu64 "",
				r_path->ToString().c_str(),
				resource_budgets[r_path]->GetAmount());
	}

	return SCHED_OK;
}

uint32_t TempuraSchedPol::GetPowerBudget(br::ResourcePathPtr_t const & rp) {
	(void) rp;
	return 5;

}

int64_t TempuraSchedPol::GetResourceBudget(br::ResourcePathPtr_t const & rp) {
	(void) rp;
	return 80;

}

SchedulerPolicyIF::ExitCode_t TempuraSchedPol::DoResourcePartitioning() {
	ba::AppCPtr_t papp;
	AppsUidMapIt app_it;

	// Ready applications
	papp = sys->GetFirstReady(app_it);
	for (; papp; papp = sys->GetNextReady(app_it)) {
		AssignWorkingMode(papp);
	}

	// Running applications
	papp = sys->GetFirstRunning(app_it);
	for (; papp; papp = sys->GetNextRunning(app_it)) {
		AssignWorkingMode(papp);
	}

	return SCHED_OK;
}

bool TempuraSchedPol::CheckSkip(ba::AppCPtr_t const & papp) {
	if (!papp->Active() && !papp->Blocking()) {
		logger->Debug("Skipping [%s] State = [%s, %s]",
				papp->StrId(),
				ApplicationStatusIF::StateStr(papp->State()),
				ApplicationStatusIF::SyncStateStr(papp->SyncState()));
		return true;
	}

	// Avoid double AWM selection for already RUNNING applications
	if ((papp->State() == Application::RUNNING) && papp->NextAWM()) {
		logger->Debug("Skipping [%s] AWM %d => No reconfiguration",
				papp->StrId(), papp->CurrentAWM()->Id());
		return true;
	}

	// Avoid double AWM selection for already SYNC applications
	if ((papp->State() == Application::SYNC) && papp->NextAWM()) {
		logger->Debug("Skipping [%s] AWM already assigned [%d]",
				papp->StrId(), papp->NextAWM()->Id());
		return true;
	}

	return false;
}


SchedulerPolicyIF::ExitCode_t
TempuraSchedPol::AssignWorkingMode(ba::AppCPtr_t papp) {
	uint64_t resource_slot;
	uint64_t resource_amount;

	// Build a new working mode featuring assigned resources
	ba::AwmPtr_t pawm(
			new ba::WorkingMode(
				papp->WorkingModes().size(), "Run-time", 1));
	pawm->SetOwner(papp);

	// Resource assignment (from each binding domain)
	for (auto & rb_entry: resource_budgets) {
		br::ResourcePathPtr_t const & r_path(rb_entry.first);
		br::UsagePtr_t & resource_budget(rb_entry.second);

		// Slots to allocate for this resource binding domain
		logger->Debug("Assign: Resource [%s] budget = % " PRIu64 "",
				r_path->ToString().c_str(), resource_budget->GetAmount());
		resource_slot  = resource_budget->GetAmount() / slots;
		logger->Debug("Assign: [%s] rslots of [%s] assigned = %4d",
				papp->StrId(), r_path->ToString().c_str(), resource_slot);
		resource_amount =
				(sys->ApplicationLowestPriority() - papp->Priority() + 1) *
				resource_slot;
		logger->Debug("Assign: [%s] amount of [%s] assigned = %4d",
				papp->StrId(), r_path->ToString().c_str(), resource_amount);
		pawm->AddResourceUsage(r_path->ToString(), resource_amount);
	}

	// Enqueue scheduling entity
	SchedEntityPtr_t psched(new SchedEntity_t(papp, pawm, R_ID_ANY, 0));
	entities.push_back(psched);

	return SCHED_OK;
}

SchedulerPolicyIF::ExitCode_t TempuraSchedPol::DoScheduling() {
	SchedulerPolicyIF::ExitCode_t result;
	Application::ExitCode_t app_result = Application::APP_SUCCESS;
	logger->Debug("DoScheduling: START");

	for (SchedEntityPtr_t & psched: entities) {
		// Bind the assigned resources and try to schedule
		result = DoBinding(psched);
		if (result != SCHED_OK) {
			logger->Error("DoScheduling: [%s] skipping scheduling", psched->StrId());
			continue;
		}

		// Check application status
		if (CheckSkip(psched->papp)) {
			logger->Debug("DoScheduling: [%s] skipping status", psched->StrId());
			continue;
		}

		// Scheduling request
		logger->Info("DoScheduling: [%s] scheduling request", psched->StrId());
		app_result = psched->papp->ScheduleRequest(
				psched->pawm, sched_status_view, psched->bind_refn);
		if (app_result != ApplicationStatusIF::APP_WM_ACCEPTED) {
			logger->Error("DoScheduling: [%s] failed", psched->StrId());
			continue;
		}
		logger->Debug("DoScheduling: [%s] success", psched->StrId());
	}
	logger->Debug("DoScheduling: STOP");
	return SCHED_OK;
}

SchedulerPolicyIF::ExitCode_t TempuraSchedPol::DoBinding(
		SchedEntityPtr_t psched) {
	logger->Debug("DoBinding: START");
	size_t ref_n = 0;

	BindingMap_t & bindings(ra.GetBindingOptions());
	for (auto & bd_entry: bindings) {
		BindingInfo_t const & bd_info(*(bd_entry.second));
		br::ResourceIdentifier::Type_t bd_type = bd_entry.first;
		// CPU, GPU level binding
		// Resource path e.g., "sys0.cpu[0..n].XX"
		for (br::ResourcePtr_t const & rsrc: bd_info.rsrcs) {
			br::ResID_t bd_id = rsrc->ID();
			logger->Debug("DoBinding: [%s] binding to %s%d",
					psched->StrId(), br::ResourceIdentifier::TypeStr[bd_type], bd_id);

			ref_n = psched->pawm->BindResource(bd_type, bd_id, bd_id, ref_n);
			logger->Debug("DoBinding: [%s] reference number %ld",
					psched->StrId(), ref_n);

			// The resource binding should never fail
			if (ref_n == 0) {
				logger->Error("DoBinding: [%d] on '%s%d' failed",
						psched->StrId(),
						br::ResourceIdentifier::TypeStr[bd_type], bd_id);
				return SCHED_ERROR;
			}
		}

		// Set scheduled binding reference number
		psched->bind_refn = ref_n;
	}

	logger->Debug("DoBinding: STOP");
	return SCHED_OK;
}


} // namespace plugins

} // namespace bbque
