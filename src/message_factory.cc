// message_factory.cc

#include "message_factory.h"
#include "utils.h"


std::shared_ptr<rpc_msg> MessageFactory::generic_msg(uint32_t typ, uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = std::make_shared<rpc_msg>();
    msg->mutable_hdr()->set_typ(typ);
    msg->mutable_hdr()->set_token(token);
    msg->mutable_hdr()->set_app_pid(app_pid);
    msg->mutable_hdr()->set_exc_id(exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::app_pair(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t mjr_version, uint32_t mnr_version, std::string app_name) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_APP_PAIR, token, app_pid, exc_id);
    msg->set_mjr_version(mjr_version);
    msg->set_mnr_version(mnr_version);
    msg->set_app_name(app_name);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::app_exit(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_APP_EXIT, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_register(uint32_t token, uint32_t app_pid, uint8_t exc_id, std::string exc_name, std::string recipe, uint32_t lang) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_REGISTER, token, app_pid, exc_id);
    msg->set_exc_name(exc_name);
    msg->set_recipe(recipe);
    msg->set_lang(lang);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_unregister(uint32_t token, uint32_t app_pid, uint8_t exc_id, std::string exc_name) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_UNREGISTER, token, app_pid, exc_id);
    msg->set_exc_name(exc_name);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_set(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_SET, token, app_pid, exc_id);
    return msg;
}

void MessageFactory::exc_set_add_constraint(std::shared_ptr<rpc_msg> msg, uint32_t awm, uint32_t op, uint32_t type) {
    Constraint *constraint = msg->add_constraints();
    constraint->set_awm(awm);
    constraint->set_operation(op);
    constraint->set_type(type);
}

std::shared_ptr<rpc_msg> MessageFactory::exc_clear(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_CLEAR, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_rtnotify(uint32_t token, uint32_t app_pid, uint8_t exc_id, int32_t gap, int32_t cusage, int32_t ctime_ms) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_RTNOTIFY, token, app_pid, exc_id);
    msg->set_gap(gap);
    msg->set_cusage(cusage);
    msg->set_ctime_ms(ctime_ms);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_start(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_START, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_stop(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_STOP, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::exc_schedule(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_EXC_SCHEDULE, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_prechange(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t event, int32_t awm, uint64_t cpu_ids, uint64_t cpu_ids_isolation, uint64_t mem_ids) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_SYNCP_PRECHANGE, token, app_pid, exc_id);
    msg->set_event(event);
    msg->set_awm(awm);
    msg->set_cpu_ids(cpu_ids);
    msg->set_cpu_ids_isolation(cpu_ids_isolation);
    msg->set_mem_ids(mem_ids);
    return msg;
}

void MessageFactory::bbq_syncp_prechange_system(std::shared_ptr<rpc_msg> msg, int32_t sys_id, int32_t nr_cpus, int32_t nr_procs, int32_t r_proc, int32_t r_mem, int32_t r_gpu, int32_t r_acc, int32_t dev) {
    rpc_msg_BBQ_SYNCP_PRECHANGE_SYSTEM *system = msg->add_systems();
    system->set_sys_id(sys_id);
    system->set_nr_cpus(nr_cpus);
    system->set_nr_procs(nr_procs);
    system->set_r_proc(r_proc);
    system->set_r_mem(r_mem);
    system->set_r_gpu(r_gpu);
    system->set_r_acc(r_acc);
    system->set_dev(dev);
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_prechange_resp(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t synclatency, uint32_t result) {
    std::shared_ptr<rpc_msg> resp = generic_msg(RPC_BBQ_RESP, token, app_pid, exc_id);
    resp->set_synclatency(synclatency);
    resp->set_result(result);
    return resp;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_syncchange(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_SYNCP_SYNCCHANGE, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_syncchange_resp(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t result) {
    std::shared_ptr<rpc_msg> resp = generic_msg(RPC_BBQ_RESP, token, app_pid, exc_id);
    resp->set_result(result);
    return resp;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_dochange(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_SYNCP_DOCHANGE, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_postchange(uint32_t token, uint32_t app_pid, uint8_t exc_id) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_SYNCP_POSTCHANGE, token, app_pid, exc_id);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_syncp_postchange_resp(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t result) {
    std::shared_ptr<rpc_msg> resp = generic_msg(RPC_BBQ_RESP, token, app_pid, exc_id);
    resp->set_result(result);
    return resp;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_stop(uint32_t token, uint32_t app_pid, uint8_t exc_id, uint32_t timeout) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_STOP_EXECUTION, token, app_pid, exc_id);
    msg->set_timeout(timeout);
    return msg;
}

std::shared_ptr<rpc_msg> MessageFactory::bbq_get_profile(uint32_t token, uint32_t app_pid, uint8_t exc_id, bool is_ocl) {
    std::shared_ptr<rpc_msg> msg = generic_msg(RPC_BBQ_GET_PROFILE, token, app_pid, exc_id);
    msg->set_is_ocl(is_ocl);
    return msg;
}
