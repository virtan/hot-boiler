#include <boiler/classic_server.h>

void boiler::acceptor_event_descriptor::put_on_queue()
{
    try {
        socket_type::resolver_type *resolver=new socket_type::resolver_type;
        socket_type *client=new socket_type(this->acceptor(), *resolver, true);
        tp->putq(fsa_st(*reactor,&classic_server::per_client_wrapper,srv,client,resolver));
    } catch(socket_type::failure &e) {
        // simply ignore ?
    }
}

bool boiler::classic_server::shutdown()
{
    boiler::globals().reactors()[reactor]->deregister_event(aed);
    listening=false;
    users_lock.wrlock();
    users_lock.unlock();
    return true;
}

boiler::classic_server::~classic_server()
{
    if(listening)
        this->classic_server::shutdown();
}

void boiler::classic_server::start()
{
    while(true) {
        try {
            aed.acceptor().exceptions(socket_type::ios_base::eofbit|socket_type::ios_base::failbit|socket_type::ios_base::badbit);
            aed.acceptor().set_timeout(0,3000);
            aed.acceptor().set_timeout(1,0);
            aed.acceptor().set_timeout(2,0);
            aed.acceptor().bind(bind_address,bind_pool_size,true);
            aed.tp=boiler::pool(thread_pool.c_str());
            aed.srv=this;
            aed.reactor=&reactor;
            boiler::globals().reactors()[reactor]->register_event(aed);
            listening=true;
            break;
        } catch(socket_type::failure &e) {
            log_error("server " << this->name() << ": acceptor socket exception: " << e.what());
        }
        sleep(re_bind_period);
    }
    this->after_start();
}

void boiler::classic_server::per_client_wrapper(socket_type *client, socket_type::resolver_type *resolver)
{
    users_lock.rdlock();
    string extra_access_log;
    try {
        client->exceptions(socket_type::ios_base::failbit|socket_type::ios_base::badbit);
        per_client(client,resolver,extra_access_log);
    } catch(socket_type::failure &e) {
        // connection closed (or other problem), just report, real actions in per_client
        if(!extra_access_log.empty())
            extra_access_log+=' ';
        extra_access_log+=string("close failure: ")+e.what();
    } catch(...) {
        if(!extra_access_log.empty())
            extra_access_log+=' ';
        extra_access_log+="close failure: unknown exception";
    }
    log_stream("access","from " << resolver->compile_ip() << ' ' << extra_access_log);
    delete client;
    delete resolver;
    users_lock.unlock();
}

