#include <boiler/reactor_epoll.h>

#ifdef HAVE_LIBepoll

#include <boiler/exceptions.h>
#include <boiler/safe_errno.h>
#include <boiler/sockfcn.h>
#include <boiler/ipv4.h>
#include <boiler/tcp.h>
#include <errno.h>
#include <hot/guard.h>


using hot::guard;

boiler::reactor_epoll::reactor_epoll() : eentry_1(-1), eentry_2(-1)
{
    notifier[0]=-1;
    notifier[1]=-1;
    eentry_1=epoll_create(256);
    if(eentry_1<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    eentry_2=epoll_create(256);
    if(eentry_2<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    if(pipe(notifier)<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    sockfcn<IPv4,TCP,char,0>::set_nonblock(notifier[0]);
    sockfcn<IPv4,TCP,char,0>::set_nonblock(notifier[1]);
    struct epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=notifier[0];
    int res=epoll_ctl(eentry_1,EPOLL_CTL_ADD,notifier[0],&ev);
    if(res<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    ev.data.fd=eentry_2;
    res=epoll_ctl(eentry_1,EPOLL_CTL_ADD,eentry_2,&ev);
    if(res<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
}

boiler::reactor_epoll::~reactor_epoll()
{
    if(eentry_2>=0)
        close(eentry_2);
    if(eentry_1>=0)
        close(eentry_1);
    if(notifier[0]>=0)
        close(notifier[0]);
    if(notifier[1]>=0)
        close(notifier[1]);
}

void boiler::reactor_epoll::register_event(int sock, event_type events, int timeout_ms)
{
    struct epoll_event ev;
    int res;
    if(event_to_eev_read(events)) {
        ev.events=EPOLLIN;
        ev.data.fd=sock;
        res=epoll_ctl(eentry_2,EPOLL_CTL_ADD,sock,&ev);
        if(res<0) {
            // TODO: imitate event 
            // XXX: very seldom event
        }
        if(timeout_ms>0) {
            time_t timeout=time(NULL);
            timeout+=timeout_ms/1000;
            guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
            timer_to_extsock.insert(timer_to_extsock_t::value_type(timeout,(uint32_t) sock));
            extsock_to_timer[(uint32_t) sock]=timeout;
        }
    }
    if(event_to_eev_write(events)) {
        ev.events=EPOLLOUT;
        ev.data.fd=sock;
        res=epoll_ctl(eentry_1,EPOLL_CTL_ADD,sock,&ev);
        if(res<0) {
            // TODO: imitate event 
            // XXX: very seldom event
        }
        if(timeout_ms>0) {
            time_t timeout=time(NULL);
            timeout+=timeout_ms/1000;
            guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
            timer_to_extsock.insert(timer_to_extsock_t::value_type(timeout,((uint32_t) sock)|(1<<31)));
            extsock_to_timer[((uint32_t) sock)|(1<<31)]=timeout;
        }
    }
}

void boiler::reactor_epoll::deregister_event(int sock, event_type events, int timeout_ms)
{
    struct epoll_event ev;
    int res;
    if(event_to_eev_read(events)) {
        ev.events=EPOLLIN;
        ev.data.fd=sock;
        res=epoll_ctl(eentry_2,EPOLL_CTL_DEL,sock,&ev);
        if(res<0) {
            // TODO: do nothing ?
            // XXX: very seldom event
        }
        if(timeout_ms>0) {
            guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
            extsock_to_timer_t::iterator i=extsock_to_timer.find((uint32_t) sock);
            if(i!=extsock_to_timer.end()) {
                time_t timeout=i->second;
                extsock_to_timer.erase(i);
                for(timer_to_extsock_t::iterator j=timer_to_extsock.lower_bound(timeout);
                    j!=timer_to_extsock.end() && j->first==timeout;
                    j++) {
                    if(j->second==(uint32_t) sock) {
                        timer_to_extsock.erase(j);
                        break;
                    }
                }
            }
        }
    }
    if(event_to_eev_write(events)) {
        ev.events=EPOLLOUT;
        ev.data.fd=sock;
        res=epoll_ctl(eentry_1,EPOLL_CTL_DEL,sock,&ev);
        if(res<0) {
            // TODO: do nothing ?
            // XXX: very seldom event
        }
        if(timeout_ms>0) {
            guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
            extsock_to_timer_t::iterator i=extsock_to_timer.find(((uint32_t) sock)|(1<<31));
            if(i!=extsock_to_timer.end()) {
                time_t timeout=i->second;
                extsock_to_timer.erase(i);
                for(timer_to_extsock_t::iterator j=timer_to_extsock.lower_bound(timeout);
                    j!=timer_to_extsock.end() && j->first==timeout;
                    j++) {
                    if(j->second==((uint32_t) sock)|(1<<31)) {
                        timer_to_extsock.erase(j);
                        break;
                    }
                }
            }
        }
    }
}

void boiler::reactor_epoll::quit_loop()
{
    int c=2;
    ssize_t written=::write(notifier[1],&c,1);
    if(written!=1)
        throw sys_exception(safe_errno(errno));
}

void boiler::reactor_epoll::event(int &sock, event_type &revent, event_descriptor::result_type &result)
{
    {
        guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
        timer_to_extsock_t::iterator i=timer_to_extsock.begin();
        if(i!=timer_to_extsock.end() && i->first<=time(NULL)) {
            sock=(int) (i->second&0x7fffffff);
            revent=(i->second>>31)?event_type(e_write):event_type(e_read|e_accept|e_connect);
            result=event_descriptor::timeout;
            extsock_to_timer.erase(i->second);
            timer_to_extsock.erase(i);
            return;
        }
    }
    struct epoll_event ev;
    int res;
reeventing:
    while(1) {
        res=epoll_wait(eentry_1,&ev,1,1000);
        if(res<0)
            throw sys_exception(safe_errno(errno),EXCPLACE);
        if(!res) {
            {
                guard<mutex> timer_operations_mutex_guard(timer_operations_mutex);
                timer_to_extsock_t::iterator i=timer_to_extsock.begin();
                if(i!=timer_to_extsock.end() && i->first<=time(NULL)) {
                    sock=(int) (i->second&0x7fffffff);
                    revent=(i->second>>31)?event_type(e_write):event_type(e_read|e_accept|e_connect);
                    result=event_descriptor::timeout;
                    extsock_to_timer.erase(i->second);
                    timer_to_extsock.erase(i);
                    return;
                }
            }
            continue;
        }
        break;
    }
    if(ev.data.fd==eentry_2) {
        res=epoll_wait(eentry_2,&ev,1,0);
        if(res<0)
            throw sys_exception(safe_errno(errno),EXCPLACE);
        if(!res) {
            // strange case
            goto reeventing;
        }
        sock=ev.data.fd;
        revent=event_type(e_read|e_accept|e_connect);
        result=event_descriptor::ok;
    } else if(ev.data.fd==notifier[0]) {
        // quit
        sock=-1;
        return;
    } else {
        sock=ev.data.fd;
        revent=event_type(e_write);
        result=event_descriptor::ok;
    }
}

#endif
