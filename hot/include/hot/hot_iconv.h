#ifndef H_ICONV_H
#define H_ICONV_H

#include <hot/config.h>

#ifdef HAVE_ICONV
#include <hot/string.h>
#include <iconv.h>

namespace hot {

class iconv
{
    public:
        typedef enum { ok=0,bad_direction,bad_sequence,unknown } return_type;

    public:
        iconv(const string &from, const string &to, bool recover=true);
        ~iconv();
        void direction(const string &from, const string &to);
        void tune_charset(string &charset);
        void recover(bool value);
        bool recover();
        return_type recode(const string &source, string &dest);
        bool iserror();
        static const string error(int error_code);
        static return_type recode(const string &from, const string &to,
                const string &source, string &dest, bool recover=true);

    private:
        iconv_t ip;
        char *buf;
        bool mode_recover;
};

}

#endif

#endif
