// message_factory.h

#ifndef AOS_PROJECT_MESSAGE_FACTORY_H
#define AOS_PROJECT_MESSAGE_FACTORY_H

#include <memory>
#include "rpc_messages.pb.h"


class MessageFactory {

public:
    /** Generic */
    static std::shared_ptr<rpc_msg> generic_msg(rpc_msg_type typ, uint32_t token);
    static std::shared_ptr<rpc_msg> generic_exc_msg(rpc_msg_type typ, uint32_t token, uint32_t exc_id);
    static std::shared_ptr<rpc_msg_resp> generic_response(rpc_msg_type typ, uint32_t token);

    /**  */
    static std::shared_ptr<rpc_msg> app_pair(uint32_t token, uint32_t mjr_version, uint32_t mnr_version, std::string app_name);
    static std::shared_ptr<rpc_msg> app_exit(uint32_t token);
    static std::shared_ptr<rpc_msg> exc_register(uint32_t token, uint32_t exc_id, std::string exc_name, std::string recipe, RTLIB_ProgrammingLanguage lang);
    static std::shared_ptr<rpc_msg> exc_unregister(uint32_t token, uint32_t exc_id, std::string exc_name);
    static std::shared_ptr<rpc_msg> exc_set(uint32_t token, uint32_t exc_id, uint32_t count, RTLIB_Constraint *constraints);
    static std::shared_ptr<rpc_msg> exc_clear(uint32_t token, uint32_t exc_id);
    static std::shared_ptr<rpc_msg> exc_rtnotify(uint32_t token, uint32_t exc_id, int32_t gap, int32_t cusage, int32_t ctime_ms);
    static std::shared_ptr<rpc_msg> exc_start(uint32_t token, uint32_t exc_id);
    static std::shared_ptr<rpc_msg> exc_stop(uint32_t token, uint32_t exc_id);
    static std::shared_ptr<rpc_msg> exc_schedule(uint32_t token, uint32_t exc_id);
    static std::shared_ptr<rpc_msg> bbq_syncp_prechange(uint32_t token, uint32_t event, int32_t awm, uint64_t cpu_ids, uint64_t cpu_ids_isolation, uint64_t mem_ids);
    void bbq_syncp_prechange_system(std::shared_ptr<rpc_msg> msg, int32_t sys_id, int32_t nr_cpus, int32_t nr_procs, int32_t r_proc, int32_t r_mem, int32_t r_gpu, int32_t r_acc, int32_t dev);
    static std::shared_ptr<rpc_msg_resp> bbq_syncp_prechange_resp(uint32_t token, uint32_t synclatency, uint32_t result);
    static std::shared_ptr<rpc_msg> bbq_syncp_syncchange(uint32_t token);
    static std::shared_ptr<rpc_msg_resp> bbq_syncp_syncchange_resp(uint32_t token, uint32_t result);
    static std::shared_ptr<rpc_msg> bbq_syncp_dochange(uint32_t token);
    static std::shared_ptr<rpc_msg> bbq_syncp_postchange(uint32_t token);
    static std::shared_ptr<rpc_msg_resp> bbq_syncp_postchange_resp(uint32_t token, uint32_t result);
    static std::shared_ptr<rpc_msg> bbq_stop(uint32_t token, uint32_t timeout);
    static std::shared_ptr<rpc_msg> bbq_get_profile(uint32_t token, bool is_ocl);

};


#endif //AOS_PROJECT_MESSAGE_FACTORY_H
