#ifndef H_SAFE_ERRNO_H
#define H_SAFE_ERRNO_H

#include <hot/config.h>
#include <hot/string.h>
#include <errno.h>
#include <string.h>
#include <iostream>

namespace hot
{

inline string safe_errno(int ern)
{
#ifdef HAVE_STRERROR_R
    char buf[128];
#ifdef STRERROR_R_CHAR_P
    return strerror_r(ern,buf,128);
#else
    strerror_r(ern,buf,128);
    buf[127]=0;
    return buf;
#endif
#else
    return strerror(ern);
#endif
}

}

#endif
