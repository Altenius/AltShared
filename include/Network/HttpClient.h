#ifndef ALT_HTTPSCLIENT_H
#define ALT_HTTPSCLIENT_H

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ssl.h>
#include <string>

#include "HttpHeader.h"
#include "TcpLink.h"
#include "Cookie.h"

class HttpClient
{
public:
    class Callbacks
    {
    public:
        virtual void onConnect(TcpLink &link)
        {};



        virtual void onRequestSent()
        {};



        virtual void onHeader(const std::string &key, const std::string &value)
        {};



        virtual void onBody(const std::string &body)
        {};



        virtual void onError(const std::string &error)
        {};



        virtual void onRedirect(const std::string &location)
        {};



        virtual void onClose()
        {};
    };

    typedef std::shared_ptr<Callbacks> CallbacksPtr;

    enum HttpMethod
    {
        GET,
        POST
    };

    static std::pair<bool, std::string> request(HttpMethod method,
                                                const std::string &url,
                                                Callbacks *callbacks,
                                                const std::string &body = "",
                                                HttpHeader header = HttpHeader(),
                                                CookieJarPtr cookieJar = nullptr);


};

#endif