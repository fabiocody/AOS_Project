// barbeque.cc

#include "fifo.h"
#include "fifo_monitor.h"
#include "message_factory.h"
#include "rpc_messages.pb.h"

using namespace std;


int main() {
    Fifo<rpc_msg> out_fifo("/tmp/fifo_b2a");
    Fifo<rpc_msg_resp> in_fifo("/tmp/fifo_a2b");
    rpc_msg msg;
    rpc_msg_resp resp;
    string input;
    FifoMonitor<rpc_msg_resp> fifoMonitor(in_fifo);
    do {
        cout << ">>> ";
        getline(cin, input);
        // TODO: choose message
        out_fifo.send_msg(msg);
    } while (input != "exit");
    return 0;
}