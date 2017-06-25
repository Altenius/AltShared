#include "Network/UrlParser.h"
#include "StringUtil.h"



std::pair<bool, std::string> UrlParser::parse(const std::string &url, std::string &scheme, std::string &username,
                                              std::string &password, std::string &host, unsigned short &port,
                                              std::string &path, std::string &query, std::string &fragment)
{
    auto sColon = url.find(':');
    if (sColon == std::string::npos) {
        return std::make_pair(false, "No scheme found");
    }

    scheme = StringUtil::lower(url.substr(0, sColon));
    if ((port = getDefaultPort(scheme)) == 0) {
        return std::make_pair(false, "Unknown scheme");
    }

    auto authStart = sColon + 1;
    if (url.substr(authStart, 2) == "//") {
        authStart += 2;
    }

    auto authEnd = url.find('/', authStart);
    if (authEnd == std::string::npos) {
        authEnd = url.size();
    }

    auto res = parseAuthority(url.substr(authStart, authEnd - authStart), username,
                              password, host, port);
    if (!res.first) {
        return res;
    }

    if (authEnd != url.size()) {
        auto pathStart = authEnd + 1;
        auto pathEnd = url.find_first_of("?#", pathStart);
        if (pathEnd == std::string::npos) {
            pathEnd = url.size();
        }

        path = StringUtil::absolutePath(url.substr(pathStart, pathEnd));

        auto fragmentStart = url.find('#', pathEnd);
        if (fragmentStart != std::string::npos) {
            fragment = url.substr(fragmentStart + 1);
        } else {
            fragmentStart = url.size();
        }

        if (fragmentStart != pathEnd) {
            query = url.substr(pathEnd + 1, fragmentStart - pathEnd);
        }
    }

    if (path.empty()) {
        path = "/";
    }

    return std::make_pair(true, std::string());
}



std::pair<bool, std::string> UrlParser::parseAuthority(const std::string &auth, std::string &username,
                                                       std::string &password, std::string &host, unsigned short &port)
{
    auto authenEnd = auth.find('@');
    if (authenEnd != std::string::npos) {
        auto authenColon = auth.substr(0, authenEnd).find(':');
        if (authenColon == std::string::npos) {
            return std::make_pair(false, "Invalid authentication section");
        }

        username = auth.substr(0, authenColon);
        password = auth.substr(authenColon + 1, authenEnd);
        authenEnd++;
    } else {
        authenEnd = 0;
    }

    auto hostEnd = auth.find(':');
    if (hostEnd == std::string::npos) {
        hostEnd = auth.size();
    } else {
        if (!StringUtil::stoi(auth.substr(hostEnd + 1), port)) {
            return std::make_pair(false, "Invalid port");
        }
    }

    host = auth.substr(authenEnd, hostEnd);
    if (host.empty()) {
        return std::make_pair(false, "No host");
    }

    return std::make_pair(true, std::string());
}



unsigned short UrlParser::getDefaultPort(const std::string &scheme)
{
    if (scheme == "http") {
        return 80;
    } else if (scheme == "https") {
        return 443;
    }

    return 0;
}
