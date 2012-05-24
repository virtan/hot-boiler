#ifndef H_MD5_H
#define H_MD5_H

#include <hot/config.h>
#include <hot/string.h>

namespace hot {

#define md5byte unsigned char
#define UWORD32 unsigned int

class md5
{
    public:
        md5();
        void update(const string &str);
        string final(); // in binary : 16 bytes
        string final_hex(); // in hex : 32 bytes
        void clear();
        operator string(); // alias for final_hex

    private:
        void init();
        void transform(UWORD32 *buf, UWORD32 const *in);

    private:
        UWORD32 buf[4];
        UWORD32 bytes[2];
        UWORD32 in[16];
        md5byte digest[16];
        bool finalized;
};

}

#endif
