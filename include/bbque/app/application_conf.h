/**
 *       @file  application_conf.h
 *      @brief  Application updating status interface
 *
 * This defines the interfaces for updating runtime information of the
 * application as priority, scheduled status and next working mode
 *
 *     @author  Giuseppe Massari (jumanix), joe.massanga@gmail.com
 *
 *   @internal
 *     Created  04/04/2011
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *     Company  Politecnico di Milano
 *   Copyright  Copyright (c) 2011, Giuseppe Massari
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * ============================================================================
 */

#ifndef BBQUE_APPLICATION_CONF_IF_H_
#define BBQUE_APPLICATION_CONF_IF_H_

#include "bbque/app/application_status.h"

namespace bbque { namespace res {
/** Numeric value used as token for the resource views */
typedef size_t RViewToken_t;
}}

using bbque::res::RViewToken_t;

namespace bbque { namespace app {

/**
 * @class ApplicationConfIF
 * @brief Provide interfaces to update runtime application information of the
 * application as priority, scheduled status and next working mode
 */
class ApplicationConfIF: public ApplicationStatusIF {

public:

	/**
	 * @brief Set the priority of the application
	 */
	virtual void SetPriority(AppPrio_t prio) = 0;

	/**
	 * @brief Enable the application for resources assignment
	 *
	 * A newly created application is disabled by default, thus it will not be
	 * considered for resource scheduling until it is enabled.
	 */
	virtual ExitCode_t Enable() = 0;

	/**
	 * @brief Disabled the application for resources assignment
	 *
	 * A disabled application will not be considered for resources scheduling.
	 */

	virtual ExitCode_t Disable() = 0;

	/**
	 * @brief Set next scheduled working mode
	 *
	 * Let the Scheduler/Optimizer module to new working mode of the
	 * application. The method infers if the scheduling choice implies a
	 * reconfiguration (change of working mode), a migration (move the
	 * application from a cluster to another), both, or to continue in the
	 * same working mode, in the same cluster scope.
	 *
	 * @param awm Next working mode scheduled for the application
	 * @param tok The token referencing the resources state view
	 */
	virtual ExitCode_t SetNextSchedule(AwmPtr_t & awm,
			RViewToken_t tok = 0) = 0;

};

} // namespace app

} // namespace bbque

#endif // BBQUE_APPLICATION_CONF_IF_H_

