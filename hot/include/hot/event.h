#ifndef H_EVENT_H
#define H_EVENT_H

#include <hot/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

namespace hot
{

    typedef enum { e_read=1, e_write=2, e_accept=4, e_connect=8 } event_type;

    inline short event_to_poll(const event_type &e) {
        register short res=0;
        if(e&e_read || e&e_accept)
            res|=POLLIN;
        if(e&e_write || e&e_connect)
            res|=POLLOUT;
        return res;
    }

    inline event_type poll_to_event(short e) {
        register event_type res=event_type(0);
        if(e&POLLIN)
            res=event_type(res|e_read|e_accept);
        if(e&POLLOUT)
            res=event_type(res|e_write|e_connect);
        return res;
    }              
}


#endif
