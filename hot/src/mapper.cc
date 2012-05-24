#include <hot/mapper.h>
#include <hot/safe_errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <hot/guard.h>

namespace hot {

const size_t mapper::max_size_t(((size_t) -1)/2);

mapper::mapper() : addr(NULL), length(0), end_(NULL), fd(0)
{
}

mapper::~mapper()
{
    unmap();
}

bool mapper::is_mapped()
{
    return (bool) addr;
}

bool mapper::map(const string &filename, string *er, off_t offset, bool need_writeability)
{
    unmap();
    struct stat sb;
    int err=stat(filename.c_str(),&sb);
    if(err) {
        if(er) {
            *er=filename;
            *er+=": ";
            *er+=safe_errno(errno);
        }
        return false;
    }
    if(!(sb.st_mode&S_IFREG) && !(sb.st_mode&S_IFLNK)) {
        if(er) {
            *er=filename;
            *er+=": can't map anything except regular or symlinked file";
        }
        return false;
    }
    if(sb.st_size>(off_t) max_size_t)
        throw sys_exception("too big file to map",EXCPLACE);
    length=sb.st_size;
    fd=open(filename.c_str(),need_writeability?O_RDWR:O_RDONLY);
    if(fd==-1) {
        if(er) {
            *er=filename;
            *er+=": no ";
            if(need_writeability) *er+="read/write"; else *er+="read";
            *er+=" access";
        }
        return false;
    }
    addr=mmap(NULL,length,need_writeability?PROT_READ|PROT_WRITE:PROT_READ,MAP_SHARED,fd,offset);
    if(addr==(void*)-1) {
        addr=NULL;
        close(fd);
        if(er) {
            *er=filename;
            *er+=": ";
            *er+=safe_errno(errno);
        }
        return false;
    }
    end_=(void*)((char*)addr+length);
    return true;
}

void mapper::unmap()
{
    if(!is_mapped())
        return;
    guard<rwlock,rw> unmap_lock_guard(unmap_lock);
    munmap(addr,length);
    close(fd);
    addr=NULL;
}

}
