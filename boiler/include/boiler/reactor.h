#ifndef BREACTOR_H
#define BREACTOR_H

#include <boiler/config.h>
#include <boiler/event.h>
#include <boiler/thread_pool.h>
#include <boiler/tp_functor.h>
#include <hot/mutex.h>
#include <boiler/bconfig.h>
#include <map>
#include <sys/poll.h>

namespace boiler {

using hot::mutex;

class event_descriptor
{
    public:
        typedef enum { ok=0,timeout,system_error } result_type;
        typedef bool (*poq_handler_type)(const event_type &, bool);

    public:
        event_descriptor() : long_life(false) {};
	virtual ~event_descriptor() {};

        virtual void put_on_queue() {
            tp->putq(f);
        }

    public:
        int sock;
        event_type events;
        int timeout_ms;
        result_type result;
        functor *f;
        thread_pool *tp;
        bool long_life;
};

class reactor_implementation
{
    public:
        virtual void register_event(int sock, event_type events, int timeout_ms) = 0;
        virtual void deregister_event(int sock, event_type events, int timeout_ms) = 0;
        virtual void event(int &sock, event_type &revent, event_descriptor::result_type &result) = 0;
        virtual void quit_loop() = 0;
        virtual ~reactor_implementation() {};
};

class reactor {
    public:
        typedef std::multimap<int, event_descriptor *> reactor_map;
        typedef std::list<event_descriptor*> event_list;

    public:
        static reactor *run(const config_t &cf);
        ~reactor();
        void register_event(event_descriptor &ed);
        void deregister_event(event_descriptor &ed);
        static void *cycle(void *_reactor);

    private:
        reactor(const config_t &cf);
        void cycle();

    private:
        reactor_implementation *ri;
        reactor_map rmap;
        mutex rmap_mutex;
        pthread reactor_thread;
};

}

#endif
