#include <hot/bpthread_once.h>
#include <hot/safe_errno.h>

pthread_once_t hot::pthread_once::ctrl_tmpl=PTHREAD_ONCE_INIT;

hot::pthread_once::pthread_once() : ctrl(ctrl_tmpl)
{
}

hot::pthread_once::pthread_once(const pthread_once&) : ctrl(ctrl_tmpl)
{
}

hot::pthread_once::pthread_once(routine r) : ctrl(ctrl_tmpl)
{
    run(r);
}

void hot::pthread_once::run(routine r)
{
    int ret=::pthread_once(&ctrl, r);
    if(ret)
        throw sys_exception(safe_errno(ret),EXCPLACE);
}
