#include <hot/hot_iconv.h>
#include <errno.h>

#ifdef HAVE_ICONV

hot::iconv::iconv(const string &from, const string &to, bool recover) :
    ip((iconv_t)(-1)),buf(NULL),mode_recover(recover)
{
    direction(from,to);
}

hot::iconv::~iconv() {
    if(!iserror())
        iconv_close(ip);
    delete[] buf;
}

void hot::iconv::direction(const string &from, const string &to) {
    if(!iserror())
        iconv_close(ip);
    ip=iconv_open(to.c_str(),from.c_str());
    if(!iserror()) return;
    string tuned_from(from),tuned_to(to);
    tune_charset(tuned_from);
    tune_charset(tuned_to);
    ip=iconv_open(tuned_to.c_str(),tuned_from.c_str());
}

void hot::iconv::tune_charset(string &charset) {
    if(charset.find("1251")!=string::npos)
        charset="cp1251";
}

void hot::iconv::recover(bool value) {
    mode_recover=value;
}

bool hot::iconv::recover() {
    return mode_recover;
}

hot::iconv::return_type hot::iconv::recode(const string &source, string &dest) {
    if(iserror())
        return bad_direction;
#ifdef HAVE_ICONV_CONSTCHAR
    const char *inbuf=source.c_str();
#else
    char *inbuf=const_cast<char*>(source.c_str());
#endif
    char *outbuf;
    size_t inbl=source.size(),oubl;
    if(!buf)
        buf=new char[4096];
    outbuf=buf;
    oubl=4096;
    while(true) {
        int res=::iconv(ip,&inbuf,&inbl,&outbuf,&oubl);
        if(res>=0) {
            dest.append(buf,4096-oubl);
            if(inbl!=0)
                return unknown;
            return ok;
        } else {
            if(errno==EILSEQ) {
                if(mode_recover) {
                    inbuf++;
                    inbl--;
                    continue;
                } else {
                    dest.append(buf,4096-oubl);
                    return bad_sequence; // ICONV_BADSEQUENCE
                }
            } else if(errno==EINVAL) {
                dest.append(buf,4096-oubl);
                return bad_sequence; // ICONV_BADSEQUENCE or ICONV_NEEDMORE
            } else if(errno==E2BIG) {
                dest.append(buf,4096-oubl);
                outbuf=buf;
                oubl=4096;
                continue;
            } else {
                dest.append(buf,4096-oubl);
                return unknown;
            }
        }
    }
}

bool hot::iconv::iserror() {
    return ip==(iconv_t)(-1);
}

const hot::string hot::iconv::error(int error_code) {
    string error_str;
    switch(error_code) {
        case ok:
            error_str.assign("no error");
            break;
        case bad_direction:
            error_str.assign("unknown direction requested");
            break;
        case unknown:
            error_str.assign("unknown error");
            break;
        default:
            error_str.assign("bad sequence met");
            break;
    }
    return error_str;
}
        
hot::iconv::return_type hot::iconv::recode(const string &from, const string &to, const string &source, string &dest, bool recover)
{
    iconv ic(from,to,recover);
    return ic.recode(source,dest);
}

#endif
