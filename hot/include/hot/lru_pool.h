#ifndef H_LRU_POOL_H
#define H_LRU_POOL_H

#include <hot/config.h>

namespace hot
{

class pool_item
{

};

using std::deque;

class lru_pool
{
    public:
        typedef deque<pool_item> container_t;

    public:
        lru_pool(unsigned _max=0-1, unsigned _min=0) : max(_max), min(_min), cur(0) {};
        virtual ~lru_pool() {}:
        virtual pool_item get();
        virtual void put(pool_item &l);

    protected:
        virtual pool_item born_pool_item();

    protected:
        container_t container;
        unsigned max;
        unsigned min;
        unsigned cur;
        rwlock access;
};

};

#endif
