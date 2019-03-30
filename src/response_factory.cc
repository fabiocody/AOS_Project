// response_factory.cc

#include "response_factory.h"
#include "message_factory.h"
#include "utils.h"


void ResponseFactory::resp(std::shared_ptr<rpc_msg> msg, Fifo & fifo) {
    std::shared_ptr<rpc_msg> resp;
    rpc_msg_type type = msg->hdr().typ();
    if (type >= RPC_APP_PAIR && type <= RPC_APP_EXIT)
        resp = app_resp(msg);
    else if (type >= RPC_EXC_SCHEDULE && type <= RPC_EXC_UNREGISTER)
        resp = exc_resp(msg);
    else if (type >= RPC_BBQ_SYNCP_POSTCHANGE && type <= RPC_BBQ_GET_PROFILE)
        resp = bbq_resp(msg);
    else
        resp = nullptr;
    if (resp != nullptr)
        fifo.send_msg(resp);
}


std::shared_ptr<rpc_msg> ResponseFactory::app_resp(std::shared_ptr<rpc_msg> msg) {
    std::shared_ptr<rpc_msg> resp = MessageFactory::generic_msg(RPC_APP_RESP, msg->hdr().token());
    return resp;
}


std::shared_ptr<rpc_msg> ResponseFactory::exc_resp(std::shared_ptr<rpc_msg> msg) {
    std::shared_ptr<rpc_msg> resp = MessageFactory::generic_msg(RPC_EXC_RESP, msg->hdr().token());
    return resp;
}


std::shared_ptr<rpc_msg> ResponseFactory::bbq_resp(std::shared_ptr<rpc_msg> msg) {
    std::shared_ptr<rpc_msg> resp;
    rpc_msg_type type = msg->hdr().typ();
    uint32_t token = msg->hdr().token();
    std::function<unsigned int ()> urand = get_urand();
    if (type == RPC_BBQ_SYNCP_PRECHANGE) {
        resp = MessageFactory::bbq_syncp_prechange_resp(token, urand(), urand());
    } else if (type == RPC_BBQ_SYNCP_SYNCCHANGE) {
        resp = MessageFactory::bbq_syncp_syncchange_resp(token, urand());
    } else if (type == RPC_BBQ_SYNCP_POSTCHANGE) {
        resp = MessageFactory::bbq_syncp_postchange_resp(token, urand());
    } else {
        resp = MessageFactory::generic_msg(RPC_BBQ_RESP, token);
    }
    return resp;
}
