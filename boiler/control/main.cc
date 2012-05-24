#include <boiler/config.h>
#include <boiler/exceptions.h>
#include <boiler/string.h>
#include <boiler/globals.h>
#include <boiler/thread_pool.h>
#include <boiler/reactor.h>
#include <boiler/reactor_poll.h>
#include <boiler/signals.h>
#include <boiler/server.h>
#include <boiler/bconfig.h>
#include <boiler/sockstream.h>
// #include <../tests/simple_server.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

using namespace boiler;

int main(int argc, char **argv) {
    try {
        globals().main_thread()=pthread_self();
        signal_handling_initiation();
        globals().args().define("config","specify xml file to be used as config for boiler");
        globals().args().define("debug","turn on debugging output");
        globals().args().load(argc,argv);
        string config_file=globals().args().arg<string>("config");
        if(config_file.empty())
            config_file=string(SYSCONFDIR)+"/boiler.cfg";
        
        config_t cf;
        cf.parse(config_file);

        if(config_t lcf=cf/"logs")
            globals().log().init(lcf,globals().args().arg<bool>("debug"));

        log_debug("server initializing: config " << config_file);

        if(config_t priv=cf/"privileged") {
            if(geteuid()) {
                log_critical("server initializing: has privileged section, but hasn't root rights");
                exit(1);
            }
            string become_user;
            string become_group;
            for(config_t instr=*priv;instr;++instr) {
                if(instr.name()==string("bind")) {
                    for(config_t addr=*instr;addr;++addr) {
                        string address=(const char *) addr;
                        try {
                            sockstream<> *pss=new sockstream<>;
                            pss->bind(address,256,true,true);
                            globals().privileged_ports()[hot::system_resolver<IPv4,TCP>(address)]=(void*) pss;
                        } catch(connect_socket_exception &e) {
                            log_critical("server initialization: can't bind privileged port " << address << ": " << e);
                            exit(1);
                        }
                    }
                } else if(instr.name()==string("user"))
                    become_user=(const char *) instr;
                else if(instr.name()==string("group"))
                    become_group=(const char *) instr;
            }
            if(!become_group.empty()) {
                // since we aren't in multithread we can use not-reenterable functions
                struct group *gr_entry=getgrnam(become_group.c_str());
                if(!gr_entry) {
                    log_critical("server initializing: can't get entry for group " << become_group << ": " << safe_errno(errno));
                    exit(1);
                }
                if(setgid(gr_entry->gr_gid)) {
                    log_critical("server initializing: can't change group: " << safe_errno(errno));
                    exit(1);
                }
            }
            if(!become_user.empty()) {
                // since we aren't in multithread we can use not-reenterable functions
                struct passwd *pw_entry=getpwnam(become_user.c_str());
                if(!pw_entry) {
                    log_critical("server initializing: can't get entry for user " << become_user << ": " << safe_errno(errno));
                    exit(1);
                }
                if(setuid(pw_entry->pw_uid)) {
                    log_critical("server initializing: can't change user: " << safe_errno(errno));
                    exit(1);
                }
            }
        }

        if(config_t tps=cf/"thread_pools")
            for(config_t tp=*tps;tp;++tp)
                globals().thread_pools()[tp.name()]=thread_pool::run(tp);

        if(config_t rs=cf/"reactors")
            for(config_t r=*rs;r;++r)
                globals().reactors()[r.name()]=reactor::run(r);

        if(config_t ss=cf/"servers")
            for(config_t s=*ss;s;++s)
                globals().servers()[s.name()]=server::run(s);

        // TODO: plan how to make waiting
        log_stream("control","server started");
        while(true) {
            signal_notification_wait();
            // std::cerr << "got signal notification" << std::endl;
            if(globals().terminate()) {
                log_stream("control","server is terminating...");
                globals_t::server_map_t &servers=globals().servers();
                for(globals_t::server_map_t::iterator i=servers.begin();i!=servers.end();i++) {
                    if(!i->second->shutdown())
                        log_error("can't shutdown server " << i->first << ", so killing it");
                    delete i->second;
                    i->second=0;
                }
                servers.clear();
                globals_t::reactor_map_t &reactors=globals().reactors();
                for(globals_t::reactor_map_t::iterator i=reactors.begin();i!=reactors.end();i++) {
                    delete i->second;
                    i->second=0;
                }
                reactors.clear();
                globals_t::thread_pool_map_t &thread_pools=globals().thread_pools();
                for(globals_t::thread_pool_map_t::iterator i=thread_pools.begin();i!=thread_pools.end();i++) {
                    delete i->second;
                    i->second=0;
                }
                thread_pools.clear();
                break;
            }
            if(globals().respawn()) {
                std::cerr << "respawn action: not implemented yet" << std::endl;
            }
            if(globals().free_logs()) {
                globals().log().free_logs();
                log_stream("control","server reopened logs");
            }
        }
    } catch(xml_exception &e) {
        log_critical("config problems: " << e);
    } catch(invarg_exception &e) {
        log_critical("invalid argument: " << e);
    } catch(std_exception &e) {
        log_critical("got " << e.type() << " exception: " << e << " at " << e.function());
    } catch(...) {
        log_critical("got unknown exception");
    }

    log_stream("control","server stopped");
    destroy_globals();
    return 0;
}

