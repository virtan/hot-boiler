#ifndef H_PTHREAD_KEY_H
#define H_PTHREAD_KEY_H

#include <hot/safe_errno.h>
#include <hot/mutex.h>
#include <map>

namespace hot {

template <class Some>
class pthread_key
{
    public:
        typedef void (*destructor_t)(void*);

    public:
        inline pthread_key(destructor_t _destructor=NULL) : destructor(_destructor) {
            register int err=pthread_key_create(&key,destructor);
            if(err)
                throw sys_exception(safe_errno(err),EXCPLACE);
        }
        inline pthread_key(const pthread_key &r) : destructor(r.destructor) {
            register int err=pthread_key_create(&key,destructor);
            if(err)
                throw sys_exception(safe_errno(err),EXCPLACE);
        }
        inline ~pthread_key() {
            pthread_key_delete(key);
        }
        inline void set_specific(Some &value) {
            register int err=pthread_setspecific(key,(void*) &value);
            if(err)
                throw sys_exception(safe_errno(err),EXCPLACE);
        }
        inline Some &get_specific() {
            return *((Some*) pthread_getspecific(key));
        }

    private:
        pthread_key_t key;
        destructor_t destructor;

};

}

#endif
