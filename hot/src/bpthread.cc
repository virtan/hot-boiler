#include <hot/bpthread.h>
#include <hot/safe_errno.h>
#include <errno.h>

hot::pthread::pthread() : running_(false)
{
}

hot::pthread::~pthread()
{
}

void hot::pthread::create(void *(*start)(void*), void *arg) throw(hot::sys_exception)
{
    create(start,arg,NULL);
}

void hot::pthread::create(void *(*start)(void*), void *arg, pthread_attr &thr_attr) throw(hot::sys_exception)
{
    create(start,arg,&thr_attr);
}

void hot::pthread::create(void *(*start)(void*), void *arg, pthread_attr *thr_attr) throw(hot::sys_exception)
{
    register int err=pthread_create(&thr,(thr_attr?static_cast<pthread_attr_t*>(*thr_attr):NULL),start,arg);
    if(err)
        throw sys_exception(safe_errno(err),EXCPLACE);
    running_=true;
}

bool hot::pthread::cancel() throw(hot::badcode_exception)
{
    register int err=pthread_cancel(thr);
    if(err) {
        if(err==ESRCH)
            return false;
        else
            throw badcode_exception(safe_errno(err),EXCPLACE);
    }
    running_=false;
    return true;
}

bool hot::pthread::join(void **retval) throw(hot::badcode_exception)
{
    void *noretval;
    register int err=pthread_join(thr,retval?retval:&noretval);
    if(err) {
        if(err==ESRCH)
            return false;
        else
            throw badcode_exception(safe_errno(err),EXCPLACE);
    }
    running_=false;
    return true;
}

void hot::pthread::detach() throw(hot::badcode_exception)
{
    register int err=pthread_detach(thr);
    if(err)
        throw badcode_exception(safe_errno(err),EXCPLACE);
}

void hot::pthread::exit(void *retval)
{
    running_=false;
    pthread_exit(retval);
}

hot::pthread::operator pthread_t&()
{
    return thr;
}

hot::pthread::operator pthread_t*()
{
    return &thr;
}

bool hot::pthread::running() {
    return running_;
}
