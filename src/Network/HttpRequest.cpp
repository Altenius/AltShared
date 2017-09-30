#include "Network/HttpRequest.h"
#include <Network/UrlParser.h>

#include <iostream>

HttpRequest::~HttpRequest()
{
}


void HttpRequest::onHeader(const std::string &key, const std::string &value)
{
    if (cookieJar_ != nullptr)
    {
        if (StringUtil::lower(key) == "set-cookie")
        {
            cookieJar_->parse(value, host_, path_);
        }
    }
    if (StringUtil::lower(key) == "location")
    {
        location_ = value;
    }
    callbacks_->onHeader(key, value);
}


void HttpRequest::onRemoteClose()
{
    callbacks_->onClose();
    parser_.onRemoteClose();
    link_.reset();
}


void HttpRequest::onBody(const std::string &body)
{
    callbacks_->onBody(body);
}


void HttpRequest::onStatusLine(const std::string &line)
{
    std::stringstream ss(line);
    ss >> version_ >> status_;
}


void HttpRequest::onHeadersFinished()
{
    if (status_ == 301)
    { // Moved permanently
        if (!location_.empty())
        {
            link_->close();
            link_ = nullptr;
            parser_.reset();
            url_ = location_;
            location_.clear();
            callbacks_->onRedirect(url_);
            header_.reset();
            auto res = doRequest();
            if (!res.first)
            {
                onError(res.second);
            }
        }
    }
}


void HttpRequest::onConnect()
{
    callbacks_->onConnect();
    sendRequest();
}


void HttpRequest::onRead(const char *data, size_t len)
{
    size_t res = parser_.parse(data, len);
    if (res == std::string::npos)
    {
        onError("failed to parse message");
    }
}


void HttpRequest::onError(const std::string &error)
{
    link_->close();
    callbacks_->onError(error);
}


HttpRequestPtr HttpRequest::request(HttpClient::HttpMethod method,
                                    const std::string &url,
                                    HttpClient::Callbacks *callbacks,
                                    const std::string &body, HttpHeader header,
                                    CookieJarPtr cookieJar)
{

    std::shared_ptr<HttpRequest> req(
        new HttpRequest(method, url, callbacks, body, header, cookieJar));
    req->doRequest();
    return req;
}


void HttpRequest::sendRequest()
{
    std::stringstream ss;
    switch (method_)
    {
    case HttpClient::GET:
        ss << "GET ";
        break;
    case HttpClient::POST:
        ss << "POST ";
        break;
    }


    ss << path_;
    if (!query_.empty())
    {
        ss << "?" << query_;
    }
    ss << " "
       << "HTTP/1.1\r\n";
    for (auto it = header_.header().begin(); it != header_.header().end(); it++)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }

    if (cookieJar_)
    {
        std::string cookies = cookieJar_->getCookies(host_, path_);
        if (!cookies.empty())
        {
            ss << "Cookie: " << cookies << "\r\n";
        }
    }
    ss << "\r\n" << body_;

    std::string data = ss.str();

    // std::cout << "request: " << data << std::endl;
    link_->send(data.c_str(), data.size());
}


std::pair<bool, std::string> HttpRequest::doRequest()
{
    auto res = UrlParser::parse(url_, scheme_, username_, password_, host_,
                                port_, path_, query_, fragment_);
    if (!res.first)
    {
        return res;
    }

    header_.addDefault("Host", host_);
    header_.addDefault("User-Agent", "Clientius");
    header_.addDefault("Accept-Encoding", "gzip");
    header_.addDefault("Connection", "keep-alive");
    if (!body_.empty() || method_ == HttpClient::POST)
    {
        header_.addDefault("Content-Length", std::to_string(body_.size()));
    }

    bool ssl = false;
    if (scheme_ == "https")
    {
        ssl = true;
    }
    else if (scheme_ != "http")
    {
        return std::make_pair(false, "Invalid scheme");
    }
    if (!(link_ = TcpLink::connect(host_, port_, *this, ssl)))
    {
        return std::make_pair(false, "failed to create tcp link");
    }
    return std::make_pair(true, std::string());
}
