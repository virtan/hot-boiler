#ifndef H_TIMESTAMP_H
#define H_TIMESTAMP_H

#include <hot/config.h>
#include <sys/time.h>

namespace hot {

class timestamp : public timespec
{
    public:
        timestamp();
        void set_actual();
        operator timespec*();
        operator const timespec*() const;
};

}

#endif
