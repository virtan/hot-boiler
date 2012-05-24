#ifndef H_MAPPER_H
#define H_MAPPER_H

#include <hot/config.h>
#include <hot/string.h>
#include <hot/rwlock.h>

namespace hot {

class mapper {
    public:
        mapper();
        ~mapper();
        bool map(const string &filename, string *er=NULL, off_t offset=0, bool need_writeability=false);
        bool is_mapped();
        void unmap();
        template <typename Return_Cast>
        inline Return_Cast base()
        {
            return (Return_Cast) addr;
        }
        template <typename Return_Cast>
        inline Return_Cast begin()
        {
            return this->base<Return_Cast>();
        }
        inline size_t length_bytes()
        {
            return length;
        }
        template <typename Return_Cast>
        inline Return_Cast end() // the address of byte after base()+length()
        {
            return (Return_Cast) end;
        }
        inline void lock_unmapping() // short time per_thread lock for unmapping, can't be recursive (rwlock,ro)
        {
            unmap_lock.wrlock();
        }
        inline void unlock_unmapping()
        {
            unmap_lock.unlock();
        }

    protected:
        void *addr;
        size_t length;
        void *end_;
        int fd;
        rwlock unmap_lock;
        static const size_t max_size_t;
};

};

#endif
