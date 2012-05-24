#ifndef B_SOCKSTREAM_H
#define B_SOCKSTREAM_H

#include <boiler/config.h>
#include <iostream>
#ifdef STLPORT
#include <stl/char_traits.h>
#else
#include <bits/char_traits.h>
#endif

#include <boiler/sockbuf.h>
#include <boiler/system_resolver.h>
#include <boiler/ipv4.h>
#include <boiler/tcp.h>
#include <hot/sockstream.h>

namespace boiler
{
    using std::basic_istream;
    using std::basic_ostream;
    using std::char_traits;

    template<class Domain=IPv4, class Type=TCP, template <class _Domain, class _Type> class Resolver=hot::system_resolver, typename _CharT=char, typename _Traits=char_traits<_CharT>, int Socket_Fcn_Selector=1>
        class sockstream : public hot::sockstream<Domain, Type, Resolver, _CharT, _Traits, Socket_Fcn_Selector>
        {
            public:
                typedef hot::sockstream<Domain, Type, Resolver, _CharT, _Traits, Socket_Fcn_Selector> ancestor_t;

            public:
                sockstream() {}
                sockstream(const typename ancestor_t::resolver_type &resolver) : ancestor_t(resolver) {}
                sockstream(sockstream &donor, typename ancestor_t::resolver_type &r, bool nowait=false) : ancestor_t(donor, r, nowait) {}
                sockstream(const sockstream &rh) : ancestor_t(rh) {}

                virtual void bind(const typename ancestor_t::resolver_type &r, int backlog=128, bool privileged_port=false, bool privileged_port_init=false)
                {
                    if(privileged_port && !privileged_port_init && globals().privileged_port_bound(r)) {
                        typename ancestor_t::sockstream_type *t=(typename ancestor_t::sockstream_type*)(globals().privileged_ports()[r]);
                        this->use_external_socket(t->socket(),true);
                        return;
                    }
                    this->ancestor_t::bind(r, backlog);
                }
        };

    /*typedef sockstream<IPv4, TCP, hot::system_resolver, char, char_traits<char>, 1> socket_type*/
}

#endif
