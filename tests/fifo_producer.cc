// fifo_producer.cc

#include "test_msg.pb.h"
#include "fifo.h"

#include <memory>

using namespace std;


int main() {
    string fifoname = "/tmp/testfifo";
    Fifo<TestMessage> fifo(fifoname);
    TestMessage msg;
    string input = "exit";
    do {
        cout << ">>> ";
        getline(cin, input);
        msg.set_str(input);
        fifo.send_msg(msg);
    } while (input != "exit");
    msg = *fifo.recv_msg();
    cout << ">>> " << msg.str() << endl;
    fifo.send_msg(msg);
    return 0;
}