/*
 * Copyright (C) 2016  Politecnico di Milano
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
 *
 * Author: Federico Reghenzani <federico1.reghenzani@mail.polimi.it>
 */
#include <bitset>
#include <unordered_map>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"

#define MAX_ALLOWED_PES 256

namespace bbque {
namespace tools {


typedef struct plp_data_s {
	std::string  uid;
	std::string guid;
	std::string cpup;
	std::string plat_cpus;
	std::string plat_mems;
	std::string feat_cpuq;
	std::string feat_memc;
} plp_data_t;

class PLPTranslator {
public:

	PLPTranslator(const plp_data_t &data) : data(data) {}
	int parse(const std::string &filename) noexcept;

	std::string get_output() const noexcept;

private:
	const plp_data_t data;

	rapidxml::xml_document<>  systems_doc;
	rapidxml::xml_document<> localsys_doc;

	std::bitset<MAX_ALLOWED_PES> host_pes;
	std::bitset<MAX_ALLOWED_PES> mdev_pes;
	std::bitset<MAX_ALLOWED_PES> host_mems;
	std::bitset<MAX_ALLOWED_PES> mdev_mems;
	std::bitset<MAX_ALLOWED_PES> mdev_currentcpu_pes;
	std::bitset<MAX_ALLOWED_PES> mdev_currentcpu_mems;
	std::unordered_map<std::string, long> memories_size;

	std::string explore_systems (const std::string &filename);
	void        explore_localsys(const std::string &filename);
	void        add_pe(int type, const std::string &pe_id,
	                   const std::string &memory);

	void commit_mdev(const std::string &memory_id);
	std::string subnodes;   // sub cgroups
	int quota_sum=0;

	static std::string bitset_to_string(const std::bitset<MAX_ALLOWED_PES> &bs) noexcept;
};


} // tools
} // bbque
