#include <hot/exceptions.h>

hot::std_exception::std_exception(const string &reason,
    const char *pretty_function, const char *file, int line) :
        string(reason), m_pretty_function(pretty_function?pretty_function:""), m_file(file), m_line(line)
{
}
hot::std_exception::~std_exception() throw()
{
}

const char *hot::std_exception::type() const
{
    return "std_exception";
}

const char *hot::std_exception::function() const
{
    return m_pretty_function;
}

const char *hot::std_exception::file() const
{
    return m_file;
}

int hot::std_exception::line() const
{
    return m_line;
}

namespace hot {

EXCEPTION_DEFINITION(sys_exception)
EXCEPTION_DEFINITION(badcode_exception)
EXCEPTION_DEFINITION(lexcast_exception)
EXCEPTION_DEFINITION(invarg_exception)

// socket_exception
socket_exception::socket_exception(const string &reason,
    const char *pretty_function, const char *file, int line) :
        std_exception(reason,pretty_function,file,line),
            std::ios_base::failure(reason.c_str())
{
}

socket_exception::~socket_exception() throw()
{
}

const char *socket_exception::type() const
{
    return "socket_exception";
}

EXCEPTION_DEFINITION_2(resolve_socket_exception, socket_exception)
EXCEPTION_DEFINITION_2(connect_socket_exception, socket_exception)
EXCEPTION_DEFINITION_2(exchange_socket_exception, socket_exception)

EXCEPTION_DEFINITION(logic_exception)

}
