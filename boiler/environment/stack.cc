#include <boiler/stack.h>
#include <boiler/globals.h>

boiler::stack_t::stack_t() : switch_tp_(0), debug(globals().args().arg<bool>("debug"))
{
    ptr_ = new uint8_t[stack_size];
    stack_=ptr_+stack_size-grace_size-ad_hoc;
}

void boiler::stack_t::debug_mark_field(uint8_t *p)
{
    for(int i=grace_size;i;i--,p++)
        *p=debug_symbol; 
}

void boiler::stack_t::debug_check_field(uint8_t *p)
{
    uint8_t *f=0;
    for(int i=grace_size;i;i--,p++)
        if(*p!=debug_symbol)
            f=p;
    if(f) {
        log_debug(__PRETTY_FUNCTION__ << ": execution run "
                  << ((p-grace_size)==(stack_+ad_hoc)?"under":"out")
                  << " of stack " << (void*) stack_
                  << " for " << (f+grace_size-p) << " bytes");
    }
}


