#ifndef DOLLER_SERVER_H
#define DOLLER_SERVER_H

#include <doller/config.h>
#include <boiler/classic_server.h>

namespace doller
{
    using boiler::string;
    using boiler::classic_server;
    using boiler::config_t;
    using boiler::socket_type;
    using boiler::string;
    using boiler::fsa_st;
    using std::endl;
    typedef socket_type::resolver_type resolver_type;

    class server : public classic_server
    {
        public:
            server(const config_t &cf) : classic_server(cf) {}
            virtual ~server() {}

            static boiler::server *run(const config_t &cf);

            virtual void per_client(socket_type *client, resolver_type *resolver, string &extra_access_log);

        protected:
            virtual void read_config(const boiler::config_t &cf);

    };
}

#endif
