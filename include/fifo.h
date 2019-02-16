#ifndef AOS_PROJECT_FIFO_H
#define AOS_PROJECT_FIFO_H

#include <memory>
#include <utility>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CHUNK_SIZE 1024


template<class T>
class Fifo {

private:
    int fd;
    char *fifoname;
    size_t buffer_size;
    void *buffer;

    Fifo():
        fd(0),
        fifoname(nullptr),
        buffer_size(CHUNK_SIZE),
        buffer(nullptr) {}

    void buffer_allocation_error() {
        perror("Cannot allocate buffer");
        close(fd);
        unlink(fifoname);
        exit(1);
    }

public:

    explicit Fifo(const char *filename):
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
        if (buffer == NULL)
            buffer_allocation_error();
    }

    virtual ~Fifo() {
        close(fd);
        unlink(fifoname);
        free(fifoname);
        free(buffer);
    }

    ssize_t send_msg(const T & msg) {
        size_t msg_size = msg.ByteSizeLong();
        if (msg_size > buffer_size) {
            buffer_size = msg_size + CHUNK_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL)
                buffer_allocation_error();
        }
        ssize_t written_bytes = write(fd, &msg_size, sizeof(size_t));
        msg.SerializeToArray(buffer, msg_size);
        written_bytes = write(fd, buffer, msg_size);
        return written_bytes;
    }

    std::shared_ptr<T> recv_msg() {
        size_t msg_size;
        read(fd, &msg_size, sizeof(size_t));
        read(fd, buffer, msg_size);
        std::shared_ptr<T> msg = std::make_shared<T>();
        msg->ParseFromArray(buffer, (int)msg_size);
        return msg;
    }

};


#endif //AOS_PROJECT_FIFO_H
