// test.cc

#include <memory>
#include "utils.h"
#include "message_factory.h"
#include "utils.h"

using namespace std;


int main() {
    dout << "Hello, world!" << ' ' << 42 << endl;
    function<int ()> rand = get_rand();
    for (unsigned int i = 0; i < 100; i++)
        cout << "Random number: " << rand() << endl;
    cout << endl;
    shared_ptr<rpc_msg> msg = MessageFactory::bbq_stop(42, 42, 42, 100);
    cout << msg->DebugString() << endl;
    return 0;
}
