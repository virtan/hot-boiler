#include <hot/bpthread_attr.h>
#include <hot/safe_errno.h>

hot::pthread_attr::pthread_attr() throw(hot::sys_exception)
{
    register int err=pthread_attr_init(&thr_attr);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::pthread_attr::pthread_attr(const pthread_attr&) throw(sys_exception)
{
    register int err=pthread_attr_init(&thr_attr);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
}

hot::pthread_attr::~pthread_attr() throw(hot::badcode_exception)
{
    register int err=pthread_attr_destroy(&thr_attr);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

hot::pthread_attr::operator pthread_attr_t*()
{
    return &thr_attr;
}

hot::pthread_attr::operator pthread_attr_t&()
{
    return thr_attr;
}
