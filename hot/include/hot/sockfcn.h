#ifndef H_SOCKFCN_H
#define H_SOCKFCN_H

#include <hot/config.h>
#include <errno.h>
#include <hot/exceptions.h>
#include <hot/safe_errno.h>
#include <hot/event.h>
//#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

namespace hot
{
    template<class Domain, class Type, typename _CharT, int Sock_Fcn_Selector>
    class sockfcn;

    template<class Domain, class Type>
    class sockfcn<Domain,Type,char,0>
    {
        protected:
            typedef sockfcn<Domain,Type,char,0> __sockfcn;

        public:
            sockfcn(bool _nonblock=true) : conn_timeout(-1), read_timeout(-1), write_timeout(-1), nonblock(_nonblock), isopen(false), need_to_close(false) {
                this->init();
            }
            sockfcn(int _existent_sock, bool _nonblock=true) : sock(_existent_sock), conn_timeout(-1), read_timeout(-1), write_timeout(-1), nonblock(_nonblock), isopen(true), need_to_close(true) /* ??? */ {
                if(sock<0)
                    throw socket_exception(safe_errno(EBADF),EXCPLACE);
            }

            sockfcn(__sockfcn &donor, struct sockaddr *ra, socklen_t *ra_len, bool nowait=false) : need_to_close(false) {
                this->accept(donor,ra,ra_len,nowait);
                // TODO: can return false !!!
            }

            virtual ~sockfcn() {
                this->real_close();
            }

            virtual void set_timeout(int tmout_type, int value_ms) {
                switch(tmout_type) {
                    case 0:
                        conn_timeout=value_ms;
                        break;
                    case 1:
                        read_timeout=value_ms;
                        break;
                    case 2:
                        write_timeout=value_ms;
                        break;
                }
            }

#define VL_SOCK_WAIT_CONN 1
#define VL_SOCK_WAIT_EXCH 2
            virtual bool connect(const struct sockaddr *sa, socklen_t sa_len) {
                int rc=::connect(sock,sa,sa_len);
                if(rc<0 && errno==EINPROGRESS && nonblock && conn_timeout>0) {
                    bool res=this->wait(e_connect /*POLLOUT*/,conn_timeout,VL_SOCK_WAIT_CONN);
                    if(!res)
                        return res;
                    rc=0;
                }
                if(rc<0)
                    throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                return isopen=true;
            }

            virtual bool can_read() {
                return this->wait(e_read /*POLLIN*/,0,VL_SOCK_WAIT_EXCH);
            }

            virtual bool can_write() {
                return this->wait(e_write /*POLLOUT*/,0,VL_SOCK_WAIT_EXCH);
            }

            virtual void bind(const struct sockaddr *sa, socklen_t sa_len, int backlog) {
                int rc=::bind(sock,sa,sa_len);
                if(rc<0)
                    throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                rc=::listen(sock,backlog);
                if(rc<0)
                    throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                isopen=true;
            }

            virtual void use_external_socket(int esock, bool eisopen)
            {
                this->real_close();
                sock=esock;
                isopen=eisopen;
                need_to_close=false;
            }

            virtual bool accept(__sockfcn &donor, struct sockaddr *ra=NULL, socklen_t *ra_len=NULL, bool nowait=false) {
                int rc=donor.accept_for_me(ra,ra_len,nowait);
                if(rc<0)
                    return false;
                real_close();
                *this=donor;
                sock=rc;
                if(nonblock)
                    set_nonblock(sock);
                isopen=true;
                need_to_close=true;
                conn_timeout=read_timeout=write_timeout=-1;
                return true;
            }

            virtual bool is_open() const {
                return isopen;
            }

            virtual void close() {
                this->real_close();
                this->init();
            }

            virtual size_t get(char *s, size_t s_len) {
                if(!s_len)
                    return s_len;
                int res;
                while(1) {
                    res=::read(sock,s,s_len);
                    //std::cerr << "::read got " << res << '(' << errno << ',' << nonblock << ',' << read_timeout << ')' << std::endl;
                    if(res<0 && errno==EAGAIN && nonblock && read_timeout>0) {
                        res=this->wait(e_read /*POLLIN*/,read_timeout,VL_SOCK_WAIT_EXCH);
                        if(!res)
                            return size_t(-1);
                        continue;
                    }
                    break;
                }
                if(res<0)
                    throw exchange_socket_exception(safe_errno(errno),EXCPLACE);
                else if(res==0)
                    return size_t(-1);
                //std::cerr << "request for " << s_len << ", returned " << res  <<std::endl;
                return res;
            }

            virtual size_t put(const char *s, size_t s_len) {
                if(!s_len)
                    return s_len;
                int res;
                while(1) {
                    res=::write(sock,s,s_len);
                    if(res<0 && errno==EAGAIN && nonblock && write_timeout>0) {
                        res=this->wait(e_write /*POLLOUT*/,write_timeout,VL_SOCK_WAIT_EXCH);
                        if(!res)
                            return size_t(-1);
                        continue;
                    }
                    break;
                }
                if(res<0) {
                    throw exchange_socket_exception(safe_errno(errno),EXCPLACE);
                } else if(res==0) {
                    throw exchange_socket_exception(safe_errno(EBADF),EXCPLACE);
                }
                return res;
            }

            virtual size_t put_all(const char *s, size_t s_len) {
                size_t already=0;
                do {
                    size_t this_try=put(s+already,s_len-already);
                    if(this_try == size_t(-1)) {
                        already=size_t(-1);
                        break;
                    }
                    already+=this_try;
                } while(already!=s_len);
                return already;
            }
 
            virtual int socket() const {
                return sock;
            }

            virtual bool current_name(struct sockaddr *ra, socklen_t *ra_len) const {
                int rc=getsockname(sock,ra,ra_len);
                if(rc<0)
                    return false;
                return true;
            }

            virtual void shutdown(int type) // 0 - read, 1 - write, other - both
            {
                int how;
                switch(type) {
                    case 0: how=SHUT_RD; break;
                    case 1: how=SHUT_WR; break;
                    default: how=SHUT_RDWR; break;
                }
                ::shutdown(sock,how);
            }

        protected:
            virtual bool wait(event_type events, int timeout_ms, int exc_type) {
                struct pollfd ufd;
                ufd.fd=sock;
                ufd.events=event_to_poll(events);
                int rc;
                while((rc=poll(&ufd,1,timeout_ms))<0 && errno==EINTR);
                if(rc<0)
                    switch(exc_type) {
                        case VL_SOCK_WAIT_CONN:
                            throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                        case VL_SOCK_WAIT_EXCH:
                            throw exchange_socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                        default:
                            throw socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                    }
                else if(rc==0) {
                    // std::cerr << "wait return false" << std::endl;
                    return false;
                }
                socklen_t rc_len=sizeof(rc);
                if(getsockopt(sock,SOL_SOCKET,SO_ERROR,&rc,&rc_len)<0)
                    switch(exc_type) {
                        case VL_SOCK_WAIT_CONN:
                            throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                        case VL_SOCK_WAIT_EXCH:
                            throw exchange_socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                        default:
                            throw socket_exception(safe_errno(errno),EXCPLACE);
                            break;
                    }
                if(rc)
                    switch(exc_type) {
                        case VL_SOCK_WAIT_CONN:
                            throw connect_socket_exception(safe_errno(rc),EXCPLACE);
                            break;
                        case VL_SOCK_WAIT_EXCH:
                            throw exchange_socket_exception(safe_errno(rc),EXCPLACE);
                            break;
                        default:
                            throw socket_exception(safe_errno(rc),EXCPLACE);
                            break;
                    }
                return true;
            }

            virtual int accept_for_me(struct sockaddr *ra, socklen_t *ra_len, bool nowait) {
                int rc=::accept(sock,ra,ra_len);
                if(rc<0 && errno==EAGAIN && nonblock && conn_timeout>0 && !nowait) {
                    bool res=this->wait(e_accept /*POLLIN*/,conn_timeout,VL_SOCK_WAIT_CONN);
                    if(!res)
                        return (-1); // timeout
                    rc=::accept(sock,ra,ra_len);
                }
                if(rc<0)
                    throw connect_socket_exception(safe_errno(errno),EXCPLACE);
                return rc;
            }

            virtual void init() {
                sock=::socket(Domain::domain_id(),Type::type_id(),0);
                if(sock<0)
                    throw socket_exception(safe_errno(errno),EXCPLACE);
                need_to_close=true;
                int val=1;
                setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
                if(nonblock)
                    set_nonblock(sock);
            }

            virtual void real_close()
            {
                if(need_to_close)
                    ::close(sock);
            }

        public:
#ifdef O_NONBLOCK
            static void set_nonblock(int the_sock) {
                // std::cerr << "set_nonblock " << the_sock << std::endl;
                int flags=fcntl(the_sock, F_GETFL, 0);
                if(flags<0)
                    return;
                fcntl(the_sock, F_SETFL, flags | O_NONBLOCK);
            }
            static void set_block(int the_sock) {
                // std::cerr << "set_block " << the_sock << std::endl;
                int flags=fcntl(the_sock, F_GETFL, 0);
                if(flags==-1)
                    return;
                fcntl(the_sock, F_SETFL, flags & (~O_NONBLOCK));
            }
#else
            static void set_nonblock(int the_sock) {
                // std::cerr << "set_nonblock2 " << the_sock << std::endl;
                int yes=1;
                ioctl(the_sock, FIONBIO, &yes);
            }
            static void set_block(int the_sock) {
                int no = 0;
                ioctl(the_sock, FIONBIO, &no);
            }
#endif

        protected:
            int sock;
            unsigned int conn_timeout;
            unsigned int read_timeout;
            unsigned int write_timeout;
            bool nonblock;
            bool isopen;
            bool need_to_close;
    };
}

#endif
