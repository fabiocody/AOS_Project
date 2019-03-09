// test.cc

#include <memory>
#include "utils.h"
#include "message_factory.h"

using namespace std;


int main() {
    dout << "Hello, world!" << ' ' << 42 << endl;
    std::shared_ptr<rpc_msg> msg = MessageFactory::bbq_stop(42, 100);
    cout << msg->DebugString() << endl;
    return 0;
}

