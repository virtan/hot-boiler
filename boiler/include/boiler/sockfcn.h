#ifndef B_SOCKFCN_H
#define B_SOCKFCN_H

#include <boiler/config.h>
#include <boiler/exceptions.h>
#include <hot/sockfcn.h>
#include <boiler/event.h>
#include <boiler/tp_functor_st.h>
#include <boiler/safe_errno.h>
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
*/
/*
#include <string.h>
*/
#include <setjmp.h>

namespace hot
{

    using namespace boiler;

    template <class Domain, class Type>
    class sockfcn<Domain,Type,char,1> : public sockfcn<Domain,Type,char,0>
    {
        private:
            typedef sockfcn<Domain,Type,char,0> ancestor_t;

        public:
            sockfcn(bool _nonblock=true) : ancestor_t(_nonblock) {}
            sockfcn(int _existent_sock, bool _nonblock=true) : ancestor_t(_existent_sock, _nonblock) {}
            sockfcn(typename ancestor_t::__sockfcn &donor, struct sockaddr *ra, socklen_t *ra_len, bool nowait=false) : ancestor_t(donor, ra, ra_len, nowait) {}

            virtual bool can_read() {
                return this->wait_no_reactor(e_read,0,VL_SOCK_WAIT_EXCH);
            }

            virtual bool can_write() {
                return this->wait_no_reactor(e_write,0,VL_SOCK_WAIT_EXCH);
            }

        protected:
            virtual bool wait(event_type events, int timeout_ms, int exc_type) {
#ifdef THREAD_PER_CLIENT_MODE
                return wait_no_reactor(events, timeout_ms, exc_type);
#endif
                boiler::stack_t *execution_stack=execution_stack_of_task.get_specific();
                // int stack_size=functor_adapter_st<Domain>::stack_size;
                // deliver blocking event
                event_descriptor &ed=execution_stack->ed();
                ed.sock=this->sock;
                ed.events=events;
                ed.timeout_ms=timeout_ms;
                // switch context
                if(!_setjmp(execution_stack->suspend_jmp())) {
                    _longjmp(execution_stack->control_jmp(),1);
                }
                // get blocking event result and return
                return ed.result==event_descriptor::ok;
            }
            
            virtual bool wait_no_reactor(event_type events, int timeout_ms, int exc_type) {
                return this->ancestor_t::wait(events, timeout_ms, exc_type);
            }
    };
}

namespace boiler
{
    using hot::sockfcn;
}

#endif
