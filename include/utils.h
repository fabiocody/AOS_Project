// utils.h

#ifndef AOS_PROJECT_UTILS_H
#define AOS_PROJECT_UTILS_H

#include <iostream>
#include <random>
#include <rpc_messages.pb.h>


#ifdef DEBUG
    #define dout std::cout << "DEBUG: "
#else
    #define dout 0 && std::cout
#endif


std::function<int ()> get_rand();
std::function<unsigned int ()> get_urand();
void print_msg(std::shared_ptr<rpc_msg> msg);


#define	RPC_APP_PAIR        0   
#define	RPC_APP_EXIT        1

#define	RPC_APP_RESP        2
#define	RPC_APP_MSGS_COUNT  3

#define	RPC_EXC_SCHEDULE    4
#define	RPC_EXC_START       5
#define	RPC_EXC_SET         6
#define	RPC_EXC_CLEAR       7
#define	RPC_EXC_STOP        8
#define	RPC_EXC_REGISTER    9
#define	RPC_EXC_RTNOTIFY    10
#define	RPC_EXC_UNREGISTER  11

#define	RPC_EXC_RESP        12
#define	RPC_EXC_MSGS_COUNT  13

#define	RPC_BBQ_SYNCP_POSTCHANGE    14
#define	RPC_BBQ_SYNCP_DOCHANGE      15
#define	RPC_BBQ_SYNCP_SYNCCHANGE    16
#define	RPC_BBQ_SYNCP_PRECHANGE     17

#define	RPC_BBQ_STOP_EXECUTION      18
#define	RPC_BBQ_GET_PROFILE         19

#define	RPC_BBQ_RESP        20
#define RPC_BBQ_MSGS_COUNT  21


#endif //AOS_PROJECT_UTILS_H
