// synchronized_queue.hpp

#ifndef SYNCHRONIZED_QUEUE_HPP
#define SYNCHRONIZED_QUEUE_HPP

#include <list>
#include <mutex>
#include <condition_variable>

template <class T>
class SynchronizedQueue {

    private:
        std::list<T> queue;
        std::mutex m;
        std::condition_variable cv;

    public:

        SynchronizedQueue() {}

        ~SynchronizedQueue() {}

        T get() {
            std::unique_lock<std::mutex> lck(m);
            while (queue.empty()) cv.wait(lck);
            T result = queue.front();
            queue.pop_front();
            return result;
        }

        void put(T item) {
            std::unique_lock<std::mutex> lck(m);
            queue.push_back(item);
            cv.notify_all();
        }

        bool empty() {
            std::unique_lock<std::mutex> lck(m);
            return queue.empty();
        }

};

#endif // SYNCHRONIZED_QUEUE_HPP
