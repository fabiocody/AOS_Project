// fifo_monitor.h

#ifndef AOS_PROJECT_FIFO_MONITOR_H
#define AOS_PROJECT_FIFO_MONITOR_H

#include <thread>
#include <atomic>
#include "fifo.h"
#include "synchronized_queue.h"


template <class T>
class FifoMonitor {

private:
    std::thread t;
    std::atomic<bool> quit;
    Fifo<T> & fifo;

    void run() {
        while (!quit) {
            std::shared_ptr<T> msg = fifo.recv_msg();
            queue.put(msg);
            dout << msg->DebugString() << std::endl;
        }
    }

public:
    SynchronizedQueue<std::shared_ptr<T>> queue;
    FifoMonitor(const FifoMonitor &)=delete;
    FifoMonitor& operator=(const FifoMonitor &)=delete;

    explicit FifoMonitor(Fifo<T> & fifo): t(&FifoMonitor::run, this), quit(false), fifo(fifo) {}

    ~FifoMonitor() {
        if (quit) return;
        quit = true;
        T msg;
        fifo.send_msg(msg);
        t.join();
    }

};


#endif //AOS_PROJECT_FIFO_MONITOR_H
