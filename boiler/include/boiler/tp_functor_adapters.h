#ifndef BTP_FUNCTOR_ADAPTERS_H
#define BTP_FUNCTOR_ADAPTERS_H

#include <boiler/config.h>
#include <boiler/tp_functor.h>

namespace boiler {

/*
template <typename Class>
class functor_adapter : public functor
{
    public:
        functor_adapter(int (Class::*_mf)(), Class &_cl) : mf(_mf), cl(_cl) {}
        virtual void operator() () {
            (cl.*mf)();
        }
    private:
        int (Class::*mf)();
        Class &cl;
};
*/

template <typename Class, typename Arg=void>
class functor_adapter : public functor
{
    public:
        functor_adapter(int (Class::*_mf)(Arg), Class &_cl, Arg &_arg) : mf(_mf), cl(_cl), arg(_arg) {}
        virtual void operator() () {
            (cl.*mf)(arg);
        }
    private:
        int (Class::*mf)(Arg);
        Class &cl;
        Arg &arg;
};

}

#endif
