#ifndef BTP_FUNCTOR_H
#define BTP_FUNCTOR_H

#include <boiler/config.h>

namespace boiler {

class functor
{
    public:
        functor() {};
        // returns whether delete functor or not // true==delete
        virtual bool operator() () =0;
        virtual ~functor() {};
};

}

#endif
