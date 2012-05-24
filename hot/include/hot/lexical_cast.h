#ifndef H_LEXICAL_CAST_H
#define H_LEXICAL_CAST_H

#include <hot/config.h>
#include <hot/exceptions.h>
#include <sstream>

namespace hot {

template<typename Target, typename Source>
Target lexical_cast(Source arg, const char *w=0)
{
    std::stringstream wrk;
    wrk << arg;
    if(wrk.fail())
        throw lexcast_exception("bad incoming lexical cast argument",w);
    Target result;
    wrk >> result;
    if(wrk.fail())
        throw lexcast_exception("bad outgoing lexical cast argument",w);
    return result;
};

}

#endif
