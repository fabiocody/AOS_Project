// fifo_monitor.h

#ifndef AOS_PROJECT_FIFO_MONITOR_H
#define AOS_PROJECT_FIFO_MONITOR_H

#include <thread>
#include <atomic>
#include "fifo.h"


class FifoMonitor {

private:
    std::thread t;
    std::atomic<bool> quit;
    Fifo & fifo;
    std::function<void(std::shared_ptr<rpc_msg>)> callback;
    void run();

public:
    FifoMonitor(const FifoMonitor &)=delete;
    FifoMonitor& operator=(const FifoMonitor &)=delete;
    FifoMonitor(Fifo & fifo, std::function<void(std::shared_ptr<rpc_msg>)> callback);
    ~FifoMonitor();
    void set_callback(std::function<void(std::shared_ptr<rpc_msg>)> callback);

};


#endif //AOS_PROJECT_FIFO_MONITOR_H
