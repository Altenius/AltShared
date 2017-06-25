#include "Network/HttpClient.h"
#include "Network/UrlParser.h"
#include "StringUtil.h"
#include "Network/HttpParser.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>


class HttpRequest : public TcpLink::Callbacks, public HttpParser::Callbacks {
public:
	virtual void onConnect(TcpLink &link) override;
	virtual void onRead(const char *data, size_t len) override;
	virtual void onError(const std::string &error) override;
	
	static std::pair<bool, std::string> request(HttpClient::HttpMethod method, const std::string &url,
												HttpClient::Callbacks *callbacks, const std::string &body,
												HttpHeader header, CookieJarPtr cookieJar);
	
protected:
	HttpRequest(HttpClient::HttpMethod method, const std::string &url,
				HttpClient::Callbacks *callbacks, const std::string &body,
				HttpHeader header, CookieJarPtr cookieJar) : method_(method), url_(url), callbacks_(callbacks), body_(body), header_(header), parser_(this), cookieJar_(cookieJar) {
		
	}
	
	std::pair<bool, std::string> doRequest(std::shared_ptr<HttpRequest> self);
	
	std::shared_ptr<HttpRequest> self_;
	
	virtual void onHeader(const std::string &key, const std::string &value) override {
		if (cookieJar_ != nullptr) {
			if (StringUtil::lower(key) == "set-cookie") {
				cookieJar_->parse(value, host_, path_);
			}
		}
		if (StringUtil::lower(key) == "location") {
			location_ = value;
		}
		callbacks_->onHeader(key, value);
	}
	
	virtual void onRemoteClose() override {
		callbacks_->onClose();
		parser_.onRemoteClose();
		self_.reset();
	}
	
	virtual void onBody(const std::string &body) override {
		callbacks_->onBody(body);
	}
	
	virtual void onStatusLine(const std::string &line) override {
		std::stringstream ss(line);
		ss >> version_ >> status_;
	}
	
	virtual void onHeadersFinished() override {
		if (status_ == 301) { // Moved permanently
			if (!location_.empty()) {
				link_->close();
				link_ = nullptr;
				parser_.reset();
				url_ = location_;
				location_.clear();
				callbacks_->onRedirect(url_);
				header_.reset();
				auto res = doRequest(self_);
				if (!res.first) {
					onError(res.second);
				}
			}
		}
	}
	
private:
	void sendRequest();
	
	HttpClient::Callbacks *callbacks_;
	
	std::string version_;
	unsigned int status_;
	std::string location_;
	
	std::string url_, body_;
	HttpClient::HttpMethod method_;
	HttpHeader header_;

	std::string scheme_, username_, password_, host_, path_, query_, fragment_;
	unsigned short port_;
	
	TcpLink *link_;
	HttpParser parser_;
	CookieJarPtr cookieJar_;
};



void HttpRequest::onConnect(TcpLink &link) {
	link_ = &link;
	callbacks_->onConnect(link);
	sendRequest();
}

void HttpRequest::onRead(const char *data, size_t len) {
	size_t res = parser_.parse(data, len);
	if (res == std::string::npos) {
		onError("failed to parse message");
	}
}

void HttpRequest::onError(const std::string &error) {
	callbacks_->onError(error);
	self_.reset();
}

std::pair<bool, std::string> HttpRequest::request(HttpClient::HttpMethod method, const std::string &url, HttpClient::Callbacks *callbacks,
						 const std::string &body, HttpHeader header, CookieJarPtr cookieJar) {
	
	std::shared_ptr<HttpRequest> req(new HttpRequest(method, url, callbacks, body, header, cookieJar));
	return req->doRequest(req);
}

void HttpRequest::sendRequest() {
	std::stringstream ss;
	switch (method_) {
		case HttpClient::GET:
			ss << "GET ";
			break;
		case HttpClient::POST:
			ss << "POST ";
			break;
	}
	
	ss << path_ << " " << "HTTP/1.1\r\n";
	for (auto it = header_.header().begin(); it != header_.header().end(); it++) {
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n" << body_;
	
	std::string data = ss.str();
	link_->send(data.c_str(), data.size());
}

std::pair<bool, std::string> HttpRequest::doRequest(std::shared_ptr<HttpRequest> self) {
	self_ = self;
	auto res = UrlParser::parse(url_, scheme_, username_, password_, host_, port_, path_, query_, fragment_);
	if (!res.first) {
		return res;
	}
	
	header_.addDefault("Host", host_);
	header_.addDefault("User-Agent", "Clientius");
	// header_.addDefault("Connection", "Close");
	header_.addDefault("Accept-Encoding", "gzip");
	if (!body_.empty() || method_ == HttpClient::POST) {
		header_.addDefault("Content-Length", std::to_string(body_.size()));
	}

	bool ssl = false;
	if (scheme_ == "https") {
		ssl = true;
	}
	if (!TcpLink::connect(host_, port_, *self_, ssl)) {
		return std::make_pair(true, "failed to create tcp link");
	}

	return std::make_pair(true, std::string());
}




std::pair<bool, std::string> HttpClient::request(HttpClient::HttpMethod method, const std::string &url,
												 HttpClient::Callbacks *callbacks, const std::string &body,
												 HttpHeader header, CookieJarPtr cookieJar) {
	return HttpRequest::request(method, url, callbacks, body, std::move(header), cookieJar);
}