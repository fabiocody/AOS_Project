// consumer.cc

#include "test_msg.pb.h"
#include "fifo.h"

#include <memory>

using namespace std;


int main() {
    string fifoname = "/tmp/testfifo";
    const char *c_fifoname = fifoname.c_str();
    Fifo<TestMessage> fifo(c_fifoname);
    shared_ptr<TestMessage> msg;
    do {
        msg = fifo.recv_msg();
        cout << ">>> " << msg->str() << endl;
    } while (msg->str() != "exit");
    msg->set_str("Nel mezzo del cammin di nostra vita");
    cout << ">>> " << msg->str() << endl;
    fifo.send_msg(*msg);
    return 0;
}