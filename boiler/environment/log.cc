#include <boiler/log.h>
#include <hot/mutex.h>
#include <hot/rwlock.h>
#include <hot/guard.h>
#include <iostream>
#include <fstream>
#include <list>
#include <time.h>

namespace boiler {

using hot::mutex;
using hot::rwlock;
using hot::guard;
using hot::ro;
using hot::rw;

class log_impl
{
    public:
        struct the_log_t {
            the_log_t() {}
            the_log_t(std::ostream &s) : stream(&s) {}
            // the_log_t(const the_log_t &r) : stream(r.stream), filename(r.filename) {} // don't needed, since mutex isn't working
            std::ostream *stream;
            string filename;
            // mutex access_lock; // isn't working on freebsd
        };
        typedef std::map<string, the_log_t> log_map_t;

    public:
        log_impl(const config_t &cf, /*log_t::log_map_type &_log_map,*/ bool debug=false); // already under exclusive lock
        ~log_impl(); // already_under exclusive lock
        void output(const string &type, const string &data); // already under non-exclusive lock
        void free_logs(); // already under exclusive lock

    private:
        log_map_t log_map;
        mutex log_write_lock; // since the_log_t's mutex isn't working on freebsd
};

}

boiler::log_impl::log_impl(const config_t &cf, /*log_t::log_map_type &_log_map,*/ bool debug) /* : log_map(_log_map)*/
{
    log_map.clear();
    log_map["warning"]=the_log_t(std::cerr);
    log_map["error"]=the_log_t(std::cerr);
    log_map["critical"]=the_log_t(std::cerr);
    if(debug)
        log_map["debug"]=the_log_t(std::cerr);
    for(config_t l=*cf;l;++l) {
        the_log_t the_log;
        the_log.filename=(const char*) l;
        if(the_log.filename.empty())
            throw invarg_exception(string("each specified log stream must have valid filename, but ")+l.name()+" has no",EXCPLACE);
        try {
            the_log.stream=new std::ofstream(the_log.filename.c_str(),std::ios_base::app|std::ios_base::ate|std::ios_base::out);
        } catch(std::ofstream::failure &e) {
            throw invarg_exception(string("problem opening/creating file for log stream ")+l.name()+": "+e.what());
        }
        log_map[l.name()]=the_log;
    }
}

boiler::log_impl::~log_impl()
{
    for(log_map_t::iterator i=log_map.begin();i!=log_map.end();i++)
        if(!i->second.filename.empty())
            delete i->second.stream;
    log_map.clear();
}

void boiler::log_impl::free_logs()
{
    for(log_map_t::iterator i=log_map.begin();i!=log_map.end();i++)
        if(!i->second.filename.empty()) {
            the_log_t &the_log=i->second;
            delete the_log.stream;
            the_log.stream=0;
            try {
                the_log.stream=new std::ofstream(the_log.filename.c_str(),std::ios_base::app|std::ios_base::ate|std::ios_base::out);
            } catch(std::ofstream::failure &e) {
                // TODO: ?
                std::cerr << string("problem opening/creating file for log stream ")+i->first+": "+e.what() << std::endl;
                the_log.stream=&std::cerr;
            }
        }
}

void boiler::log_impl::output(const string &type, const string &data)
{
    log_map_t::iterator i=log_map.find(type);
    if(i!=log_map.end()) {
        the_log_t &the_log=i->second;
        guard<mutex> access_lock_guard(log_write_lock);
        the_log.stream->write(data.data(),data.size());
        *(the_log.stream) << std::endl;
    }
}

boiler::log_t::log_t() : l(0), debug_(false)
{
    //log_map["warning"]=&(std::cerr);
    //log_map["error"]=&(std::cerr);
    //log_map["critical"]=&(std::cerr);
}

boiler::log_t::~log_t()
{
    guard<rwlock,rw> init_lock_guard(init_lock);
    delete l;
    l=0;
}

void boiler::log_t::init(const config_t &cf, bool _debug)
{
    guard<rwlock,rw> init_lock_guard(init_lock);
    if(l) {
        delete l;
        l=0;
    }
    debug_=_debug;
    l=new log_impl(cf,debug_);
}

void boiler::log_t::output(const string &type, const string &data, bool no_date)
{
    guard<rwlock,ro> init_lock_guard(init_lock);
    if(!l)
        throw badcode_exception("write to log before initialization",EXCPLACE);
    l->output(type,(no_date?data:this->current_time()+data));
    //log_map_type::iterator i=log_map.find(type);
    //if(i!=log_map.end()) {
        //string date=current_time();
        //guard<mutex> log_write_lock_guard(log_write_lock);
        //i->second->write(date.data(),date.size());
        //i->second->write(data.data(),data.size());
        //(*i->second) << std::endl;
    //}
}

boiler::string boiler::log_t::current_time()
{
    char buf[21];
    time_t time_=time(0);
    struct tm time_tm;
    localtime_r(&time_,&time_tm);
    strftime(buf,21,"%Y-%m-%d %H:%M:%S ",&time_tm);
    return string(buf,20);
}

void boiler::log_t::free_logs()
{
    guard<rwlock,ro> init_lock_guard(init_lock);
    if(!l)
        throw badcode_exception("using of logs before initialization",EXCPLACE);
    l->free_logs();
}
