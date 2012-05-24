AC_PREREQ(2.57)
AC_INIT(doller,1.0,virtan@virtan.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/server.cc])
AM_CONFIG_HEADER([config.h])

# Maintainer mode
AM_MAINTAINER_MODE

# Define _GNU_SOURCE
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Lang for tests
AC_LANG(C++)

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
                                                                                                                   
AC_PERVERTED_CHECK_LIB([boiler],[/usr /usr/local],[1],[],[],[
        AC_LANG_PROGRAM([
#include <boiler/server.h>
        ],[
class test_server : public boiler::server
{
    public:
        bool shutdown() {
            return true;
        }
};
        ])
    ],[boiler],[1])
                                                                                                                   
AC_PERVERTED_CHECK_LIB([boost],[/usr /usr/local],[1],[],[],[
        AC_LANG_PROGRAM([
#include <iostream>
#include <boost/spirit.hpp>
        ],[
SPIRIT_VERSION
        ])
    ],[],[1])
                                                                                                                   
AC_PERVERTED_CHECK_LIB([mysqlclient],[/usr /usr/local],[1],[mysql],['' mysql],[
        AC_LANG_PROGRAM([
#include <mysql.h>
        ],[
MYSQL_FIELD mf;
        ])
    ],[mysqlclient_r],[1])

AC_PERVERTED_CHECK_LIB([mysqlpp],[/usr /usr/local],[1],[mysql++],[],[
        AC_LANG_PROGRAM([
#include <tiny_int.h>
        ],[
mysqlpp::tiny_int nt;
        ])
    ],[mysqlpp],[1])

# Options
AC_SYS_LARGEFILE

AC_CONFIG_HOOKER
AC_OUTPUT([Makefile
           include/Makefile
           src/Makefile])
