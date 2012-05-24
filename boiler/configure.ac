AC_PREREQ(2.57)
AC_INIT(boiler,0.9,virtan@virtan.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([environment/bconfig.cc])
AM_CONFIG_HEADER([config.h])

# Maintainer mode
AM_MAINTAINER_MODE

# Boiler environment flags
AH_TEMPLATE(BOILER_ENVIRONMENT,[boiler environment flag])
AC_DEFINE([BOILER_ENVIRONMENT],[1])
AH_TEMPLATE(BOILER_VERSION,[boiler environment version])
AC_DEFINE([BOILER_VERSION],[1.0])


# Define _GNU_SOURCE
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Lang for tests
AC_LANG(C++)

# Sys conf dir definition
AH_TEMPLATE(SYSCONFDIR,[system configuration directory])
AC_DEFINE_UNQUOTED([SYSCONFDIR],["`eval "echo $sysconfdir"`"])

# threads
AC_THREADS_LIBRARY

# Extra portability mode
AH_TEMPLATE(THREAD_PER_CLIENT_MODE,[one thread per client mode - for portability purposes])
AC_ARG_ENABLE(extra-portability,AC_HELP_STRING([--enable-extra-portability],[optimize for portability instead of speed]),
    AC_DEFINE(THREAD_PER_CLIENT_MODE,1)
    AC_MSG_NOTICE("Extra portability mode enabled")
    ,
)

# Perverted checks
AC_PERVERTED_CHECK_LIB([hot],[/usr /usr/local],[1],[],[],[
        AC_LANG_PROGRAM([
#include <hot/string.h>
#include <hot/exceptions.h>
        ],[
hot::string s;
hot::std_exception t;
        ])
    ],[hot],[1])
                                                                                                                   
AC_PERVERTED_CHECK_LIB([kqueue],[/usr /usr/local],[1],[],[],[
        AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
        ],[
kqueue();
        ])
    ],[],[])
                                                                                                                   
AC_PERVERTED_CHECK_LIB([epoll],[/usr /usr/local],[1],[],[],[
        AC_LANG_PROGRAM([
#include <sys/epoll.h>
        ],[
epoll_create(256);
        ])
    ],[],[])
                                                                                                                   
AC_PERVERTED_CHECK_LIB([libxml2],[/usr /usr/local],[1],[libxml2],[],[
        AC_LANG_PROGRAM([
#include <libxml/tree.h>
        ],[
xmlNodePtr p;
if(p)
return 0;
        ])
    ],[xml2],[1])
                                                                                                                   

AC_DL_CHECK([1])

AC_CONFIG_HOOKER
AC_OUTPUT([Makefile
           include/Makefile
           etc/Makefile
           environment/Makefile
           thread_pool/Makefile
           reactor/Makefile
           grouping/Makefile
           control/Makefile
           templates/Makefile
           templates/classic_server/Makefile
           templates/classic_server/doller/Makefile
           templates/classic_server/doller/include/Makefile
           templates/classic_server/doller/src/Makefile])
