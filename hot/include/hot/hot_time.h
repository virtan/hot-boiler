#ifndef H_HOT_TIME_H
#define H_HOT_TIME_H

#include <hot/config.h>
#include <hot/string.h>
#include <time.h>

namespace hot
{

class hot_time
{
    public:
        hot_time() : t(time(NULL)) {};
        hot_time(time_t _t) : t(_t) {};

        string format(const string &f, bool globaltime=false);

    private:
        time_t t;
};

}

#endif
