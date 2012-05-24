#include <hot/mutex.h>
#include <hot/safe_errno.h>
#include <errno.h>

hot::mutex::mutex() throw(hot::sys_exception)
{
    register int err=pthread_mutex_init(&mux,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::mutex::mutex(const mutex&) throw(hot::sys_exception)
{
    register int err=pthread_mutex_init(&mux,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::mutex::~mutex() throw(hot::badcode_exception)
{
    register int err=pthread_mutex_destroy(&mux);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::mutex::lock() throw(hot::badcode_exception)
{
    register int err=pthread_mutex_lock(&mux);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

bool hot::mutex::trylock() throw(hot::badcode_exception)
{
    register int err=pthread_mutex_trylock(&mux);
    if(err) {
        if(err==EBUSY)
            return false;
        else
            throw badcode_exception(safe_errno(err),EXCPLACE);
    }
    return true;
}

void hot::mutex::unlock() throw(hot::badcode_exception)
{
    register int err=pthread_mutex_unlock(&mux);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

hot::mutex::operator pthread_mutex_t&()
{
    return mux;
}

hot::mutex::operator pthread_mutex_t*()
{
    return &mux;
}
