// fifo.cc

#include "fifo.h"


Fifo::Fifo(const char *filename):
        buffer_size(CHUNK_SIZE) {
    fifoname = strdup(filename);
    mkfifo(filename, S_IRUSR | S_IWUSR);
    fd = open(filename, O_RDWR);
    if (fd == 0) {
        perror("Cannot open FIFO");
        unlink(filename);
        exit(1);
    }
    buffer = malloc(buffer_size);
    if (buffer == nullptr)
        buffer_allocation_error();
    dout << "Created FIFO " << fifoname << std::endl;
}


Fifo::Fifo(const std::string & fifoname): Fifo(fifoname.c_str()) {}


Fifo::~Fifo() {
    dout << "Releasing FIFO " << fifoname << std::endl;
    close(fd);
    unlink(fifoname);
    free(fifoname);
    free(buffer);
}


void Fifo::buffer_allocation_error() {
    perror("Cannot allocate buffer");
    close(fd);
    unlink(fifoname);
    exit(1);
}


ssize_t Fifo::send_msg(const rpc_msg & msg) {
    size_t msg_size = msg.ByteSizeLong();
    if (msg_size > buffer_size) {
        buffer_size = msg_size + CHUNK_SIZE;
        buffer = realloc(buffer, buffer_size);
        if (buffer == nullptr)
            buffer_allocation_error();
    }
    write(fd, &msg_size, sizeof(size_t));
    msg.SerializeToArray(buffer, (int)msg_size);
    ssize_t written_bytes = write(fd, buffer, msg_size);
    dout << written_bytes << " bytes sent" << std::endl;
    dout << msg.DebugString() << std::endl;
    return written_bytes;
}

ssize_t Fifo::send_msg(std::shared_ptr<rpc_msg> msg) {
    return send_msg(*msg);
}


std::shared_ptr<rpc_msg> Fifo::recv_msg() {
    size_t msg_size;
    read(fd, &msg_size, sizeof(size_t));
    ssize_t read_bytes = read(fd, buffer, msg_size);
    std::shared_ptr<rpc_msg> msg = std::make_shared<rpc_msg>();
    msg->ParseFromArray(buffer, (int)msg_size);
    dout << read_bytes << " bytes read" << std::endl;
    dout << msg->DebugString() << std::endl;
    return msg;
}
