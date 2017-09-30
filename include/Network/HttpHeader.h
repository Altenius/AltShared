#ifndef ALT_HTTPHEADER_H
#define ALT_HTTPHEADER_H

#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class HttpHeader
{
public:
    typedef std::pair<std::string, std::string> EntryType;
    typedef std::vector<EntryType> HeaderType;
    typedef std::unordered_map<std::string, std::string> MapType;

    HttpHeader();

    void add(const char *name, const char *value);

    void add(const std::string &name, const std::string &value);

    void addDefault(const char *name, const std::string &value);


    inline HeaderType &header()
    {
        return header_;
    }


    inline MapType &map()
    {
        return map_;
    }


    inline void reset()
    {
        map_.clear();
        header_.clear();
    }


private:
    HeaderType header_;
    MapType map_;
};

#endif
