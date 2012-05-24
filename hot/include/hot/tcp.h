#ifndef H_TCP_H
#define H_TCP_H

#include <hot/config.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace hot
{
    class TCP
    {
        public:
            static const int type_id() {
                return SOCK_STREAM;
            }
    };
}

#endif
