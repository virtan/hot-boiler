#ifndef B_STACK_H
#define B_STACK_H

#include <boiler/config.h>
#include <boiler/log.h>
#include <boiler/reactor.h>
#include <inttypes.h>
#include <setjmp.h>

namespace boiler {

class stack_t
{
    public:
        struct jmp_buf_adapter { jmp_buf jb; };

    private:
        static const int stack_size=512*1024;
        static const int grace_size=64;
        static const int ad_hoc=32;
        static const char debug_symbol='d';

    public:
        stack_t();

        inline ~stack_t() {
            delete[] ptr_;
        }

        inline void init() {
            if(debug) {
                debug_mark_field(stack_+ad_hoc);
                debug_mark_field(ptr_);
            }
        }

        inline void deinit() {
            if(debug) {
                debug_check_field(stack_+ad_hoc);
                debug_check_field(ptr_);
            }
        }

        inline uint8_t *stack() {
            return stack_;
        }

        inline jmp_buf &control_jmp() {
            return control_jmp_.jb;
        }

        inline jmp_buf &suspend_jmp() {
            return suspend_jmp_.jb;
        }

        inline event_descriptor &ed() {
            return ed_;
        }

        inline thread_pool *&switch_tp() {
            return switch_tp_;
        }

        void debug_mark_field(uint8_t *p);
        void debug_check_field(uint8_t *p);

    private:
        uint8_t *ptr_;
        uint8_t *stack_;
        jmp_buf_adapter control_jmp_;
        jmp_buf_adapter suspend_jmp_;
        event_descriptor ed_;
        thread_pool *switch_tp_;
        bool debug;
};

}

#endif
