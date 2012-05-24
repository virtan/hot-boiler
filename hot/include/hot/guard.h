#ifndef H_GUARD_H
#define H_GUARD_H

#include <hot/config.h>
#include <hot/mutex.h>
#include <hot/rwlock.h>

namespace hot {

typedef enum { ro=0,rw } guard_lock_type;

template <class Locker, guard_lock_type Type=ro>
class guard
{
    public:
        guard(Locker &_lck) : lck(_lck)
        {
            lck.lock();
        }
        ~guard()
        {
            lck.unlock();
        }

    private:
        Locker &lck;
};

template <>
inline guard<rwlock,ro>::guard(rwlock &_lck) : lck(_lck) {
    lck.rdlock();
}

template <>
inline guard<rwlock,rw>::guard(rwlock &_lck) : lck(_lck) {
    lck.wrlock();
}


}

#endif
