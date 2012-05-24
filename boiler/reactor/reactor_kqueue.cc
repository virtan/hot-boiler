#include <boiler/reactor_kqueue.h>

#ifdef HAVE_LIBkqueue

#include <boiler/exceptions.h>
#include <boiler/safe_errno.h>
#include <boiler/sockfcn.h>
#include <boiler/ipv4.h>
#include <boiler/tcp.h>
#include <errno.h>


boiler::reactor_kqueue::reactor_kqueue()
{
    kentry=kqueue();
    if(kentry<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    if(pipe(notifier)<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    sockfcn<IPv4,TCP,char,1>::set_nonblock(notifier[0]);
    sockfcn<IPv4,TCP,char,1>::set_nonblock(notifier[1]);
    struct kevent cl;
    EV_SET(&cl,notifier[0],EVFILT_READ,EV_ADD,0,0,0);
    int res=kevent(kentry,&cl,1,0,0,0);
    if(res<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
}

boiler::reactor_kqueue::~reactor_kqueue()
{
    if(notifier[0]>=0)
        close(notifier[0]);
    if(notifier[1]>=0)
        close(notifier[1]);
}

void boiler::reactor_kqueue::register_event(int sock, event_type events, int timeout_ms)
{
    struct kevent cl[2];
    if(event_to_kfilter_read(events)) {
        EV_SET(&cl[0],sock,EVFILT_READ,EV_ADD,0,0,0);
        if(timeout_ms>0) {
            EV_SET(&cl[1],sock*2,EVFILT_TIMER,EV_ADD|EV_ONESHOT,0,timeout_ms,(void*)1);
        }
        int res=kevent(kentry,cl,timeout_ms>0?2:1,0,0,0);
        if(res<0) {
            // TODO: we have to imitate event
            // XXX: very seldom event
        }
    }
    if(event_to_kfilter_write(events)) {
        EV_SET(&cl[0],sock,EVFILT_WRITE,EV_ADD,0,0,0);
        if(timeout_ms>0) {
            EV_SET(&cl[1],sock*2+1,EVFILT_TIMER,EV_ADD|EV_ONESHOT,0,timeout_ms,(void*)1);
        }
        int res=kevent(kentry,cl,timeout_ms>0?2:1,0,0,0);
        if(res<0) {
            // TODO: we have to imitate event
            // XXX: very seldom event
        }
    }
}

void boiler::reactor_kqueue::deregister_event(int sock, event_type events, int timeout_ms)
{
    struct kevent cl[2];
    if(event_to_kfilter_read(events)) {
        EV_SET(&cl[0],sock,EVFILT_READ,EV_DELETE,0,0,0);
        if(timeout_ms>0) {
            EV_SET(&cl[1],sock*2,EVFILT_TIMER,EV_DELETE,0,0,0);
        }
        int res=kevent(kentry,cl,timeout_ms>0?2:1,0,0,0);
        if(res<0) {
            // TODO: do nothing ?
            // XXX: very seldom event
        }
    }
    if(event_to_kfilter_write(events)) {
        EV_SET(&cl[0],sock,EVFILT_WRITE,EV_DELETE,0,0,0);
        if(timeout_ms>0) {
            EV_SET(&cl[1],sock*2+1,EVFILT_TIMER,EV_DELETE,0,0,0);
        }
        int res=kevent(kentry,cl,timeout_ms>0?2:1,0,0,0);
        if(res<0) {
            // TODO: do nothing ?
            // XXX: very seldom event
        }
    }
}

void boiler::reactor_kqueue::quit_loop()
{
    int c=2;
    ssize_t written=::write(notifier[1],&c,1);
    if(written!=1)
        throw sys_exception(safe_errno(errno));
}

void boiler::reactor_kqueue::event(int &sock, event_type &revent, event_descriptor::result_type &result)
{
    struct kevent kes;
    while(1) {
        int res=kevent(kentry,0,0,&kes,1,0);
        if(res<0) {
            // XXX: must not be
            throw sys_exception(safe_errno(errno),EXCPLACE);
        }
        if(!res)
            continue;
        break;
    }
    if(kes.filter==EVFILT_READ) {
        if((int) kes.ident==notifier[0]) {
            // quit
            sock=-1;
            return;
        }
        sock=(int) kes.ident;
        revent=event_type(e_read|e_accept|e_connect);
        result=event_descriptor::ok;
    } else if(kes.filter==EVFILT_WRITE) {
        sock=(int) kes.ident;
        revent=event_type(e_write);
        result=event_descriptor::ok;
    } else if(kes.filter==EVFILT_TIMER) {
        sock=(int) (kes.ident/2);
        result=event_descriptor::timeout;
    } else {
        sock=(int) kes.ident;
        if(kes.udata)
            sock/=2;
        result=event_descriptor::system_error;
    }
}

#endif
