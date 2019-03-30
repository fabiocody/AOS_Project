// fifo.h

#ifndef AOS_PROJECT_FIFO_H
#define AOS_PROJECT_FIFO_H

#include <memory>
#include <utility>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <rpc_messages.pb.h>
#include "utils.h"

#define CHUNK_SIZE 1024


class Fifo {

private:
    int fd;
    char *fifoname;
    size_t buffer_size;
    void *buffer;
    void buffer_allocation_error();

public:

    explicit Fifo(const char *filename);
    explicit Fifo(const std::string & fifoname);
    virtual ~Fifo();
    ssize_t send_msg(const rpc_msg & msg);
    ssize_t send_msg(std::shared_ptr<rpc_msg> msg);
    std::shared_ptr<rpc_msg> recv_msg();

};


#endif //AOS_PROJECT_FIFO_H
