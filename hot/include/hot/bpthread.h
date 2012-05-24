#ifndef H_PTHREAD_H
#define H_PTHREAD_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>
#include <hot/bpthread_attr.h>

namespace hot {

class pthread
{
    public:
        pthread();
        ~pthread();
        void create(void *(*start)(void*), void *arg=NULL) throw(sys_exception);
        void create(void *(*start)(void*), void *arg, pthread_attr &thr_attr) throw(sys_exception);
        void create(void *(*start)(void*), void *arg, pthread_attr *thr_attr) throw(sys_exception);
        bool cancel() throw(badcode_exception); // false if this thread not exists already
        bool join(void **retval=NULL) throw(badcode_exception); // false if this thread not exists already
        void detach() throw(badcode_exception);
        void exit(void *retval=NULL);
        bool running();
        operator pthread_t&();
        operator pthread_t*();

    private:
        pthread_t thr;
        bool running_;
};

}

#endif
