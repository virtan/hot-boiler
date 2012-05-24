#include <boiler/stack_pool.h>

using hot::guard;

const int boiler::stack_pool_t::review_delta=300;
const int boiler::stack_pool_t::too_inactive_delta=900;

boiler::stack_pool_t::stack_pool_t() : last_review_time(time(NULL))
{
}

boiler::stack_pool_t::~stack_pool_t()
{
    guard<mutex> stack_pool_guard(stack_pool_mutex);
    for(stack_map_t::iterator i=stack_map.begin();i!=stack_map.end();i++)
        delete i->second;
    stack_map.clear();
}

boiler::stack_t *boiler::stack_pool_t::get()
{
    while(1) {
        guard<mutex> stack_pool_guard(stack_pool_mutex);
        if(stack_map.empty())
            break;
        stack_map_t::iterator smi=stack_map.end();
        --smi;
        stack_t *s=smi->second;
        stack_map.erase(smi);
        return s;
    }
    return new stack_t;
}

void boiler::stack_pool_t::put(stack_t *s)
{
    guard<mutex> stack_pool_guard(stack_pool_mutex);
    time_t now=time(NULL);
    stack_map.insert(stack_map.end(),stack_map_t::value_type(now,s));
    if(now > last_review_time+review_delta)
        this->review(now);
}

void boiler::stack_pool_t::review(time_t now)
{
    stack_map_t::iterator i;
    while(now > (i=stack_map.begin())->first+too_inactive_delta) {
        delete i->second;
        stack_map.erase(i);
    }
    last_review_time=now;
}
