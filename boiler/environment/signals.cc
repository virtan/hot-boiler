#include <boiler/signals.h>
#include <boiler/safe_errno.h>
#include <errno.h>

namespace boiler {

void signal_handling_initiation()
{
    signal_set_action(SIGHUP,sigaction_free_logs);
    signal_set_action(SIGUSR1,sigaction_respawn);
    signal_set_action(SIGTERM,sigaction_terminate);
    signal_set_action(SIGINT,sigaction_terminate);
    signal_set_action(SIGQUIT,sigaction_terminate);
}

void signal_set_action(int signal_number, void (*signal_handler)(int))
{
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));
    sa.sa_handler=signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask,signal_number);
    bool bad=sigaction(signal_number,&sa,NULL);
    if(bad)
        throw sys_exception(safe_errno(errno),EXCPLACE);
}

void signal_block_in_thread(bool do_block)
{
    if(pthread_self()!=globals().main_thread())
        return;
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss,SIGHUP);sigaddset(&ss,SIGUSR1);sigaddset(&ss,SIGTERM);sigaddset(&ss,SIGINT);sigaddset(&ss,SIGQUIT);
    bool bad=pthread_sigmask(do_block?SIG_BLOCK:SIG_UNBLOCK,&ss,NULL);
    if(bad)
        throw sys_exception(safe_errno(errno),EXCPLACE);
}

void signal_interrupt(int signal_number, bool interrupt)
{
    bool bad=siginterrupt(signal_number,interrupt);
    if(bad)
        throw sys_exception(safe_errno(errno),EXCPLACE);
}

void signal_notification_wait()
{
    signal_interrupt(SIGHUP,1);signal_interrupt(SIGUSR1,1);signal_interrupt(SIGTERM,1);signal_interrupt(SIGINT,1);signal_interrupt(SIGQUIT,1);
    while(!sleep(3600));
    signal_interrupt(SIGHUP,0);signal_interrupt(SIGUSR1,0);signal_interrupt(SIGTERM,0);signal_interrupt(SIGINT,0);signal_interrupt(SIGQUIT,0);
}

void sigaction_free_logs(int)
{
    // std::cerr << "signal: free logs" << std::endl;
    globals().free_logs()=true;
}

void sigaction_respawn(int)
{
    // std::cerr << "signal: respawn" << std::endl;
    globals().respawn()=true;
}

void sigaction_terminate(int)
{
    // std::cerr << "signal: terminate" << std::endl;
    globals().terminate()=true;
}


}
