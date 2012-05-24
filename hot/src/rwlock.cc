#include <hot/rwlock.h>
#include <hot/safe_errno.h>
#include <errno.h>

hot::rwlock::rwlock() throw(hot::sys_exception)
{
    register int err=pthread_rwlock_init(&rwl,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::rwlock::rwlock(const rwlock&) throw(hot::sys_exception)
{
    register int err=pthread_rwlock_init(&rwl,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::rwlock::~rwlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_destroy(&rwl);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::rwlock::rdlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_rdlock(&rwl);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::rwlock::wrlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_wrlock(&rwl);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

bool hot::rwlock::tryrdlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_tryrdlock(&rwl);
    if(err) {
        if(err==EBUSY)
            return false;
        else
            throw badcode_exception(safe_errno(err),EXCPLACE);
    }
    return true;
}

bool hot::rwlock::trywrlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_trywrlock(&rwl);
    if(err) {
        if(err==EBUSY)
            return false;
        else
            throw badcode_exception(safe_errno(err),EXCPLACE);
    }
    return true;
}

void hot::rwlock::unlock() throw(hot::badcode_exception)
{
    register int err=pthread_rwlock_unlock(&rwl);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

hot::rwlock::operator pthread_rwlock_t&()
{
    return rwl;
}

hot::rwlock::operator pthread_rwlock_t*()
{
    return &rwl;
}
