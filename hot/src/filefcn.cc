#include <hot/filefcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool hot::filefcn::exists_file(const string &path)
{
    int err=access(path.c_str(),R_OK);
    if(err)
        // TODO !
        return false;
    return true;
}

bool hot::filefcn::delete_file(const string &path)
{
    int err=unlink(path.c_str());
    if(err)
        // TODO !
        return false;
    return true;
}

bool hot::filefcn::flag_file(const string &path)
{
    int err=open(path.c_str(),O_CREAT|O_EXCL,0644);
    if(err<0)
        return false;
    close(err);
    return true;
}

bool hot::filefcn::rename_file(const string &from, const string &to)
{
    int err=rename(from.c_str(),to.c_str());
    if(err)
        return false;
    return true;
}

bool hot::filefcn::is_dir(const string &path)
{
    struct stat buf;
    int err=stat(path.c_str(),&buf);
    if(err)
        return false;
    return S_ISDIR(buf.st_mode);
}


long long int hot::filefcn::file_size(const string &path)
{
    struct stat buf;
    int err=stat(path.c_str(),&buf);
    if(err)
        return -1;
    return (long long int) buf.st_size;
}


hot::directory::directory(const string &path)
{
    d=opendir(path.c_str());
    if(!d)
        throw std::ios_base::failure("can't open dir "+path);
}

hot::directory::~directory()
{
    if(d)
        closedir(d);
}

bool hot::directory::next(string &name)
{
    struct dirent *de=readdir(d);
    if(!de)
        return false;
    name=de->d_name;
    return true;
}

