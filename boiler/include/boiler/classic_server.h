#ifndef BCLASSICSERVER_H
#define BCLASSICSERVER_H

#include <boiler/config.h>
#include <boiler/event.h>
#include <boiler/server.h>
#include <boiler/reactor.h>
#include <boiler/sockstream.h>
#include <boiler/bconfig.h>
#include <hot/rwlock.h>

namespace boiler {

typedef sockstream<> socket_type;
class classic_server;

class acceptor_event_descriptor : public boiler::event_descriptor
{
    public:
        acceptor_event_descriptor() : srv(0), reactor(0), acceptor_(0)
        {
            long_life=true;
            events=e_accept;
            timeout_ms=-1;
        }
        virtual ~acceptor_event_descriptor()
        {
            delete acceptor_;
        }
        virtual void put_on_queue();
        inline socket_type &acceptor() {
            if(!acceptor_) {
                acceptor_=new socket_type();
                sock=acceptor_->socket();
            }
            return *acceptor_;
        }

    public:
        classic_server *srv;
        string *reactor;

    private:
        socket_type *acceptor_;
};

class classic_server : public boiler::server
{
    public:
        classic_server(const boiler::config_t &cf) : client_buffer_size(512), bind_pool_size(256), re_bind_period(5), listening(false) {
            my_name=(const char *) cf;
        }
        virtual ~classic_server();

        static boiler::server *run(const boiler::config_t &cf);

        virtual const char *name() {
            return my_name.c_str();
        }
        virtual bool shutdown();

        virtual void per_client_wrapper(socket_type *client, socket_type::resolver_type *resolver);
        virtual void per_client(socket_type *client, socket_type::resolver_type *resolver, string &extra_access_log)=0;

    protected:
        virtual void read_config(const boiler::config_t &cf)=0;
        virtual void start();
        virtual void after_start() {};

    protected:
        acceptor_event_descriptor aed;
        int client_buffer_size;
        int bind_pool_size;
        int re_bind_period;
        string bind_address; // must be filled in read_config
        string thread_pool; // must be filled in read_config
        string reactor; // must be filled in read_config
        string my_name;
        rwlock users_lock;
        bool listening;
};

#define CLASSIC_SERVER_DECLARATION(server_name) \
extern "C" { \
\
boiler::server *BOILER_SERVER_DECLARATION(const boiler::config_t &cf) \
{ \
            return server_name::run(cf); \
} \
\
} \

#define CLASSIC_SERVER_RUN_DECLARATION(server_name) \
boiler::server *server_name::run(const boiler::config_t &cf) \
{ \
    server_name *me=new server_name(cf); \
    me->read_config(cf); \
    boiler::pool(me->thread_pool)->putq(boiler::fsa_st(me->reactor, (void (server_name::*)()) &server_name::start, me)); \
    return me; \
} \


}

#endif
