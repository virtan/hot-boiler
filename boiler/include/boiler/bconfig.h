#ifndef B_BCONFIG_XML_H
#define B_BCONFIG_XML_H

#include <boiler/config.h>
#include <boiler/string.h>
#include <boiler/exceptions.h>


namespace boiler {

EXCEPTION_DECLARATION(xml_exception)

#define XMLCAST (unsigned char*)

struct config_hdn_t;

class config_t
{
    public:
        config_t();
        config_t(const string &filename);
        config_t(const config_t &ptr);
        ~config_t();

        void parse(const string &filename);
        operator bool() const;
        config_t operator/(const string &node_name) const;
        config_t operator*() const;
        config_t &operator++();
        operator const char*() const;
        const char *name() const;
        const char *attribute(const string &a) const;

    private:
        config_t(void *ptr);
        void sanitize_xml(void *_ptr);
        bool is_garbage(void *ptr) const;

    private:
        config_hdn_t *r;
};

}

#endif
