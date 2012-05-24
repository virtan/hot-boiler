#include <hot/timestamp.h>
#include <time.h>
#include <hot/safe_errno.h>
#include <hot/exceptions.h>

using hot::safe_errno;

hot::timestamp::timestamp()
{
    tv_sec=0;
    tv_nsec=0;
}

void hot::timestamp::set_actual() {
    int bad=clock_gettime(CLOCK_REALTIME, this);
    if(bad)
        throw sys_exception(safe_errno(errno));
}

hot::timestamp::operator timespec*()
{
    return this;
}

hot::timestamp::operator const timespec*() const
{
    return this;
}
