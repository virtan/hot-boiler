#include <boiler/reactor.h>
#include <hot/guard.h>
#include <boiler/reactor_poll.h>
#include <boiler/reactor_kqueue.h>
#include <boiler/reactor_epoll.h>
#include <boiler/safe_errno.h>
#include <boiler/signals.h>
#include <signal.h>


using hot::guard;

boiler::reactor::reactor(const config_t &cf)
{
    if(string((const char*)(cf/"type"))=="poll")
        ri=new reactor_poll;
#ifdef HAVE_LIBkqueue
    else if(string((const char*)(cf/"type"))=="kqueue")
        ri=new reactor_kqueue;
#endif
#ifdef HAVE_LIBepoll
    else if(string((const char*)(cf/"type"))=="epoll")
        ri=new reactor_epoll;
#endif
    else
        throw invarg_exception(string("no reactor type ")+(const char*) (cf/"type")+" known (reactor "+cf.name()+")");
}

boiler::reactor::~reactor()
{
    if(reactor_thread.running()) {
        ri->quit_loop();
        reactor_thread.join();
        // TODO: not right solution
        // reactor_thread.cancel();
    }
    delete ri;
}

boiler::reactor *boiler::reactor::run(const config_t &cf)
{
    // block SIGPIPE
    sigset_t blocker;
    sigemptyset(&blocker);
    sigaddset(&blocker,SIGPIPE);
    int err=pthread_sigmask(SIG_BLOCK,&blocker,NULL);
    if(err<0)
        throw sys_exception(safe_errno(-err));

    reactor *r=new reactor(cf);
    signal_block_in_thread(true);
    r->reactor_thread.create(&reactor::cycle,reinterpret_cast<void*>(r));
    signal_block_in_thread(false);
    return r;
}

void boiler::reactor::register_event(event_descriptor &ed)
{
    {
        guard<mutex> rmap_mutex_guard(rmap_mutex);
        rmap.insert(reactor_map::value_type(ed.sock,&ed));
    }
    ri->register_event(ed.sock,ed.events,ed.timeout_ms);
    // std::cerr << "registering " << ed.sock << std::endl;
}

void boiler::reactor::deregister_event(event_descriptor &ed)
{
    bool found=false;
    {
        guard<mutex> rmap_mutex_guard(rmap_mutex);
        reactor_map::iterator rmi=rmap.find(ed.sock);
        if(rmi==rmap.end())
            return;
        for(;rmi->first==ed.sock;)
            if(rmi->second->events==ed.events && rmi->second->timeout_ms==ed.timeout_ms) {
                reactor_map::iterator to_remove=rmi;
                rmi++;
                rmap.erase(to_remove);
                found=true;
            } else
                rmi++;
    }
    if(found)
        ri->deregister_event(ed.sock,ed.events,ed.timeout_ms);
}

void *boiler::reactor::cycle(void *_reactor)
{
    try {
        reinterpret_cast<reactor*>(_reactor)->cycle();
    } catch(std::exception &e) {
        std::cerr << "catched exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "catched unknown exception" << std::endl;
    }
    return NULL;
}

void boiler::reactor::cycle()
{
    int sock;
    event_type revent;
    event_descriptor::result_type result;
    while(1) {
        ri->event(sock,revent,result);
        if(sock==-1)
            break;
        event_list ped_list;
        {
            guard<mutex> rmap_mutex_guard(rmap_mutex);
            reactor_map::iterator rmi=rmap.find(sock);
            for(;rmi!=rmap.end() && rmi->first==sock;) {
                if(rmi->second->events&revent) {
                    // std::cerr << "activated " << rmi->first << std::endl;
                    rmi->second->result=result;
                    ped_list.push_back(rmi->second);
                    if(!rmi->second->long_life) {
                        reactor_map::iterator to_remove=rmi;
                        rmi++;
                        rmap.erase(to_remove);
                    } else {
                        rmi++;
                    }
                } else
                    rmi++;
            }
        }
        for(event_list::iterator pli=ped_list.begin();pli!=ped_list.end();pli++) {
            if(!(*pli)->long_life)
                ri->deregister_event((*pli)->sock,(*pli)->events,(*pli)->timeout_ms);
            (*pli)->put_on_queue();
        }
    }
}
