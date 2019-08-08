// response_factory.h

#ifndef AOS_PROJECT_RESPONSE_FACTORY_H
#define AOS_PROJECT_RESPONSE_FACTORY_H

#include "rpc_messages.pb.h"
#include "fifo.h"
#include <memory>


class ResponseFactory {

private:
    static std::shared_ptr<rpc_msg> bbq_resp(std::shared_ptr<rpc_msg> msg);

public:
    static void resp(std::shared_ptr<rpc_msg> msg, Fifo&);

};


#endif //AOS_PROJECT_RESPONSE_FACTORY_H
