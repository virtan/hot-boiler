#ifndef H_PTHREAD_ONCE_H
#define H_PTHREAD_ONCE_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>

namespace hot {

// any instance must be declared as static
class pthread_once
{
    public:
        typedef void (*routine)(void);

    public:
        pthread_once();
        pthread_once(const pthread_once&);
        pthread_once(routine r);

        void run(routine r);

    private:
        pthread_once_t ctrl;
        static pthread_once_t ctrl_tmpl;
};

}

#endif
