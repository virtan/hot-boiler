#ifndef B_LOG_H
#define B_LOG_H

#include <boiler/config.h>
#include <boiler/string.h>
#include <boiler/bconfig.h>
#include <hot/rwlock.h>
#include <ostream>
#include <sstream>
#include <map>

namespace boiler {

using hot::rwlock;

class log_impl;

class log_t
{
    public:
        typedef std::ostringstream log_stream_type;
        // typedef std::ostream log_stream_output;
        // typedef std::map<string, log_stream_output *> log_map_type;

    public:
        log_t();
        ~log_t();

        // init must be called in one-thread mode before any instantiation
        void init(const config_t &cf, bool _debug);

        // debug flag
        inline bool debug() const
        {
            return debug_;
        }

        // debug: anything not needed in production
        inline void debug(const log_stream_type &s)
        {
            this->output("debug",s.str(),false);
        }
        // warning: something wrong, but execution will run as planned
        inline void warning(const log_stream_type &s)
        {
            this->output("warning",s.str(),false);
        }
        // error: some of functional execution will be not executed
        inline void error(const log_stream_type &s)
        {
            this->output("error",s.str(),false);
        }
        // critical: can't continue, exiting
        inline void critical(const log_stream_type &s)
        {
            this->output("critical",s.str(),false);
        }

        // generic log stream
        inline void stream(const string &type, const log_stream_type &s, bool no_date=false)
        {
            this->output(type,s.str(), no_date);
        }

        void free_logs();

    private:
        void output(const string &type, const string &data, bool no_date);
        // return "YYYY-MM-DD HH:MM:SS "
        string current_time();

    private:
        //log_map_type log_map;
        log_impl *l;
        //mutex log_write_lock;
        //mutex init_lock;
        rwlock init_lock;
        bool debug_;
};

// here is no better way come in my mind

#define log_generic(dir, data) { \
    boiler::log_t::log_stream_type log_stream; \
    log_stream << data; \
    boiler::log_t &log_=boiler::globals().log(); \
    if(log_.debug()) \
        log_stream << " ( " << __PRETTY_FUNCTION__ << " )"; \
    log_.dir(log_stream); \
}

#define log_stream(type, data) { \
    boiler::log_t::log_stream_type log_stream; \
    log_stream << data; \
    boiler::log_t &log_=boiler::globals().log(); \
    if(log_.debug()) \
        log_stream << " ( " << __PRETTY_FUNCTION__ << " )"; \
    log_.stream(type,log_stream); \
}

#define log_stream_e(type, data, no_date) { \
    boiler::log_t::log_stream_type log_stream; \
    log_stream << data; \
    boiler::log_t &log_=boiler::globals().log(); \
    if(log_.debug()) \
        log_stream << " ( " << __PRETTY_FUNCTION__ << " )"; \
    log_.stream(type,log_stream,no_date); \
}

#define log_debug(data) log_generic(debug, data)
#define log_warning(data) log_generic(warning, data)
#define log_error(data) log_generic(error, data)
#define log_critical(data) log_generic(critical, data)


}

#endif
