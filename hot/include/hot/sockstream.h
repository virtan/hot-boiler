#ifndef H_SOCKSTREAM_H
#define H_SOCKSTREAM_H

#include <hot/config.h>
#include <iostream>
#ifdef STLPORT
#include <stl/char_traits.h>
#else
#include <bits/char_traits.h>
#endif

#include <hot/sockbuf.h>
#include <hot/system_resolver.h>
#include <hot/ipv4.h>
#include <hot/tcp.h>

namespace hot
{
    using std::basic_istream;
    using std::basic_ostream;
    using std::char_traits;

    template<class Domain=IPv4, class Type=TCP, template <class _Domain, class _Type> class Resolver=hot::system_resolver, typename _CharT=char, typename _Traits=char_traits<_CharT>, int Socket_Fcn_Selector=0>
        class sockstream
        {
            public:
                typedef _CharT char_type;
                typedef _Traits traits_type;

                typedef sockbuf<Domain, Type, char_type, traits_type, Socket_Fcn_Selector> sockbuf_type;
                typedef basic_istream<char_type, traits_type> istream_type;
                typedef basic_ostream<char_type, traits_type> ostream_type;
                typedef sockstream<Domain, Type, Resolver, char_type, traits_type, Socket_Fcn_Selector> sockstream_type;

                typedef std::ios_base ios_base;
                typedef ios_base::failure failure;

                typedef Resolver<Domain, Type> resolver_type;

            protected:
                sockbuf_type sockbuf;
                resolver_type *resolver_;

            public:
                istream_type in;
                ostream_type out;

            public:
                sockstream() : resolver_(0), in(&sockbuf), out(&sockbuf)
                {}

                sockstream(const resolver_type &resolver) : in(&sockbuf), out(&sockbuf)
                {
                    this->connect(resolver);
                }

                sockstream(sockstream &donor, resolver_type &r, bool nowait=false) : sockbuf(donor.sockbuf,r.sockaddr(),r.sockaddr_len_in(), nowait), in(&sockbuf), out(&sockbuf)
                {
                }

                sockstream(const sockstream &rh) : in(&sockbuf), out(&sockbuf)
                {
                }

                virtual ~sockstream()
                {
                }

                virtual ios_base::iostate exceptions() const
                {
                    ios_base::iostate res=(in.exceptions() | out.exceptions()) & (~ios_base::goodbit);
                    res|=(in.exceptions() & out.exceptions()) & ios_base::goodbit;
                    return res;
                }

                virtual ios_base::iostate exceptions(ios_base::iostate except)
                {
                    ios_base::iostate prev=exceptions();
                    in.exceptions(except);
                    out.exceptions(except);
                    return prev;
                }

                virtual void set_timeout(int tmout_type, int value_ms) { // 0 - connect, 1 - read, 2 - write
                    sockbuf.set_timeout(tmout_type,value_ms);
                }

                virtual bool connect(const resolver_type &resolver)
                {
                    bool res=sockbuf.connect(resolver.sockaddr(),resolver.sockaddr_len());
                    if(!res) {
                        in.setstate(std::ios_base::badbit);
                        out.setstate(std::ios_base::badbit);
                    }
                    return res;
                }

                virtual bool can_read() {
                    return sockbuf.can_read();
                }

                virtual bool can_write() {
                    return sockbuf.can_write();
                }

                virtual void close()
                {
                    sockbuf.close();
                }

                virtual bool accept(sockstream_type &donor, resolver_type &r)
                {
                    return sockbuf.accept(donor.sockbuf,r.sockaddr(),r.sockaddr_len_in());
                }

                virtual bool accept(sockstream_type &donor)
                {
                    return sockbuf.accept(donor.sockbuf,NULL,NULL);
                }

                virtual bool accept_nowait(sockstream_type &donor, resolver_type &r)
                {
                    return sockbuf.accept(donor.sockbuf,r.sockaddr(),r.sockaddr_len_in(),true);
                }

                virtual bool accept_nowait(sockstream_type &donor)
                {
                    return sockbuf.accept(donor.sockbuf,NULL,NULL,true);
                }

                virtual void bind(const resolver_type &r, int backlog=128)
                {
                    sockbuf.bind(r.sockaddr(),r.sockaddr_len(), backlog);
                }

                virtual sockbuf_type *buf()
                {
                    return &sockbuf;
                }

                virtual int socket() const
                {
                    return sockbuf.socket();
                }

                virtual void use_external_socket(int esock, bool eisopen)
                {
                    sockbuf.use_external_socket(esock,eisopen);
                }

                virtual resolver_type *resolver()
                {
                    return resolver_;
                }

                virtual void resolver(resolver_type *p)
                {
                    resolver_=p;
                }

                virtual resolver_type current_name() const
                {
                    resolver_type r;
                    sockbuf.current_name(r.sockaddr(),r.sockaddr_len_in());
                    return r;
                }

                virtual void shutdown(int type) // 0 - read, 1 - write, other - both
                {
                    sockbuf.shutdown(type);
                }
        };

    typedef sockstream<IPv4,TCP,hot::system_resolver,char,char_traits<char>,0> socket_t;
}

#endif
