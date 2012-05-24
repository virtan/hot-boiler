#include <hot/condition.h>
#include <hot/safe_errno.h>
#include <errno.h>

hot::condition::condition() throw(hot::sys_exception)
{
    register int err=pthread_cond_init(&cond,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::condition::condition(const condition&) throw(hot::sys_exception)
{
    register int err=pthread_cond_init(&cond,NULL);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::condition::~condition() throw(hot::badcode_exception)
{
    register int err=pthread_cond_destroy(&cond);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::condition::wait(mutex &mux) throw(hot::badcode_exception)
{
    register int err=pthread_cond_wait(&cond,mux);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

bool hot::condition::timedwait(mutex &mux, const timestamp &tst) throw(hot::badcode_exception)
{
    register int err;
    while(1) {
        err=pthread_cond_timedwait(&cond,mux,tst);
        if(err) {
            if(err==ETIMEDOUT)
                return false;
            else if(err==EINTR)
                continue;
            else
                throw badcode_exception(safe_errno(err),EXCPLACE);
        }
        break;
    }
    return true;
}

void hot::condition::signal() throw(hot::badcode_exception)
{
    register int err=pthread_cond_signal(&cond);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::condition::broadcast() throw(hot::badcode_exception)
{
    register int err=pthread_cond_broadcast(&cond);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

hot::condition::operator pthread_cond_t&()
{
    return cond;
}

hot::condition::operator pthread_cond_t*()
{
    return &cond;
}
