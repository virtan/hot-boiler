# Perverted check (name,   default_with_list, is_add_include_lib, with_inc_addon, with_lib_addon,
#                  program,              libraries, is_critical, is_not_report)
# Example:        ([liba], [/usr /usr/local], [1],                [liba],         [],
#                  AC_LANG_PROGRAM(...), [liba],    [1],         [1])
AC_DEFUN([AC_PERVERTED_CHECK_LIB],[
    AH_TEMPLATE(HAVE_LIB$1,[$1 usability])
    if test -z "$9" ; then 
        AC_MSG_CHECKING([for $1])
    fi
    AC_ARG_WITH($1,AC_HELP_STRING([--with-$1=path],[specify path to $1 @<:@$2@:>@]),[
        _WITH="$withval"
        _IS_ADD_IL="$3"
        _ARG_SET="1"
    ],[
        _WITH="$2"
        _IS_ADD_IL="1"
        _ARG_SET=""
    ])
    _WITH_INC=""
    _WITH_LIB=""
    _LINK_SHARED=""
    if test "$_IS_ADD_IL" ; then _ADD_I="/include" ; _ADD_L="/lib" ; else _ADD_I="" ; _ADD_L="" ; fi
    _LIST2="$_WITH" ; for j in $_LIST2 ; do
        if test "$4" ; then
            _LIST="$4" ; for i in $_LIST ; do _WITH_INC="$_WITH_INC $j$_ADD_I/$i" ; done
        else
            _WITH_INC="$_WITH_INC $j$_ADD_I"
        fi
        if test "$5" ; then
            _LIST="$5" ; for i in $_LIST ; do _WITH_LIB="$_WITH_LIB $j$_ADD_L/$i" ; done
        else
            _WITH_LIB="$_WITH_LIB $j$_ADD_L"
        fi
    done
    _RESULT_INC=""
    _RESULT_LIB=""
    for i in $_WITH_INC ; do _RESULT_INC="$_RESULT_INC -I$i" ; done
    for i in $_WITH_LIB ; do _RESULT_LIB="$_RESULT_LIB -L$i" ; done
    _LIST="$7" ; for i in $_LIST ; do 
        if test "X$i" = "X-shared" ; then
            _LINK_SHARED="1"
        else
            _RESULT_LIB="$_RESULT_LIB -l$i"
        fi
    done
    _RESULT=""
    _OLD_CXXFLAGS="$CXXFLAGS"
    _OLD_LIBS="$LIBS"
    _OLD_LDFLAGS="$LDFLAGS"
    CXXFLAGS="$CXXFLAGS $_RESULT_INC"
    LIBS="$LIBS $_RESULT_LIB"
    if test "X$_LINK_SHARED" = "X1" ; then
        LDFLAGS="$LDFLAGS -shared"
    fi
    AC_LINK_IFELSE([$6],[_RESULT="1"],[])
    CXXFLAGS="$_OLD_CXXFLAGS"
    LIBS="$_OLD_LIBS"
    LDFLAGS="$_OLD_LDFLAGS"
    if test $_RESULT ; then
        AC_DEFINE([HAVE_LIB$1])
        AC_SUBST(INC_$1,["$_RESULT_INC"])
        AC_SUBST(LIB_$1,["$_RESULT_LIB"])
        if test -z "$9" ; then 
            AC_MSG_RESULT([yes])
        fi
    else
        if test -z "$9" ; then 
            AC_MSG_RESULT([no])
        fi
        if test "$8" ; then
            if test "$_ARG_SET" ; then _PATH=" at location $_WITH" ; else _PATH="" ; fi
            AC_MSG_ERROR([can't find or use $1$_PATH (required)])
        fi
    fi
])

# Iconv second arg type detection
AH_TEMPLATE(HAVE_ICONV,[whether we can compile with iconv or not])
AH_TEMPLATE(HAVE_ICONV_CHAR,[second argument for iconv function is char**])
AH_TEMPLATE(HAVE_ICONV_CONSTCHAR,[second argument for iconv function is const char**])
AC_DEFUN([AC_ICONV_ARG_TYPE],[
    AC_MSG_CHECKING([for iconv])
    _OLD_CFLAGS="$CFLAGS"
    _OLD_CXXFLAGS="$CXXFLAGS"
    CFLAGS="$CFLAGS -pedantic-errors"
    CXXFLAGS="$CXXFLAGS -pedantic-errors"
    _FFLAG=""
    AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([
#include <iconv.h>
        ],[
iconv((iconv_t)(-1),(char**)0,(size_t*)0,(char**)0,(size_t*)0);
        ])
    ],[
        AC_DEFINE([HAVE_ICONV])
        AC_DEFINE([HAVE_ICONV_CHAR])
        AC_MSG_RESULT([yes, 2 arg is char**])
        _FFLAG="1"
    ],[
    ])
    AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([
#include <iconv.h>
        ],[
iconv((iconv_t)(-1),(const char**)0,(size_t*)0,(char**)0,(size_t*)0);
        ])
    ],[
        AC_DEFINE([HAVE_ICONV])
        AC_DEFINE([HAVE_ICONV_CONSTCHAR])
        AC_MSG_RESULT([yes, 2 arg is const char**])
        _FFLAG="1"
    ],[
    ])
    CFLAGS="$_OLD_CFLAGS"
    CXXFLAGS="$_OLD_CXXFLAGS"
    if test ! "$_FFLAG" ; then
        AC_MSG_RESULT([no])
    fi
])

# Config hooker
AC_DEFUN([AC_CONFIG_HOOKER],[
    rm -f config.h.in_hooked
    cat config.h.in | grep -v "^#undef PACKAGE" | grep -v "^#undef VERSION$" > config.h.in_hooked
    mv -f config.h.in_hooked config.h.in
])

