#ifndef H_RESOLVER_H
#define H_RESOLVER_H

#include <hot/config.h>
#include <hot/exceptions.h>
#include <hot/string.h>
#include <hot/lexical_cast.h>
#include <hot/ipv4.h>
#include <hot/tcp.h>
#include <hot/safe_errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

namespace hot
{
    template <class Domain, class Type>
        struct system_resolver_less;

    template <class Domain, class Type>
        class system_resolver
        {
            public:
                system_resolver() {
                }

                system_resolver(const char *address) {
                    this->resolve(address);
                }

                system_resolver(const string &str) {
                    this->resolve(str.c_str());
                }

                system_resolver(const char *adr1, const char *adr2) {
                    this->resolve(adr1,adr2);
                }

                void resolve(const string &str) {
                    this->resolve(str.c_str());
                }

                void resolve(const char *address) {
                    if(!address)
                        throw resolve_socket_exception(safe_errno(EINVAL));
                    int i;
                    for(i=0;address[i] && address[i]!=':';i++);
                    if(!address[i])
                        throw resolve_socket_exception(safe_errno(EINVAL));
                    string host(address,i);
                    this->resolve(host.c_str(),address+i+1);
                }

                void resolve(const char *adr1, const char *adr2) {
                    struct addrinfo hint,*ain;
                    memset(&hint,0,sizeof(hint));
                    hint.ai_family=Domain::domain_id();
                    hint.ai_socktype=Type::type_id();
                    int res=getaddrinfo(adr1,adr2,&hint,&ain);
                    if(res)
                        throw resolve_socket_exception(gai_strerror(res));
                    memcpy(&sa,ain->ai_addr,ain->ai_addrlen);
                    sa_len=ain->ai_addrlen;
                    freeaddrinfo(ain);
                }

                string compile_ip() const;

                const struct sockaddr *sockaddr() const {
                    return &sa;
                }

                struct sockaddr *sockaddr() {
                    return &sa;
                }

                socklen_t sockaddr_len() const {
                    return sa_len;
                }

                socklen_t *sockaddr_len_in();

                string ip_only() const;
                string port_only() const;

            friend struct system_resolver_less<Domain,Type>;

            private:
                struct sockaddr sa;
                socklen_t sa_len;
                mutable string compiled_ip;
        };

    template <class Domain, class Type>
        struct system_resolver_less
        {
            bool operator()(const system_resolver<Domain,Type> &lh, const system_resolver<Domain,Type> &rh) const {
                if(lh.sa_len!=rh.sa_len)
                    return lh.sa_len<rh.sa_len;
                for(register unsigned int i=0;i<lh.sa_len;i++)
                    if(((char*)&lh.sa)[i]!=((char*)&rh.sa)[i])
                        return ((char*)&lh.sa)[i]<((char*)&rh.sa)[i];
                return false;
            }
        };

}

#endif
