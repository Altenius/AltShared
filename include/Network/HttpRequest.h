#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HttpClient.h"
#include "HttpParser.h"
#include "StringUtil.h"


class HttpRequest;
typedef std::shared_ptr<HttpRequest> HttpRequestPtr;

class HttpRequest : public TcpLink::Callbacks, public HttpParser::Callbacks
{
public:
    virtual void onConnect() override;

    virtual void onRead(const char *data, size_t len) override;

    virtual void onError(const std::string &error) override;

    static HttpRequestPtr request(HttpClient::HttpMethod method,
                                  const std::string &url,
                                  HttpClient::Callbacks *callbacks,
                                  const std::string &body, HttpHeader header,
                                  CookieJarPtr cookieJar);

    ~HttpRequest();

protected:
    HttpRequest(HttpClient::HttpMethod method, const std::string &url,
                HttpClient::Callbacks *callbacks, const std::string &body,
                HttpHeader header, CookieJarPtr cookieJar)
        : method_(method), url_(url), callbacks_(callbacks), body_(body),
          header_(header), parser_(this), cookieJar_(cookieJar)
    {
    }

    std::pair<bool, std::string> doRequest();


    virtual void onHeader(const std::string &key,
                          const std::string &value) override;

    virtual void onRemoteClose() override;

    virtual void onBody(const std::string &body) override;

    virtual void onStatusLine(const std::string &line) override;

    virtual void onHeadersFinished() override;

private:
    void sendRequest();


    std::string version_;
    unsigned int status_;
    std::string location_;

    HttpClient::HttpMethod method_;
    std::string url_, body_;
    HttpHeader header_;

    std::string scheme_, username_, password_, host_, path_, query_, fragment_;
    unsigned short port_;

    TcpLinkPtr link_;
    HttpParser parser_;
    CookieJarPtr cookieJar_;

    HttpClient::Callbacks *callbacks_;
};


#endif
