#ifndef H_CONDITION_H
#define H_CONDITION_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>
#include <hot/mutex.h>
#include <hot/timestamp.h>

namespace hot {

class condition
{
    public:
        condition() throw(sys_exception);
        condition(const condition&) throw(sys_exception);
        ~condition() throw(badcode_exception);
        void wait(mutex &mux) throw(badcode_exception);
        bool timedwait(mutex &mux, const timestamp &tst) throw(badcode_exception);
        void signal() throw(badcode_exception);
        void broadcast() throw(badcode_exception);
        operator pthread_cond_t&();
        operator pthread_cond_t*();

    private:
        pthread_cond_t cond;

};

}

#endif
