// application.cc

#include "rpc_messages.pb.h"
#include "message_factory.h"
#include "response_factory.h"
#include "fifo.h"
#include "fifo_monitor.h"
#include "utils.h"
#include <random>
#include <functional>

using namespace std;
using namespace std::placeholders;


int main(int argc, char *argv[]) {
    Fifo out_fifo("/tmp/fifo_a2b");
    Fifo in_fifo("/tmp/fifo_b2a");
    shared_ptr<rpc_msg> msg;
    string input;
    FifoMonitor fifoMonitor(in_fifo, bind(ResponseFactory::resp, _1, ref(out_fifo)));
    function<int ()> rand = get_rand();
    function<unsigned int ()> urand = get_urand();
    do {
        // cout << ">>> ";
        getline(cin, input);
        if (input == "app_pair") {
            msg = MessageFactory::app_pair(urand(), urand(), urand(), "Hello, world!");
        } else if (input == "app_exit") {
            msg = MessageFactory::app_exit(urand());
        } else if (input == "exc_register") {
            msg = MessageFactory::exc_register(urand(), urand(), "Hello, world!", "Hello, world!", RTLIB_LANG_CPP);
        } else if (input == "exc_unregister") {
            msg = MessageFactory::exc_unregister(urand(), urand(), "Hello, world!");
        } else if (input == "exc_set") {
            msg = MessageFactory::exc_set(urand(), urand());
            MessageFactory::exc_set_add_constraint(msg, urand(), CONSTRAINT_ADD, LOWER_BOUND);
        } else if (input == "exc_clear") {
            msg = MessageFactory::exc_clear(urand(), urand());
        } else if (input == "exc_rtnotify") {
            msg = MessageFactory::exc_rtnotify(urand(), urand(), rand(), rand(), rand());
        } else if (input == "exc_start") {
            msg = MessageFactory::exc_start(urand(), urand());
        } else if (input == "exc_stop") {
            msg = MessageFactory::exc_stop(urand(), urand());
        } else if (input == "exc_schedule") {
            msg = MessageFactory::exc_schedule(urand(), urand());
        } else if (input == "exit") {
            msg = nullptr;
        } else {
            cout << "Invalid message" << endl;
            msg = nullptr;
        }
        if (msg != nullptr)
            out_fifo.send_msg(msg);
    } while (input != "exit");
    return 0;
}
