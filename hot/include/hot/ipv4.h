#ifndef H_IPV4_H
#define H_IPV4_H

#include <hot/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace hot
{
    class IPv4
    {
        public:
            static int domain_id() {
                return PF_INET;
            }
            static size_t sa_len() {
                return sizeof(struct sockaddr_in);
            }
    };
}

#endif
