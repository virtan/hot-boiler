#ifndef H_SINGLETON_H
#define H_SINGLETON_H

#include <hot/config.h>
#include <hot/mutex.h>
#include <hot/guard.h>

namespace hot {

template<typename Var>
class singleton
{
    public:
        static Var &instance() {
            if(var==NULL) {
                guard<mutex> only_one(mt_safeness);
                if(var==NULL)
                    create_instance();
            }
            return *var;
        }

        static void destroy() {
            if(var!=NULL) {
                guard<mutex> only_one(mt_safeness);
                delete var;
                var=NULL;
            }
        }

    private:
        static void create_instance() {
            var=new Var;
        }
        singleton() {
        }
        ~singleton() {
        }

    private:
        static Var *var;
        static mutex mt_safeness;
};

template <typename Var>
mutex singleton<Var>::mt_safeness;

template<typename Var>
Var *singleton<Var>::var=NULL;

}

#endif
