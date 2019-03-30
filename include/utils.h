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


#endif //AOS_PROJECT_UTILS_H
