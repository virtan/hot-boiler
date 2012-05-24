#ifndef H_OPTIONS_H
#define H_OPTIONS_H

#include <hot/config.h>
#include <hot/string.h>
#include <hot/lexical_cast.h>
#include <hot/exceptions.h>
#include <map>

using std::map;
using std::multimap;
using std::pair;

namespace hot {

class options
{
    public:
        typedef multimap<string,string> args_type;
        typedef pair<string,string> args_value_type;
        typedef map<string,string> descs_type;

        options();
        ~options();
        
        void load(int argc, char **argv);

        void define(const string &name, const string &desc, const string &def=string("--"));

        template <typename T>
        T arg(const string &name, unsigned int pos=0);
        template <typename T>
        T arg(unsigned int pos=0);
        unsigned int arg_count(const string &name=string(""));
        const string &program();

        void help();

    private:
        string spaces(int count, int minus=0);
        string indent(int left, const string &source, int length=0, bool first_indent=false);

    private:
        args_type args;
        descs_type descs;
        args_type defs;
        string progname;
};

template <typename T>
inline T options::arg(const string &name, unsigned int pos)
{
    args_type::iterator target=args.lower_bound(name);
    while(pos--) target++;
    try {
        if(target!=args.end() && target->first==name)
            return hot::lexical_cast<T>(target->second);
        return hot::lexical_cast<T>("");
    } catch(string &lxcstexc) {
        return T();
    }
}

template <>
inline bool options::arg(const string &name, unsigned int pos)
{
    args_type::iterator target=args.lower_bound(name);
    while(pos--) target++;
    if(target!=args.end() && target->first==name)
        return true;
    return false;
}

template <typename T>
T options::arg(unsigned int pos)
{
    return arg<T>("",pos);
}

}

#endif
