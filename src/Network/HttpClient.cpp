#include "Network/HttpClient.h"
#include "Network/HttpParser.h"
#include "Network/HttpRequest.h"
#include "Network/UrlParser.h"
#include "StringUtil.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>


HttpRequestPtr HttpClient::request(HttpClient::HttpMethod method,
                                   const std::string &url,
                                   HttpClient::Callbacks *callbacks,
                                   const std::string &body, HttpHeader header,
                                   CookieJarPtr cookieJar)
{
    return HttpRequest::request(method, url, callbacks, body, std::move(header),
                                cookieJar);
}
