#ifndef H_SOCKBUF_H
#define H_SOCKBUF_H

#include <hot/config.h>
#include <hot/sockfcn.h>
#include <streambuf>

namespace hot
{
    using std::basic_streambuf;
    using std::streamsize;

    template <class Domain, class Type, typename _CharT, typename _Traits, int Socket_Fcn_Selector>
    class sockbuf : public basic_streambuf<_CharT, _Traits>
    {
        private:
            typedef _CharT char_type;
            typedef _Traits traits_type;

            typedef basic_streambuf<char_type, traits_type> streambuf_type;
            typedef sockbuf<Domain, Type, char_type, traits_type, Socket_Fcn_Selector> sockbuf_type;
            typedef char bs_char_type;
            typedef sockfcn<Domain, Type, bs_char_type, Socket_Fcn_Selector> sockfcn_type;

        private:
            sockfcn_type sock;
            char_type *bgbuf;
            char_type *egbuf;
            char_type tiny_buf[2];
            char_type def_buf[8000];

        public:
            sockbuf() {
                _real_setbuf(def_buf,sizeof(def_buf));
            }
            sockbuf(sockbuf_type &donor, struct sockaddr *ra, socklen_t *ra_len, bool nowait=false) : sock(donor.sock,ra,ra_len,nowait) {
                _real_setbuf(def_buf,sizeof(def_buf));
            }
            ~sockbuf() {};

            sockbuf(const sockbuf &rh) : sock(rh.sock), bgbuf(rh.bgbuf), egbuf(rh.egbuf)
            {
                memcpy(def_buf,rh.def_buf,sizeof(def_buf));
                memcpy(tiny_buf,rh.tiny_buf,sizeof(tiny_buf));
            }
            
            bool is_open() const {
                return sock.is_open();
            }

            void set_timeout(int tmout_type, int value_ms) {
                sock.set_timeout(tmout_type,value_ms);
            }

            bool can_read() {
                return sock.can_read();
            }

            bool can_write() {
                return sock.can_write();
            }

            streambuf_type *setbuf(char *s, streamsize n) {
                sync();
                return _real_setbuf(s,n);
            }

            int sync() {
                // setg(bgbuf,bgbuf+1,bgbuf+1); // in doubt
                streamsize n=(this->pptr())-(this->pbase());
                int res=0;
                if(n>0) {
                //std::cerr << "sync" << std::endl;
                    res=(_real_putn(this->pbase(),n)!=n?-1:0);
                    if(res>=0)
                        this->pbump(-n);
                }
                return res;
            }

            int overflow(int c=traits_type::eof()) {
                streamsize n=(this->pptr())-(this->pbase());
                if(n && sync())
                    return traits_type::eof();
                // this->pbump(-n);
                if(c != traits_type::eof()) {
                    char_type cbuf[1];
                    cbuf[0]=c;
                    if(!_real_putn(cbuf,1))
                        return traits_type::eof();
                }
                return 0;
            }

            int underflow() {
                // sync();
                streamsize read=_real_getn(bgbuf+1,egbuf-bgbuf-1);
                /*std::cerr << "underflow got " << read << "  '" ;
                std::cerr.write(bgbuf+1,read);
                std::cerr << "'" << std::endl;*/
                if(!read) {
                    //setg(bgbuf,bgbuf+1,bgbuf+1);
                    return traits_type::eof();
                }
                setg(bgbuf,bgbuf+1,bgbuf+read+1);
                return bgbuf[1];
            }

            int pbackfail(int c=traits_type::eof()) {
                //std::cerr << "pbackfail" << std::endl;
                if(this->gptr()==this->eback())
                    return traits_type::eof();
                char_type *cp=this->gptr()-1;
                if(c!=traits_type::eof())
                    *cp=c;
                setg(this->eback(),cp,this->egptr());
                return 0;
            }

            streamsize xsgetn(char_type *s, streamsize n) {
                streamsize have=(this->egptr())-(this->gptr());
                streamsize x=have>n?n:have;
                if(x>0) {
                    memcpy((char*) s,(const char*) this->gptr(),x*sizeof(char_type));
                    this->gbump(x);
                    if(have==x)
                        setg(bgbuf,bgbuf+1,bgbuf+1);
                }
                while(x<n) {
                    streamsize got=_real_getn(s+x,n-x);
                    if(got==0)
                        break;
                    x+=got;
                }
                return x;
            }

            streamsize xsputn(char_type *s, streamsize n) {
                return sync()==-1?0:_real_putn(s,n);
            }

            bool connect(const struct sockaddr *sa, socklen_t sa_len) {
                return sock.connect(sa,sa_len);
            }

            void close() {
                sock.close();
            }

            void bind(const struct sockaddr *sa, socklen_t sa_len, int backlog) {
                sock.bind(sa,sa_len,backlog);
            }

            bool accept(sockbuf_type &donor, struct sockaddr *ra=NULL, socklen_t *ra_len=NULL, bool nowait=false) {
                return sock.accept(donor.sock,ra,ra_len,nowait);
            }

            int showmanyc()
            {
                //return sock.can_read()?1:0;
                return (int)(((this->egptr())-(this->gptr())))+(sock.can_read()?1:0);
            }

            int socket() const
            {
                return sock.socket();
            }

            void use_external_socket(int esock, bool eisopen)
            {
                sock.use_external_socket(esock,eisopen);
            }

            bool current_name(struct sockaddr *ra, socklen_t *ra_len) const {
                return sock.current_name(ra,ra_len);
            }

            void shutdown(int type) {
                sock.shutdown(type);
            }
 
        private:
            streamsize _real_putn(char_type *s, streamsize n) {
                size_t written=sock.put_all((bs_char_type*) s,(n*sizeof(char_type)/sizeof(bs_char_type)))*sizeof(bs_char_type)/sizeof(char_type);
                return written!=(size_t)(-1)?written:0;
            }

            streamsize _real_getn(char_type *s, streamsize n) {
                size_t got=sock.get((bs_char_type*) s,(n*sizeof(char_type)/sizeof(bs_char_type)))*sizeof(bs_char_type)/sizeof(char_type);
                return got!=(size_t)(-1)?got:0;
            }

            streambuf_type *_real_setbuf(char *s, streamsize n) {
                if(s==0 || n<4) {
                    // no buffering (we don't accept buffers with less than 4 bytes
                    bgbuf=tiny_buf;
                    egbuf=tiny_buf+sizeof(tiny_buf);
                    setg(bgbuf,bgbuf+1,bgbuf+1);
                    setp((char_type*)0,(char_type*)0);
                } else {
                    bgbuf=s;
                    egbuf=s+n/2;
                    setg(bgbuf,bgbuf+1,bgbuf+1);
                    setp((char_type*)(s+n/2),(char_type*)(s+n));
                }
                return this;
            }

    };

}

#endif
