#ifndef H_STRING_H
#define H_STRING_H

#include <hot/config.h>
#include <string>
#include <ext/hash_map>

namespace hot {

typedef std::string string;

size_t hash_hot_string(const string &s);

}

namespace __gnu_cxx {

template<>
struct hash<hot::string>
{
    size_t operator()(const hot::string &s) const
    {
        return hot::hash_hot_string(s);
    }
};

}

#endif
