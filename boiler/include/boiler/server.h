#ifndef BSERVER_H
#define BSERVER_H

#include <boiler/config.h>
#include <boiler/thread_pool.h>
#include <boiler/bconfig.h>

namespace boiler {

#define BOILER_SERVER_DECLARATION boiler_server_run
#define _BOILER_SERVER_DECLARATION "boiler_server_run"

// class server is an interface for server used in pool
class server
{
    public:
        static server *run(const config_t &cf);
        virtual ~server() {};
        virtual const char* name()=0;
        virtual bool shutdown()=0;
};

}

#endif
