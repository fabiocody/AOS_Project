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

#ifndef BBQUE_RESOURCE_BINDER_H_
#define BBQUE_RESOURCE_BINDER_H_

#include <cstdint>

#include "bbque/res/resource_assignment.h"
#include "bbque/utils/logging/logger.h"

namespace bbque { namespace res {

class ResourceBitset;

/**
 * @class ResourceBinder
 *
 * This a static class providing methods to solve or provide information about
 * resource bindings.
 *
 * The binding is a mandatory step, through which a scheduling policy links
 * the resources requested in the AWMs (referenced by a resource path built
 * from the recipe) to the system resources (referenced by the resource path
 * registered by the Platform Proxy.
 */
class ResourceBinder {

public:

	/**
	 * @enum Exit codes
	 */
	enum ExitCode_t {
		OK         = 0 ,
		NOT_COMPATIBLE
	};

	/**
	 * @brief Bind resource assignments to system resources
	 *
	 * @param source_map The map of resource assignments to bind
	 * @param r_type The type of resource to bind
	 * @param src_r_id The ID of the resource to bind
	 * @param dst_r_id The ID of the system resource to which bind
	 * @param out_map A shared pointer to the map of bound resources to fill
	 *
	 * @param filter_rtype [optional] Type of resource to filter
	 * @param filter_mask  [optional] IDs of the resources to include in the output
	 * UsagesMap
	 *
	 * @return The number of resources on which the binding has been performed
	 */
	static uint32_t Bind(
			ResourceAssignmentMap_t const & source_map,
			ResourceType r_type,
			BBQUE_RID_TYPE src_r_id,
			BBQUE_RID_TYPE dst_r_id,
			ResourceAssignmentMapPtr_t out_map,
			ResourceType filter_rtype =
				ResourceType::UNDEFINED,
			ResourceBitset * filter_mask = nullptr);

	/**
	 * @brief Retrieve IDs of a type of resource from a ResourceAssignmentMap_t
	 *
	 * @param assign_map A shared pointer to the map of resource assignments
	 * @param r_type The type of resource to consider
	 *
	 * @return A ResourceBitset object tracking all the IDs
	 */
	static ResourceBitset GetMask(
			ResourceAssignmentMapPtr_t assign_map, ResourceType r_type);

	static ResourceBitset GetMask(
			ResourceAssignmentMap_t const & assign_map, ResourceType r_type);

	/**
	 * @brief Retrieve IDs of a type of resource under a scope
	 *
	 * <tt>
	 * Example:<br>
	 *
	 * Type:       PROC_ELEMENT
	 * Scope type: CPU
	 * Scope ID:   2
	 *
	 * Matching resource paths:<br>
	 *
	 * "sys0.cpu2.pe0"
	 * "sys0.cpu2.pe1"
	 * "sys0.cpu2.pe2"
	 * "sys0.cpu2.pe..."
	 *
	 * </tt>
	 *
	 * @param assign_map A shared pointer to the map of resource assignments
	 * @param r_type The target type of resource
	 * @param r_type_scope The type of the scope resource
	 * @param r_type_id The ID of the scope resource
	 * @param papp [optional] The application using the resource
	 * @param vtok [optional] The resource state view to consider
	 *
	 * @return A ResourceBitset object tracking all the IDs
	 */
	static ResourceBitset GetMask(
			ResourceAssignmentMapPtr_t assign_map,
			ResourceType r_type,
			ResourceType r_scope_type,
			BBQUE_RID_TYPE r_scope_id,
			AppSPtr_t papp = nullptr,
			RViewToken_t vtok = 0);

	/**
	 * @brief Retrieve IDs of a type of resource from a ResourcePtrList_t
	 *
	 * @param rpl A list of resource descriptors
	 * @param r_type The type of resource to consider
	 * @param papp [optional] The application using the resource
	 * @param vtok [optional] The resource state view to consider
	 *
	 * @return A ResourceBitset object tracking all the IDs
	 */
	static ResourceBitset GetMask(
			ResourcePtrList_t const & rpl,
			ResourceType r_type,
			ResourceType r_scope_type,
			BBQUE_RID_TYPE r_scope_id,
			AppSPtr_t papp = nullptr,
			RViewToken_t vtok = 0);

	/**
	 * @brief Check if two resource assignments map are compatible for binding
	 *
	 * @param source_map A shared pointer to the first map of resource assignments
	 * @param out_map A shared pointer to the second map of resource assignments
	 *
	 * @return OK if compatible, NOT_COMPATIBLE otherwise
	 */
	static ExitCode_t Compatible(
			ResourceAssignmentMapPtr_t source_map,
			ResourceAssignmentMapPtr_t out_map);

};


} // namespace res

} // namespace bbque

#endif // BBQUE_RESOURCE_BINDER_H_


