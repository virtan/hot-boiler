#ifndef BTHREAD_POOL_H
#define BTHREAD_POOL_H

#include <boiler/config.h>
#include <hot/bpthread.h>
#include <hot/mutex.h>
#include <hot/condition.h>
#include <boiler/tp_functor.h>
#include <boiler/tp_functor_adapters.h>
#include <hot/singleton.h>
#include <hot/bpthread_key.h>
#include <boiler/string.h>
#include <boiler/bconfig.h>
#include <queue>
#include <list>
#include <memory>

namespace boiler {

using hot::mutex;
using hot::condition;
using hot::pthread;
using hot::pthread_key;
using hot::singleton;

#define current_thread_pool (singleton<pthread_key<thread_pool* const> >::instance())

class thread_pool
{
    public:
        typedef functor* tp_task;
        typedef std::queue<tp_task> tp_queue;
        typedef std::list<pthread> tp_link;
        typedef struct { thread_pool *tp; tp_link::iterator link; } tp_pointer;

    public:
        static thread_pool *run(const config_t &cf);
        ~thread_pool();
        void putq(tp_task ptask);
        void block();
        void unblock();

    private:
        thread_pool(int quantity);

    private:
        void notify_threads(); // tell threads that new work arrived
        void warn_threads(); // wake all threads
        int born_threads(int wish);
        bool born_thread();
        static void *worker_adapter(void *cl);
        void *worker(tp_link::iterator &link);

    private:
        tp_queue ptqueue;
        mutex ptqueue_mux;
        int free_threads;
        int all_threads;
        int notrunning_threads;
        bool quit;
        bool blocked;
        mutex borning_mux;
        condition notification;
        condition quit_notification;
        tp_link running_threads;
};

thread_pool *pool(const string &name);

}

#endif
