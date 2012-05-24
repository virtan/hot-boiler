#include <hot/string.h>

size_t hot::hash_hot_string(const string &s)
{
    unsigned long __h = 0;
    string::size_type __pos;
    for (__pos=0;__pos<s.size();++__pos)
        __h = 5 * __h + s[__pos];
    return size_t(__h);   
}

