#ifndef BGLOBALS_H
#define BGLOBALS_H

#include <boiler/config.h>
#include <boiler/string.h>
#include <boiler/exceptions.h>
#include <hot/singleton.h>
#include <boiler/thread_pool.h>
#include <boiler/reactor.h>
#include <boiler/server.h>
#include <boiler/log.h>
#include <hot/options.h>
#include <boiler/stack_pool.h>
#include <boiler/ipv4.h>
#include <boiler/tcp.h>
#include <boiler/system_resolver.h>
#include <map>
#include <sys/types.h>
#include <unistd.h>


namespace boiler {

using hot::options;
using hot::singleton;

class globals_t
{
    public:
        typedef std::map<string, thread_pool*> thread_pool_map_t;
        typedef std::map<string, reactor*> reactor_map_t;
        typedef std::map<string, server*> server_map_t;
        typedef hot::system_resolver<IPv4,TCP> resolver_type;
        typedef std::map<const resolver_type, void*, hot::system_resolver_less<IPv4,TCP> > privileged_ports_map_t;

    public:
        globals_t();
        ~globals_t();

        inline bool thread_pool_exist(const string &name)
        {
            return thread_pools_.find(name)!=thread_pools_.end();
        }
        inline thread_pool_map_t &thread_pools()
        {
            return thread_pools_;
        }
        inline bool reactor_exist(const string &name)
        {
            return reactors_.find(name)!=reactors_.end();
        }
        inline reactor_map_t &reactors()
        {
            return reactors_;
        }
        inline bool server_exist(const string &name)
        {
            return servers_.find(name)!=servers_.end();
        }
        inline server_map_t &servers()
        {
            return servers_;
        }
        inline log_t &log()
        {
            return log_;
        }
        inline options &args()
        {
            return args_;
        }
        inline stack_pool_t &stack_pool()
        {
            return stack_pool_;
        }
        inline privileged_ports_map_t &privileged_ports()
        {
            return privileged_ports_map_;
        }
        inline bool privileged_port_bound(const resolver_type &address) {
            return privileged_ports_map_.find(address)!=privileged_ports_map_.end();
        }
        inline pthread_t &main_thread()
        {
            return main_thread_;
        }
        inline bool &free_logs()
        {
            return free_logs_;
        }
        inline bool &respawn()
        {
            return respawn_;
        }
        inline bool &terminate()
        {
            return terminate_;
        }

    private:
        thread_pool_map_t thread_pools_;
        reactor_map_t reactors_;
        server_map_t servers_;
        log_t log_;
        options args_;
        stack_pool_t stack_pool_;
        privileged_ports_map_t privileged_ports_map_;
        pthread_t main_thread_;
        bool free_logs_;
        bool respawn_;
        bool terminate_;
};

inline globals_t &globals()
{
    return singleton<globals_t>::instance();
}

inline void destroy_globals()
{
    singleton<globals_t>::destroy();
}

}

#endif
