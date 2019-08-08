// response_factory.cc

#include "response_factory.h"
#include "message_factory.h"
#include "utils.h"


void ResponseFactory::resp(std::shared_ptr<rpc_msg> msg, Fifo & fifo) {
    std::shared_ptr<rpc_msg> resp;
    uint32_t type = msg->hdr().typ();
    uint32_t token = msg->hdr().token();
    uint32_t app_pid = msg->hdr().app_pid();
    uint8_t exc_id = msg->hdr().exc_id();
    if (type >= RPC_APP_PAIR && type <= RPC_APP_EXIT)
        resp = MessageFactory::generic_msg(RPC_APP_RESP, token, app_pid, exc_id);
    else if (type >= RPC_EXC_SCHEDULE && type <= RPC_EXC_UNREGISTER)
        resp = MessageFactory::generic_msg(RPC_EXC_RESP, token, app_pid, exc_id);
    else if (type >= RPC_BBQ_SYNCP_POSTCHANGE && type <= RPC_BBQ_GET_PROFILE)
        resp = bbq_resp(msg);
    else
        resp = nullptr;
    if (resp != nullptr)
        fifo.send_msg(resp);
}


std::shared_ptr<rpc_msg> ResponseFactory::bbq_resp(std::shared_ptr<rpc_msg> msg) {
    std::shared_ptr<rpc_msg> resp;
    uint32_t type = msg->hdr().typ();
    uint32_t token = msg->hdr().token();
    uint32_t app_pid = msg->hdr().app_pid();
    uint8_t exc_id = msg->hdr().exc_id();
    std::function<unsigned int ()> urand = get_urand();
    if (type == RPC_BBQ_SYNCP_PRECHANGE) {
        resp = MessageFactory::bbq_syncp_prechange_resp(token, app_pid, exc_id, urand(), urand());
    } else if (type == RPC_BBQ_SYNCP_SYNCCHANGE) {
        resp = MessageFactory::bbq_syncp_syncchange_resp(token, app_pid, exc_id, urand());
    } else if (type == RPC_BBQ_SYNCP_POSTCHANGE) {
        resp = MessageFactory::bbq_syncp_postchange_resp(token, app_pid, exc_id, urand());
    } else {
        resp = MessageFactory::generic_msg(RPC_BBQ_RESP, token, app_pid, exc_id);
    }
    return resp;
}
