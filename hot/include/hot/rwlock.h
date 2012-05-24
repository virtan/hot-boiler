#ifndef H_RWLOCK_H
#define H_RWLOCK_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>

namespace hot {

class rwlock
{
    public:
        rwlock() throw(sys_exception);
        rwlock(const rwlock&) throw(sys_exception);
        ~rwlock() throw(badcode_exception);
        void rdlock() throw(badcode_exception);
        void wrlock() throw(badcode_exception);
        bool tryrdlock() throw(badcode_exception);
        bool trywrlock() throw(badcode_exception);
        void unlock() throw(badcode_exception);
        operator pthread_rwlock_t&();
        operator pthread_rwlock_t*();

    private:
        pthread_rwlock_t rwl;
};

}

#endif
