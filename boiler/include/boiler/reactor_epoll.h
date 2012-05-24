#ifndef BREACTOR_EPOLL_H
#define BREACTOR_EPOLL_H

#include <boiler/config.h>

#ifdef HAVE_LIBepoll

#include <boiler/reactor.h>
#include <sys/epoll.h>
#include <map>

namespace boiler
{

inline bool event_to_eev_read(const event_type &e) {
    if(e&e_read || e&e_accept || e&e_connect)
        return true;
    return false;
}

inline bool event_to_eev_write(const event_type &e) {
    if(e&e_write)
        return true;
    return false;
}

// Note!
// Timer granularity = 1s

class reactor_epoll : public reactor_implementation
{
    public:
        typedef std::multimap<time_t, uint32_t> timer_to_extsock_t;
        typedef std::map<uint32_t, time_t> extsock_to_timer_t;

    public:
        reactor_epoll();
        ~reactor_epoll();

        void register_event(int sock, event_type events, int timeout_ms);
        void deregister_event(int sock, event_type events, int timeout_ms);
        void event(int &sock, event_type &revent, event_descriptor::result_type &result);
        void quit_loop();

    private:
        int eentry_1; // notifier[0] + write + eentry_2
        int eentry_2; // read
        int notifier[2];
        mutex timer_operations_mutex;
        timer_to_extsock_t timer_to_extsock;
        extsock_to_timer_t extsock_to_timer;
};

}

#endif

#endif
