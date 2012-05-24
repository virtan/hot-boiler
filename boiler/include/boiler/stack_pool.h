#ifndef B_STACK_POOL_H
#define B_STACK_POOL_H

#include <boiler/config.h>
#include <boiler/stack.h>
#include <boiler/log.h>
#include <hot/options.h>
#include <map>
#include <time.h>

namespace boiler {

using hot::options;

class stack_pool_t
{
    public:
        typedef std::multimap<time_t,stack_t*> stack_map_t;

    public:
        stack_pool_t();
        ~stack_pool_t();

        stack_t *get();
        void put(stack_t *s);

    private:
        void review(time_t now);

    private:
        stack_map_t stack_map;
        mutex stack_pool_mutex;
        time_t last_review_time;
        static const int review_delta; // in seconds
        static const int too_inactive_delta; // in seconds
};

}

#endif
