#include "Network/HttpHeader.h"



HttpHeader::HttpHeader()
{

}



void HttpHeader::add(const char *name, const char *value)
{
    EntryType pair(name, value);
    header_.push_back(pair);
    map_.insert(pair);
}



void HttpHeader::add(const std::string &name, const std::string &value)
{
    EntryType pair(name, value);
    header_.push_back(pair);
    map_.insert(pair);
}



void HttpHeader::addDefault(const char *name, const std::string &value)
{
    if (map_.find(name) == map_.end()) {
        add(name, value);
    }
}