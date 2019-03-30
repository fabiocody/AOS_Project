// fifo_monitor.cc

#include "fifo_monitor.h"
#include <utility>


FifoMonitor::FifoMonitor(Fifo & fifo, std::function<void(std::shared_ptr<rpc_msg>)> callback):
    quit(false),
    fifo(fifo),
    callback(std::move(callback)),
    t(&FifoMonitor::run, this) {}


FifoMonitor::~FifoMonitor() {
    if (quit) return;
    quit = true;
    rpc_msg msg;
    fifo.send_msg(msg);
    t.join();
}


void FifoMonitor::run() {
    while (!quit) {
        std::shared_ptr<rpc_msg> msg = fifo.recv_msg();
        if (callback != nullptr)
            callback(msg);
    }
}


void FifoMonitor::set_callback(std::function<void(std::shared_ptr<rpc_msg>)> callback) {
    this->callback = std::move(callback);
}
