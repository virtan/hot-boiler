#ifndef B_BCONFIG_XML_H
#define B_BCONFIG_XML_H

#include <boiler/config.h>
#include <boiler/bconfig.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

namespace boiler {

#define XMLCAST (unsigned char*)

class config_xml_t : public config_t
{
    public:
        config_xml_t();
        config_xml_t(const string &filename);
        config_xml_t(const config_xml_t &ptr);
        ~config_xml_t();

        void parse(const string &filename);
        operator bool() const;
        config_ operator/(const string &node_name) const;
        config_t operator*() const;
        config_t &operator++();
        operator const char*() const;
        const char *name() const;

    private:
        config_xml_t(xmlNodePtr ptr);
        void sanitize_xml(xmlNodePtr ptr);
        bool is_garbage(xmlNodePtr ptr) const;

    private:
        xmlNodePtr node;
        xmlDocPtr doc;
};

}

#endif
