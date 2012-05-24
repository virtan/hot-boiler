#ifndef B_EVENT_H
#define B_EVENT_H

#include <boiler/config.h>
#include <hot/event.h>

namespace boiler {
    using hot::event_type;
    using hot::e_read;
    using hot::e_write;
    using hot::e_accept;
    using hot::e_connect;
    using hot::event_to_poll;
    using hot::poll_to_event;
}

#endif
