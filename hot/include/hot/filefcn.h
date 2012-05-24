#ifndef H_FILEFCN_H
#define H_FILEFCN_H

#include <hot/config.h>
#include <hot/string.h>
#include <sys/types.h>
#include <dirent.h>

namespace hot {

class filefcn
{
    public:
        bool exists_file(const string &path);
        bool delete_file(const string &path);
        bool rename_file(const string &from, const string &to);
        bool flag_file(const string &path);
        bool is_dir(const string &path);
        long long int file_size(const string &path);
};

class directory
{
    public:
        directory(const string &path);
        ~directory();

        bool next(string &name);
        
    private:
        DIR *d;
};

}

#endif
