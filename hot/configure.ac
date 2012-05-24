AC_PREREQ(2.57)
AC_INIT(hot,0.9,virtan@virtan.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/exceptions.cc])
AM_CONFIG_HEADER([config.h])

# Maintainer mode
AM_MAINTAINER_MODE

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

# strerror_r prototype
AC_FUNC_STRERROR_R

# threads
AC_THREADS_LIBRARY

# iconv checking
AC_ICONV_CHECK(1)

AC_RT_CHECK([1])

AC_CONFIG_HOOKER
AC_OUTPUT([Makefile
           include/Makefile
           src/Makefile])
