#ifndef BREACTOR_POLL_H
#define BREACTOR_POLL_H

#include <boiler/config.h>
#include <boiler/reactor.h>
#include <set>
#include <poll.h>

namespace boiler
{

class reactor_poll : public reactor_implementation
{
    public:
        struct entry_type {
            entry_type(int _sock, event_type _events, int _timeout_ms) : sock(_sock), events(_events), timeout_ms(_timeout_ms) {}
            int sock;
            event_type events;
            int timeout_ms;
        };
        struct entry_type_hash
        {
            size_t operator()(const entry_type &e) const
            {
                return (e.timeout_ms*5+e.events)*5+e.sock;
            }
        };
        struct entry_type_lower
        {
            bool operator()(const entry_type &s1, const entry_type &s2) const
            {
                if(s1.sock!=s2.sock)
                    return s1.sock<s2.sock;
                if(s1.events!=s2.events)
                    return s1.events<s2.events;
                return s1.timeout_ms<s2.timeout_ms;
                //return s1.sock==s2.sock && s1.events==s2.events && s1.timeout_ms==s2.timeout_ms;
            }
        };
        typedef std::multiset<entry_type,entry_type_lower> set_type;

    public:
        reactor_poll();

        void register_event(int sock, event_type events, int timeout_ms);
        void deregister_event(int sock, event_type events, int timeout_ms);
        void event(int &sock, event_type &revent, event_descriptor::result_type &result);
        void quit_loop();

    private:
        void notify_reactor_poll(bool quit=false);
        bool empty_notifier();

    private:
        set_type rp_set;
        mutex rp_set_mutex;
        struct pollfd *fds;
        unsigned int nfds;
        struct pollfd *current;
        int active;
        bool reform;
        int notifier[2];
};

}

#endif
