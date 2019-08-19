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

#ifndef BBQUE_RPC_FIFO_SERVER_H_
#define BBQUE_RPC_FIFO_SERVER_H_

#include "bbque/rtlib.h"

#include "bbque/config.h"
#include "bbque/rtlib/rpc/rpc_messages.h"
#include "bbque/utils/utility.h"

#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#define BBQUE_PUBLIC_FIFO "rpc_fifo"

#define BBQUE_FIFO_NAME_LENGTH 32

#define BBQUE_RPC_FIFO_MAJOR_VERSION 1
#define BBQUE_RPC_FIFO_MINOR_VERSION 0

#define FIFO_PKT_SIZE(RPC_TYPE)\
	sizeof(bbque::rtlib::rpc_fifo_ ## RPC_TYPE ## _t)
#define FIFO_PYL_OFFSET(RPC_TYPE)\
	offsetof(bbque::rtlib::rpc_fifo_ ## RPC_TYPE ## _t, pyl)

namespace bbque
{
namespace rtlib
{

/**
 * @brief The RPC FIFO message header
 */
typedef struct rpc_fifo_header {
	/** The bytes of the FIFO message */
	uint16_t fifo_msg_size;
	/** The offset of the RPC message start */
	uint8_t rpc_msg_offset;
	/** The type of the RPC message */
	uint8_t rpc_msg_type;
} rpc_fifo_header_t;

typedef struct rpc_fifo_GENERIC {
	/** The RPC fifo command header */
	rpc_fifo_header_t hdr;
	/** The RPC message payload */
	rpc_msg_header_t pyl;
} rpc_fifo_GENERIC_t;

#define RPC_FIFO_DEFINE_MESSAGE(RPC_TYPE)\
typedef struct rpc_fifo_ ## RPC_TYPE {\
	rpc_fifo_header_t hdr;\
	rpc_msg_ ## RPC_TYPE ## _t pyl;\
} rpc_fifo_ ## RPC_TYPE ## _t


/******************************************************************************
 * Channel Management
 ******************************************************************************/

/**
 * @brief An RPC_APP_PAIR FIFO command.
 *
 * This command is used by the FIFO communication channel to send the
 * application endpoit required to setup the communication channel.
 * The applicaiton end-point is defined by a fifo file node to be used to send
 * commands to the application.
 *
 * @note The only RPC command which contains communication channel specific
 * inforamtion is the RPC_EXC_PARI. All other commands maps on the
 * rpc_fifo_undef_t type.
 */
typedef struct rpc_fifo_APP_PAIR {
	/** The RPC fifo command header */
	rpc_fifo_header_t hdr;
	/** The name of the application private fifo */
	char rpc_fifo[BBQUE_FIFO_NAME_LENGTH];
	/** The RPC message payload */
	rpc_msg_APP_PAIR_t pyl;
} rpc_fifo_APP_PAIR_t;

RPC_FIFO_DEFINE_MESSAGE(APP_EXIT);


/******************************************************************************
 * Execution Context Requests
 ******************************************************************************/

RPC_FIFO_DEFINE_MESSAGE(EXC_REGISTER);
RPC_FIFO_DEFINE_MESSAGE(EXC_UNREGISTER);
RPC_FIFO_DEFINE_MESSAGE(EXC_SET);
RPC_FIFO_DEFINE_MESSAGE(EXC_CLEAR);
RPC_FIFO_DEFINE_MESSAGE(EXC_RTNOTIFY);
RPC_FIFO_DEFINE_MESSAGE(EXC_START);
RPC_FIFO_DEFINE_MESSAGE(EXC_STOP);
RPC_FIFO_DEFINE_MESSAGE(EXC_SCHEDULE);

//----- PreChange
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_PRECHANGE);
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_PRECHANGE_RESP);

//----- SyncChange
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_SYNCCHANGE);
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_SYNCCHANGE_RESP);

//----- DoChange
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_DOCHANGE);

//----- PostChange
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_POSTCHANGE);
RPC_FIFO_DEFINE_MESSAGE(BBQ_SYNCP_POSTCHANGE_RESP);


/******************************************************************************
 * Barbeque Commands
 ******************************************************************************/

RPC_FIFO_DEFINE_MESSAGE(BBQ_STOP);

RPC_FIFO_DEFINE_MESSAGE(BBQ_GET_PROFILE);
RPC_FIFO_DEFINE_MESSAGE(BBQ_GET_PROFILE_RESP);

/******************************************************************************
 * Utility Commands
 ******************************************************************************/
#define RPC_FIFO_HEX_DUMP_BUFFER(PBUFF, SIZE)\
do {\
fprintf(stderr, "\nRPC_FIFO_HEX_DUMP_BUFFER(@%p, %d):", PBUFF, SIZE);\
for (uint32_t i = 0; i < (SIZE); ++i) {\
	if (!(i % 16))\
		fprintf(stderr, "\n");\
	fprintf(stderr, "%02X ", ((uint8_t*)PBUFF)[i]);\
}\
fprintf(stderr, "\n");\
} while(0);

#define RPC_FIFO_HEX_DUMP_MESSAGE(RPC_TYPE)\
	RPC_FIFO_HEX_DUMP_BUFFER((void*)&rf_ ## RPC_TYPE, (int)FIFO_PKT_SIZE(RPC_TYPE))

} // namespace rtlib

} // namespace bbque

#endif // BBQUE_RPC_FIFO_SERVER_H_

