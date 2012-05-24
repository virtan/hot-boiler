#include <boiler/thread_pool.h>
#include <hot/guard.h>
#include <hot/bpthread_key.h>
#include <boiler/globals.h>
#include <boiler/log.h>
#include <hot/lexical_cast.h>
#include <boiler/signals.h>


using hot::guard;
using hot::pthread_key;

boiler::thread_pool::thread_pool(int quantity) : free_threads(0),all_threads(quantity),notrunning_threads(quantity),
                                                 quit(false),blocked(false)
{
}

boiler::thread_pool::~thread_pool()
{
    quit=true;
    warn_threads();
    borning_mux.lock();
    while(notrunning_threads!=all_threads) {
        quit_notification.wait(borning_mux);
    }
    borning_mux.unlock();
}

boiler::thread_pool *boiler::thread_pool::run(const config_t &cf)
{
    return new thread_pool(hot::lexical_cast<int>((const char*)(cf/"threads"),__PRETTY_FUNCTION__));
}

void boiler::thread_pool::putq(tp_task ptask)
{
    {
        guard<mutex> ptqueue_mux_guard(ptqueue_mux);
        ptqueue.push(ptask);
    }
    notify_threads();
}

void boiler::thread_pool::block()
{
    blocked=true;
}

void boiler::thread_pool::unblock()
{
    blocked=false;
    notify_threads();
}

void boiler::thread_pool::notify_threads()
{
    if(blocked)
        return;
    if(free_threads)
        notification.signal();
    else
        born_threads(ptqueue.size());
}

void boiler::thread_pool::warn_threads()
{
    notification.broadcast();
}

int boiler::thread_pool::born_threads(int wish)
{
    int now_borned=0;
    if(notrunning_threads>0) {
        guard<mutex> borning_mux_guard(borning_mux);
        if(notrunning_threads>0)
            for(int count=(notrunning_threads<wish?notrunning_threads:wish);count;count--)
                if(born_thread())
                    now_borned++;
    }
    return now_borned;
}

bool boiler::thread_pool::born_thread()
{
    tp_link::iterator new_thread=running_threads.insert(running_threads.end(),pthread());
    tp_pointer *p=new tp_pointer;
    p->tp=this;
    p->link=new_thread;
    try {
        signal_block_in_thread(true);
        new_thread->create(&thread_pool::worker_adapter,p);
        signal_block_in_thread(false);
    } catch(sys_exception &e) {
        delete p;
        running_threads.erase(new_thread);
        // TODO: report ? (we're under mutex)
        return false;
    }
    notrunning_threads--;
    return true;
}

void *boiler::thread_pool::worker_adapter(void *cl)
{
    tp_pointer *_p=reinterpret_cast<tp_pointer*>(cl);
    tp_pointer p=*_p;
    delete _p;
    reinterpret_cast<thread_pool*>(p.tp)->worker(p.link);
    // NOT REACHED
    return NULL;
}

void *boiler::thread_pool::worker(tp_link::iterator &link)
{
    try {
        current_thread_pool.set_specific(this);
        ptqueue_mux.lock();
        free_threads++;
        do {
            if(quit) {
                ptqueue_mux.unlock();
                break;
            }
            free_threads--;
            if(!ptqueue.empty()) {
                tp_task execution=ptqueue.front();
                ptqueue.pop();
                ptqueue_mux.unlock();
                bool to_delete;
                try {
                    to_delete=(*execution)();
                } catch(...) {
                    // TODO: log that this thread got an exception
                    // std::cerr << "exception in tp_task" << std::endl;
                    // std::cerr << "catch found" << std::endl;
                }
                if(to_delete)
                    delete execution;
                ptqueue_mux.lock();
                free_threads++;
                continue;
            }
            free_threads++;
            notification.wait(ptqueue_mux);
        } while(1);
    } catch(std_exception &e) {
        log_error("thread worker caught exception " << e.type() << ": " << e << " at " << e.function());
    }
    guard<mutex> borning_mux_guard(borning_mux);
    pthread me=*link;
    running_threads.erase(link);
    notrunning_threads++;
    quit_notification.signal();
    me.exit(); 
    // NOT REACHED
    return NULL;
}

boiler::thread_pool *boiler::pool(const string &name)
{
    if(!globals().thread_pool_exist(name))
        throw invarg_exception(string("no thread pool named \"")+name+"\" found");
    return globals().thread_pools()[name];
}

