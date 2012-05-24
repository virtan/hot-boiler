#ifndef B_EXCEPTIONS_H
#define B_EXCEPTIONS_H

#include <boiler/config.h>
#include <hot/exceptions.h>

namespace boiler {

typedef hot::std_exception std_exception;
typedef hot::sys_exception sys_exception;
typedef hot::badcode_exception badcode_exception;
typedef hot::lexcast_exception lexcast_exception;
typedef hot::invarg_exception invarg_exception;
typedef hot::socket_exception socket_exception;
typedef hot::resolve_socket_exception resolve_socket_exception;
typedef hot::connect_socket_exception connect_socket_exception;
typedef hot::exchange_socket_exception exchange_socket_exception;
typedef hot::logic_exception logic_exception;

}

#endif
