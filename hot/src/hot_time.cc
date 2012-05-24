#include <hot/hot_time.h>

hot::string hot::hot_time::format(const string &f, bool globaltime)
{
    struct tm tm_time;
    {
        struct tm *res;
        if(globaltime)
            res=gmtime_r(&t,&tm_time);
        else
            res=localtime_r(&t,&tm_time);
        if(!res)
            return string();
    }
    unsigned int size=256;
    string result;
    while(1) {
        char *buf=new char[size];
        size_t res=strftime(buf,size,f.c_str(),&tm_time);
        if(res>=size-1) {
            size*=2;
            delete buf;
            continue;
        }
        result=buf;
        delete buf;
        break;
    }
    return result;
}
