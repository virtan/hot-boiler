#ifndef H_PTHREAD_ATTR_H
#define H_PTHREAD_ATTR_H

#include <hot/config.h>
#include <pthread.h>
#include <hot/exceptions.h>

namespace hot {

class pthread_attr
{
    public:
        pthread_attr() throw(sys_exception);
        pthread_attr(const pthread_attr&) throw(sys_exception);
        ~pthread_attr() throw(badcode_exception);
        operator pthread_attr_t*();
        operator pthread_attr_t&();

    private:
        pthread_attr_t thr_attr;
};

}

#endif
