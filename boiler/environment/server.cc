#include <boiler/server.h>
#include <dlfcn.h>

boiler::server *boiler::server::run(const config_t &cf)
{
    void *lib=dlopen(cf/"lib",RTLD_NOW|RTLD_GLOBAL);
    if(!lib)
        throw invarg_exception(string("can't open ")+(const char*)(cf/"lib")+" library for server "+cf.name()+": "+dlerror());
    void *func=dlsym(lib,_BOILER_SERVER_DECLARATION);
    if(!func)
        throw invarg_exception(string("can't find function ")+_BOILER_SERVER_DECLARATION+" in "+(const char*)(cf/"lib")+" library for server "+cf.name());
    return (*((server* (*)(const config_t &cf)) (func)))(cf);
}
