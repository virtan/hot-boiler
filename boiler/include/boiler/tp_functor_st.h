#ifndef BTP_FUNCTOR_ST_H
#define BTP_FUNCTOR_ST_H

#include <iostream>
#include <boiler/config.h>
#include <boiler/tp_functor.h>
#include <hot/singleton.h>
#include <hot/bpthread_key.h>
#include <boiler/reactor.h>
#include <boiler/thread_pool.h>
#include <boiler/globals.h>
#include <boiler/log.h>
#include <boiler/stack.h>
#include <boiler/stack_pool.h>
#include <setjmp.h>

namespace boiler {

#define execution_stack_of_task (boiler::singleton<boiler::pthread_key<boiler::stack_t*> >::instance())

#ifdef __FreeBSD__
// got from /usr/src/lib/libc_r/uthread/pthread_private.h
#define JMP_SET_STACK(state,ptr) (state[0]._jb[2] = (int)(ptr))
#else
#ifdef __i386__
#ifndef JB_SP
#define JB_SP 4
#endif
#define JMP_SET_STACK(state,ptr) (state[0].__jmpbuf[JB_SP] = (int)(ptr))
#else
#ifdef __ppc__
#define JMP_JB_STACK 0
#define JMP_SET_STACK(state,ptr) (state[JMP_JB_STACK] = (int)(ptr))
#endif
#endif
#endif

class state_threads : public functor
{

    public:
        static void switch_thread_pool(thread_pool *to) {
#ifdef THREAD_PER_CLIENT_MODE
            return;
#endif
            if(!to)
                return;
            stack_t *execution_stack=execution_stack_of_task.get_specific();
            execution_stack->switch_tp()=to;
            execution_stack->ed().tp=to;
            if(!_setjmp(execution_stack->suspend_jmp())) {
                _longjmp(execution_stack->control_jmp(),2);
            }
        }

    protected:
        state_threads(const string &_reactor, char *) : reactor(_reactor), execution_stack(0) {}
        virtual ~state_threads() {
            if(execution_stack) {
                execution_stack->deinit();
                globals().stack_pool().put(execution_stack);
                execution_stack=0;
            }
        }

        virtual bool operator() () {
#ifdef THREAD_PER_CLIENT_MODE
            this->real_operator_brackets();
            return true;
#endif
            // execution_stack.init() == first run
            bool continue_mode=execution_stack;
            if(!continue_mode) {
                execution_stack=globals().stack_pool().get();
                execution_stack->init();
            }
            int return_point=_setjmp(execution_stack->control_jmp());
            if(return_point==0) {
                // set data
                execution_stack_of_task.set_specific(execution_stack);
                execution_stack->ed().f=this;
                execution_stack->ed().tp=current_thread_pool.get_specific();

                // continue
                if(continue_mode) {
                    _longjmp(execution_stack->suspend_jmp(),1);
                }

                // start new
                jmp_buf short_jmp_context;
                if(!_setjmp(short_jmp_context)) {
                    JMP_SET_STACK(short_jmp_context,execution_stack->stack());
                    _longjmp(short_jmp_context,1);
                }
                {
                    // since this point we out of scope of local variables (including this ???)
                    stack_t *real_es=execution_stack_of_task.get_specific();
                    try {
                        dynamic_cast<state_threads*>(real_es->ed().f)->real_operator_brackets();
                    } catch(std_exception &e) {
                        log_error("uncaughted exception of type " << e.type() << ": " << e << " at " << e.function());
                    } catch(...) {
                        // TODO ???
                        log_error("unwaited exception in tp_functor_st");
                    }
                    {
                        stack_t *real_es_2=execution_stack_of_task.get_specific();
                        _longjmp(real_es_2->control_jmp(),255);
                    }
                }
            } else if(return_point==1) {
                // breaking for event
                event_descriptor &ed=execution_stack->ed();
                globals().reactors()[reactor]->register_event(ed);
                return false;
            } else if(return_point==2) {
                // moving to other thread pool
                if(execution_stack->switch_tp())
                    execution_stack->switch_tp()->putq(this);
                else
                    current_thread_pool.get_specific()->putq(this);
                return false;
            }
            // exiting
            return true;
        }
        virtual void real_operator_brackets()=0;

    protected:
        const string reactor;

    private:
        stack_t *execution_stack;
};


class void_type {};

template <class Class, typename Arg1=void_type, typename Arg2=void_type, typename Arg3=void_type, typename Arg4=void_type, typename Arg5=void_type, typename Arg6=void_type, typename Arg7=void_type, typename Arg8=void_type, typename Arg9=void_type>
class functor_adapter_st : public state_threads
{
    public:
        typedef void (Class::*callback_type_0)();
        typedef void (Class::*callback_type_1)(Arg1);
        typedef void (Class::*callback_type_2)(Arg1,Arg2);
        typedef void (Class::*callback_type_3)(Arg1,Arg2,Arg3);
        typedef void (Class::*callback_type_4)(Arg1,Arg2,Arg3,Arg4);
        typedef void (Class::*callback_type_5)(Arg1,Arg2,Arg3,Arg4,Arg5);
        typedef void (Class::*callback_type_6)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6);
        typedef void (Class::*callback_type_7)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7);
        typedef void (Class::*callback_type_8)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8);
        typedef void (Class::*callback_type_9)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9);

        typedef union { callback_type_0 ct0; callback_type_1 ct1; callback_type_2 ct2; callback_type_3 ct3; callback_type_4 ct4; callback_type_5 ct5; callback_type_6 ct6; callback_type_7 ct7; callback_type_8 ct8; callback_type_9 ct9; } callback_type;

    public:
        functor_adapter_st(const string &reactor, callback_type_0 _mf, Class &_cl, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(0), cl(_cl) { mf.ct0=_mf; }
        functor_adapter_st(const string &reactor, callback_type_1 _mf, Class &_cl, Arg1 _arg1, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(1), cl(_cl), arg1(_arg1) { mf.ct1=_mf; }
        functor_adapter_st(const string &reactor, callback_type_2 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(2), cl(_cl), arg1(_arg1), arg2(_arg2) { mf.ct2=_mf; }
        functor_adapter_st(const string &reactor, callback_type_3 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(3), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3) { mf.ct3=_mf; }
        functor_adapter_st(const string &reactor, callback_type_4 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(4), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) { mf.ct4=_mf; }
        functor_adapter_st(const string &reactor, callback_type_5 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, Arg5 _arg5, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(5), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5) { mf.ct5=_mf; }
        functor_adapter_st(const string &reactor, callback_type_6 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, Arg5 _arg5, Arg6 _arg6, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(6), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6) { mf.ct6=_mf; }
        functor_adapter_st(const string &reactor, callback_type_7 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, Arg5 _arg5, Arg6 _arg6, Arg7 _arg7, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(7), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7) { mf.ct7=_mf; }
        functor_adapter_st(const string &reactor, callback_type_8 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, Arg5 _arg5, Arg6 _arg6, Arg7 _arg7, Arg8 _arg8, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(8), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8) { mf.ct8=_mf; }
        functor_adapter_st(const string &reactor, callback_type_9 _mf, Class &_cl, Arg1 _arg1, Arg2 _arg2, Arg3 _arg3, Arg4 _arg4, Arg5 _arg5, Arg6 _arg6, Arg7 _arg7, Arg8 _arg8, Arg9 _arg9, char *_exec_stack=NULL) : state_threads(reactor, _exec_stack), arg_number(9), cl(_cl), arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9) { mf.ct9=_mf; }
        virtual ~functor_adapter_st() {};

        virtual void real_operator_brackets() {
            switch(arg_number) {
                case 0:
                    (cl.*mf.ct0)();
                    break;
                case 1:
                    (cl.*mf.ct1)(arg1);
                    break;
                case 2:
                    (cl.*mf.ct2)(arg1,arg2);
                    break;
                case 3:
                    (cl.*mf.ct3)(arg1,arg2,arg3);
                    break;
                case 4:
                    (cl.*mf.ct4)(arg1,arg2,arg3,arg4);
                    break;
                case 5:
                    (cl.*mf.ct5)(arg1,arg2,arg3,arg4,arg5);
                    break;
                case 6:
                    (cl.*mf.ct6)(arg1,arg2,arg3,arg4,arg5,arg6);
                    break;
                case 7:
                    (cl.*mf.ct7)(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
                    break;
                case 8:
                    (cl.*mf.ct8)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
                    break;
                case 9:
                    (cl.*mf.ct9)(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
                    break;
                default:
                    throw badcode_exception("invalid number of arguments in functor_adapter_st");
            }
        }

    private:
        int arg_number;
        Class &cl;
        Arg1 arg1;
        Arg2 arg2;
        Arg3 arg3;
        Arg4 arg4;
        Arg5 arg5;
        Arg6 arg6;
        Arg7 arg7;
        Arg8 arg8;
        Arg9 arg9;
        callback_type mf;
};

// fsa = functor simple adapter
template <typename pClass>
functor *fsa_st(const string &reactor, void (pClass::*method)(), pClass *pc)
{
    return new functor_adapter_st<pClass>(reactor,method,*pc);
}

template <typename pClass, typename Arg1>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1), pClass *pc, Arg1 a1)
{
    return new functor_adapter_st<pClass,Arg1>(reactor,method,*pc,a1);
}

template <typename pClass, typename Arg1, typename Arg2>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2), pClass *pc, Arg1 a1, Arg2 a2)
{
    return new functor_adapter_st<pClass,Arg1,Arg2>(reactor,method,*pc,a1,a2);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3>(reactor,method,*pc,a1,a2,a3);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4>(reactor,method,*pc,a1,a2,a3,a4);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4,Arg5), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4,Arg5>(reactor,method,*pc,a1,a2,a3,a4,a5);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>(reactor,method,*pc,a1,a2,a3,a4,a5,a6);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>(reactor,method,*pc,a1,a2,a3,a4,a5,a6,a7);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>(reactor,method,*pc,a1,a2,a3,a4,a5,a6,a7,a8);
}

template <typename pClass, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
functor *fsa_st(const string &reactor, void (pClass::*method)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9), pClass *pc, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8, Arg9 a9)
{
    return new functor_adapter_st<pClass,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>(reactor,method,*pc,a1,a2,a3,a4,a5,a6,a7,a8,a9);
}

/*
template <typename Class, typename Arg>
const int functor_adapter<Class,Arg>::stack_size=2*1024*1024;
*/

}

#endif
