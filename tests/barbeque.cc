// barbeque.cc

#include "fifo.h"
#include "message_factory.h"
#include "rpc_messages.pb.h"

using namespace std;


int main() {
    Fifo<rpc_msg> out_fifo("/tmp/fifoba");
    Fifo<rpc_msg_resp> in_fifo("/tmp/fifoab");
    rpc_msg msg;
    rpc_msg_resp resp;
    string input;
    do {
        cout << ">>> ";
        getline(cin, input);
        // TODO: choose message
        out_fifo.send_msg(msg);
    } while (input != "exit");
    return 0;
}