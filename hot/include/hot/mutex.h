#ifndef H_MUTEX_H
#define H_MUTEX_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>

namespace hot {

class mutex
{
    public:
        mutex() throw(sys_exception);
        mutex(const mutex&) throw(sys_exception);
        ~mutex() throw(badcode_exception);
        void lock() throw(badcode_exception);
        bool trylock() throw(badcode_exception);
        void unlock() throw(badcode_exception);
        operator pthread_mutex_t&();
        operator pthread_mutex_t*();

    private:
        pthread_mutex_t mux;
};

}

#endif
