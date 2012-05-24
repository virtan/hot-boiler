#include <boiler/bconfig.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

namespace boiler {

EXCEPTION_DEFINITION(xml_exception)

struct config_hdn_t {
    xmlNodePtr node;
    xmlDocPtr doc;
};

}

boiler::config_t::config_t() : r(new config_hdn_t)
{
    r->node=0;
    r->doc=0;
}

boiler::config_t::config_t(const string &filename) : r(new config_hdn_t)
{
    r->node=0;
    r->doc=0;
    parse(filename);
}

boiler::config_t::config_t(const config_t &ptr) : r(new config_hdn_t)
{
    r->node=ptr.r->node;
    r->doc=0;
}

boiler::config_t::config_t(void *ptr) : r(new config_hdn_t)
{
    r->node=static_cast<xmlNodePtr>(ptr);
    r->doc=0;
}

boiler::config_t::~config_t()
{
    if(r->doc) {
        xmlFreeDoc(r->doc);
    }
    delete r;
}

void boiler::config_t::parse(const string &filename)
{
    r->doc=xmlReadFile(filename.c_str(),0,XML_PARSE_NOERROR|XML_PARSE_NOWARNING|XML_PARSE_NOBLANKS|XML_PARSE_NOCDATA|XML_PARSE_NOENT);
    if(!r->doc) {
        throw xml_exception(string("can't parse config ")+filename,__PRETTY_FUNCTION__);
    }
    sanitize_xml(r->doc->children);
    r->node=r->doc->children;
    if(!r->node) {
        throw xml_exception(string("empty config ")+filename,__PRETTY_FUNCTION__);
    }
    if(xmlStrcmp(r->node->name, XMLCAST "boiler")) {
        throw xml_exception(string("wrong config format ")+filename,__PRETTY_FUNCTION__);
    }
}

boiler::config_t::operator bool() const
{
    return r->node;
}

boiler::config_t boiler::config_t::operator/(const string &node_name) const
{
    if(r->node)
        for(xmlNodePtr p=r->node->children;p;p=p->next)
            if(!xmlStrcmp(p->name, XMLCAST node_name.c_str()))
                return config_t(static_cast<void*>(p));
    return config_t(0);
}

boiler::config_t &boiler::config_t::operator++()
{
    if(r->node)
        r->node=r->node->next;
    return *this;
}

boiler::config_t boiler::config_t::operator*() const
{
    if(r->node)
        return config_t(static_cast<void*>(r->node->children));
    return config_t(0);
}

bool boiler::config_t::is_garbage(void *ptr) const
{
    return static_cast<xmlNodePtr>(ptr)->type>XML_TEXT_NODE;
}

void boiler::config_t::sanitize_xml(void *_ptr)
{
    xmlNodePtr ptr=static_cast<xmlNodePtr>(_ptr);
    while(ptr) {
        if(ptr->children)
            sanitize_xml(static_cast<void*>(ptr->children));
        xmlNodePtr prev=ptr;
        ptr=ptr->next;
        if(is_garbage(static_cast<void*>(prev))) {
            xmlUnlinkNode(prev);
            xmlFreeNode(prev);
        }
    }
}

boiler::config_t::operator const char *() const
{
    if(!r->node)
        return "";
    for(xmlNodePtr p=r->node->children;p;p=p->next)
        if(!xmlStrcmp(p->name, XMLCAST "text"))
            return (const char*) p->content;
    return "";
}

const char *boiler::config_t::name() const
{
    if(!r->node || !r->node->name)
        return "";
    return (const char*) r->node->name;
}

const char *boiler::config_t::attribute(const string &a) const
{
    if(!r->node)
        return 0;
    for(xmlAttrPtr attr=r->node->properties;attr;attr=attr->next)
        if(!xmlStrcmp(attr->name, XMLCAST a.c_str()))
            return (const char*) attr->children->content;
    return 0;
}
