#include <hot/system_resolver.h>

namespace hot {

    template <>
        string system_resolver<IPv4,TCP>::compile_ip() const {
            if(!compiled_ip.empty())
                return compiled_ip;
            uint32_t addr=ntohl(((struct sockaddr_in*) &sa)->sin_addr.s_addr);
            for(int i=0;i<4;i++) {
                if(i>0)
                    compiled_ip+='.';
                compiled_ip+=lexical_cast<string>(addr>>24);
                addr<<=8;
            }
            compiled_ip+=':';
            uint16_t port=htons(((struct sockaddr_in*) &sa)->sin_port);
            compiled_ip+=lexical_cast<string>(port);
            return compiled_ip;
        }

    template <>
        socklen_t *system_resolver<IPv4,TCP>::sockaddr_len_in() {
            sa_len=IPv4::sa_len();
            return &sa_len;
        }

    template<>
        string system_resolver<IPv4,TCP>::ip_only() const {
            if(compiled_ip.empty())
                compile_ip();
            string::size_type o=compiled_ip.find(':');
            if(o==string::npos)
                return string();
            return compiled_ip.substr(0,o);
        }

    template<>
        string system_resolver<IPv4,TCP>::port_only() const {
            if(compiled_ip.empty())
                compile_ip();
            string::size_type o=compiled_ip.find(':');
            if(o==string::npos)
                return string();
            return compiled_ip.substr(o+1,string::npos);
        }

}
