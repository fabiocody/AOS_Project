// utils.cc

#include "utils.h"


std::function<int ()> get_rand() {
    unsigned int seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(-100, 100);
    std::function<int()> rand = bind(distribution, generator);
    return rand;
}


std::function<unsigned int ()> get_urand() {
    unsigned int seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 100);
    std::function<int()> rand = bind(distribution, generator);
    return rand;
}


void print_msg(std::shared_ptr<rpc_msg> msg) {
    std::cout << msg->DebugString() << std::endl;
}

