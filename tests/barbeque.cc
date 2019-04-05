// barbeque.cc

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
    Fifo out_fifo("/tmp/fifo_b2a");
    Fifo in_fifo("/tmp/fifo_a2b");
    shared_ptr<rpc_msg> msg;
    string input;
    FifoMonitor fifoMonitor(in_fifo, bind(ResponseFactory::resp, _1, ref(out_fifo)));
    function<int ()> rand = get_rand();
    function<unsigned int ()> urand = get_urand();
    do {
        // cout << ">>> ";
        getline(cin, input);
        if (input == "bbq_syncp_postchange") {
            msg = MessageFactory::bbq_syncp_postchange(urand());
        } else if (input == "bbq_syncp_dochange") {
            msg = MessageFactory::bbq_syncp_dochange(urand());
        } else if (input == "bbq_syncp_syncchange") {
            msg = MessageFactory::bbq_syncp_syncchange(urand());
        } else if (input == "bbq_syncp_prechange") {
            msg = MessageFactory::bbq_syncp_prechange(urand(), urand(), rand(), urand(), urand(), urand());
        } else if (input == "bbq_stop_execution") {
            msg = MessageFactory::bbq_stop(urand(), urand());
        } else if (input == "bbq_get_profile") {
            msg = MessageFactory::bbq_get_profile(urand(), false);
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
