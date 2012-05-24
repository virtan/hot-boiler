#ifndef H_EXCEPTIONS_H
#define H_EXCEPTIONS_H

#include <hot/config.h>
#include <hot/string.h>
#include <exception>

#ifndef __GNUC__
#define __PRETTY_FUNCTION__ ""
#endif

namespace hot {

class std_exception : public string, public std::exception
{
    public:
        std_exception(const string &reason=string("unspecified reason"),
            const char *pretty_function=0, const char *file="", int line=-1);
        virtual ~std_exception() throw();
        virtual const char *type() const;
        virtual const char *function() const;
        virtual const char *file() const;
        virtual int line() const;
        inline const char *what() const throw() {
            return this->c_str();
        }

    protected:
        const char *m_pretty_function;
        const char *m_file;
        const int m_line;
};

#define EXCEPTION_DECLARATION(target_exception) \
class target_exception : public std_exception \
{ \
    public: \
        target_exception(const string &reason=string("unspecified reason"), \
            const char *pretty_function="", const char *file="", int line=-1); \
        virtual const char *type() const; \
        virtual ~target_exception() throw(); \
}; \

#define EXCEPTION_DECLARATION_2(target_exception,base_exception) \
class target_exception : public base_exception \
{ \
    public: \
        target_exception(const string &reason=string("unspecified reason"), \
            const char *pretty_function="", const char *file="", int line=-1); \
        virtual const char *type() const; \
        virtual ~target_exception() throw(); \
}; \


// Definitions

#define EXCEPTION_DEFINITION(target_exception) \
target_exception::target_exception(const string &reason, \
    const char *pretty_function, const char *file, int line) : \
        std_exception(reason,pretty_function,file,line) \
{ \
} \
\
target_exception::~target_exception() throw() \
{ \
} \
\
const char *target_exception::type() const \
{ \
    return #target_exception; \
} \


#define EXCEPTION_DEFINITION_2(target_exception, base_exception) \
target_exception::target_exception(const string &reason, \
    const char *pretty_function, const char *file, int line) : \
        base_exception(reason,pretty_function,file,line) \
{ \
} \
\
target_exception::~target_exception() throw() \
{ \
} \
\
const char *target_exception::type() const \
{ \
    return #target_exception; \
} \


// System exception (not enough memory, too many open files or so)
EXCEPTION_DECLARATION(sys_exception)

// Bad Code exception (invalid argument, deadlock or so)
EXCEPTION_DECLARATION(badcode_exception)

// Bad lexical cast exception (unserialized type)
EXCEPTION_DECLARATION(lexcast_exception)

// Invalid argument exception
EXCEPTION_DECLARATION(invarg_exception)

// Socket operations exception (includes resolve/connect/exchange real socket exceptions)
class socket_exception : public std_exception, public std::ios_base::failure
{
    public:
        socket_exception(const string &reason=string("unspecified reason"),
            const char *pretty_function="", const char *file="", int line=-1);
        virtual const char *type() const;
        virtual ~socket_exception() throw();
        inline const char *what() const throw() {
            return this->c_str();
        }

};

// Socket resolve operations exception (can't resolve)
EXCEPTION_DECLARATION_2(resolve_socket_exception,socket_exception)

// Socket connect operations exception (can't connect/bind)
EXCEPTION_DECLARATION_2(connect_socket_exception,socket_exception)

// Socket exchange operations exception (can't read/write/close)
EXCEPTION_DECLARATION_2(exchange_socket_exception,socket_exception)

// Uplevel logic exception
EXCEPTION_DECLARATION(logic_exception)

}

#define EXCPLACE __PRETTY_FUNCTION__,__FILE__,__LINE__

#endif
