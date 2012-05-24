#include <hot/options.h>
#include <iostream>

hot::options::options()
{
}

hot::options::~options()
{
}

void hot::options::load(int argc, char **argv)
{
    progname=argv[0];
    bool mm_met=false;
    for(int i=1;i<argc;i++) {
        if(!mm_met)
            if(argv[i][0]=='-' && argv[i][1]=='-') {
                if(argv[i][2]==0) {
                    mm_met=true;
                    continue;
                }
                int j;
                for(j=2;argv[i][j] && argv[i][j]!='=';j++);
                if(j>2) {
                    args.insert(args_value_type(string(argv[i]+2,j-2),(argv[i][j]?argv[i]+j+1:"")));
                    continue;
                }
            }
        args.insert(args_value_type("",argv[i]));
    }

    if(args.find("help")!=args.end())
        help();

    for(args_type::iterator i=defs.begin();i!=defs.end();i=defs.upper_bound(i->first))
        if(args.find(i->first)==args.end())
            args.insert(i,defs.upper_bound(i->first));
}

void hot::options::define(const string &name, const string &desc, const string &def)
{
    if(name=="help")
        return;
    descs[name]=desc;
    if(def!="--")
        defs.insert(args_value_type(name,def));
}

unsigned int hot::options::arg_count(const string &name)
{
    return args.count(name);
}

void hot::options::help()
{
    std::cout << std::endl;
    std::cout << "Using: " << progname << " [--key[=value]] ... [arg] ..." << std::endl;
    std::cout << std::endl;
    std::cout << "Keys:" << std::endl;
    for(descs_type::iterator i=descs.begin();i!=descs.end();i++)
        std::cout << "  " << i->first << spaces(15,i->first.size()) << " "
            << indent(18,i->second,78) << std::endl;
    std::cout << std::endl;
    exit(0);
}

hot::string hot::options::spaces(int count, int minus)
{
    return string((count-minus<0?0:count-minus),' ');
}

hot::string hot::options::indent(int left, const string &source, int length, bool first_indent)
{
    string out;
    if(length>0)
        length-=left;
    for(int s=0;s<(int) source.size();) {
        int t=source.size();
        if(length>0 && t-s>length) {
            t=s+length;
            string::size_type t_saved=t;
            while(t>s)
                if(isspace(source[t]))
                    break;
                else
                    t--;
            if(t==s)
                t=t_saved;
        }
        if(s || first_indent)
            out+=spaces(left);
        out+=source.substr(s,t-s);
        out+='\n';
        s=t;
        while(isspace(source[s]))
            s++;
    }
    return out;
}

const hot::string &hot::options::program()
{
    return progname;
}
