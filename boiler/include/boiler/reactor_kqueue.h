#ifndef BREACTOR_KQUEUE_H
#define BREACTOR_KQUEUE_H

#include <boiler/config.h>

#ifdef HAVE_LIBkqueue

#include <boiler/reactor.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace boiler
{

inline bool event_to_kfilter_read(const event_type &e) {
    if(e&e_read || e&e_accept || e&e_connect)
        return true;
    return false;
}

inline bool event_to_kfilter_write(const event_type &e) {
    if(e&e_write)
        return true;
    return false;
}

class reactor_kqueue : public reactor_implementation
{
    public:
        reactor_kqueue();
        ~reactor_kqueue();

        void register_event(int sock, event_type events, int timeout_ms);
        void deregister_event(int sock, event_type events, int timeout_ms);
        void event(int &sock, event_type &revent, event_descriptor::result_type &result);
        void quit_loop();

    private:
        int kentry;
        int notifier[2];
};

}

#endif

#endif
