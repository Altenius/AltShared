#ifndef ALT_URLPARSER_H
#define ALT_URLPARSER_H

#include <string>

class UrlParser
{
public:
    static std::pair<bool, std::string>
    parse(const std::string &url, std::string &scheme, std::string &username,
          std::string &password, std::string &host, unsigned short &port,
          std::string &path, std::string &query, std::string &fragment);

    static std::pair<bool, std::string> parseAuthority(const std::string &auth,
                                                       std::string &username,
                                                       std::string &password,
                                                       std::string &host,
                                                       unsigned short &port);

    static unsigned short getDefaultPort(const std::string &scheme);
};


#endif // VOCABBOT_URLPARSER_H
