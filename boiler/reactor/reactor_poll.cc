#include <boiler/reactor_poll.h>
#include <boiler/sockfcn.h>
#include <hot/guard.h>
#include <boiler/exceptions.h>
#include <boiler/ipv4.h>
#include <boiler/tcp.h>
#include <boiler/safe_errno.h>

using hot::guard;

boiler::reactor_poll::reactor_poll()
    : fds(NULL), active(0), reform(true)
{
    if(pipe(notifier)<0)
        throw sys_exception(safe_errno(errno),EXCPLACE);
    sockfcn<IPv4,TCP,char,0>::set_nonblock(notifier[0]);
    sockfcn<IPv4,TCP,char,0>::set_nonblock(notifier[1]);
}

void boiler::reactor_poll::register_event(int sock, event_type events, int timeout_ms)
{
    {
        guard<mutex> rp_set_mutex_guard(rp_set_mutex);
        rp_set.insert(entry_type(sock,events,timeout_ms));
    }
    if(!reform)
        notify_reactor_poll();
}

void boiler::reactor_poll::quit_loop()
{
    notify_reactor_poll(true);
}

void boiler::reactor_poll::deregister_event(int sock, event_type events, int timeout_ms)
{
    {
        guard<mutex> rp_set_mutex_guard(rp_set_mutex);
        rp_set.erase(entry_type(sock,events,timeout_ms));
    }
    if(!reform)
        notify_reactor_poll();
}

void boiler::reactor_poll::event(int &sock, event_type &revent, event_descriptor::result_type &result)
{
beginning:
    if(active>0) {
        for(;;current++)
            if(current->revents) {
                active--;
                if(current==fds) {
                    // control descriptor
                    reform=true;
                    current++;
                    bool do_quit=empty_notifier();
                    if(do_quit) {
                        sock=-1;
                        return;
                    }
                    goto beginning;
                } else {
                    sock=current->fd;
                    revent=poll_to_event(current->revents);
                    result=event_descriptor::ok;
                    current++;
                    return;
                }
            }
    }
    if(reform) {
        delete[] fds;
        guard<mutex> rp_set_mutex_guard(rp_set_mutex);
        nfds=rp_set.size()+1;
        fds=new struct pollfd[nfds];
        fds->fd=notifier[0];
        fds->events=POLLIN;
        struct pollfd *fdsi=fds+1;
        for(set_type::iterator sti=rp_set.begin();sti!=rp_set.end();sti++,fdsi++)  {
            fdsi->fd=sti->sock;
            fdsi->events=event_to_poll(sti->events);
        }
        reform=false;
    }
    active=poll(fds,nfds,-1); // no timeout_yet
    current=fds;
    goto beginning;
}

void boiler::reactor_poll::notify_reactor_poll(bool quit)
{
    char c=quit?2:1;
    ::write(notifier[1],&c,1);
}

bool boiler::reactor_poll::empty_notifier()
{
    char c[256];
    int res;
    bool do_quit=false;
    do {
        res=::read(notifier[0],&c,256);
        for(int i=0;i<res;++i)
            if(c[i]==2)
                do_quit=true;
    } while(res==256);
    return do_quit;
}
