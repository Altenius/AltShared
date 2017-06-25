#include <iostream>
#include "Network/HttpParser.h"
#include "StringUtil.h"

HttpParser::HttpParser(HttpParser::Callbacks *callbacks) : callbacks_(callbacks), headerParser_(this), contentLength_(-1) {

}

size_t HttpParser::parse(const char *data, size_t size) {
    size_t inBuffer = buffer_.size();
    if (statusLine_.empty()) {
        buffer_.append(data, size);
        size_t statusParseConsumed = parseStatusLine();
        if (statusLine_.empty()) {
            return size;
        }
        
        size_t headerParseConsumed = headerParser_.parse(buffer_.data(), buffer_.size());
        if (headerParseConsumed == std::string::npos) {
            return std::string::npos;
        }

        buffer_.erase(0, headerParseConsumed);
        if (!headerParser_.done()) {
            return size;
        }
        headersFinished();

        size_t bodyParseConsumed = parseBody(buffer_.data(), buffer_.size());
        if (bodyParseConsumed == std::string::npos) {
            return std::string::npos;
        }
        
        return statusParseConsumed + headerParseConsumed + bodyParseConsumed - inBuffer;
    }
    
    if (!headerParser_.done()) {
        size_t headerParseConsumed = headerParser_.parse(data, size);
        if (headerParseConsumed == std::string::npos) {
            return std::string::npos;
        }
        
        if (headerParser_.done()) {
            headersFinished();

            size_t bodyParseConsumed = parseBody(data + headerParseConsumed, size - headerParseConsumed);
            if (bodyParseConsumed == std::string::npos) {
                return std::string::npos;
            }
        }
        return size;
    }
    
    return parseBody(data, size);
}

size_t HttpParser::parseStatusLine() {
    size_t end = buffer_.find("\r\n");
    if (end == std::string::npos) {
        return buffer_.size();
    }
    
    statusLine_ = buffer_.substr(0, end);
    
    callbacks_->onStatusLine(statusLine_);
    
    buffer_.erase(0, end + 2);
    return end + 2;
}

size_t HttpParser::parseBody(const char *data, size_t size) {
    if (transferEncodingParser_ == nullptr) {
        return std::string::npos;
    }
    
    return transferEncodingParser_->parse(data, size);
}

void HttpParser::headersFinished() {
    callbacks_->onHeadersFinished();
    
    transferEncodingParser_ = TransferEncodingParser::create(*this, transferEncoding_, contentLength_);
    if (transferEncodingParser_ == nullptr) {
        callbacks_->onError("unknown transfer encoding");
    }

    if (!contentEncoding_.empty() && contentEncoding_ != "identity") { // TODO: handling multiple content encodings
        contentDecoder_ = ContentDecoder::create(*this, contentEncoding_);
        if (contentDecoder_ == nullptr) {
            callbacks_->onError("unknown content encoding");
        }
    }
}

void HttpParser::onHeader(const std::string &key, const std::string &value) {
    callbacks_->onHeader(key, value);
    std::string l = StringUtil::lower(key);
    if (l == "transfer-encoding") {
        transferEncoding_ = value;
    } else if (l == "content-encoding") {
        contentEncoding_ = value;
    } else if (l == "content-length") {
        if (!StringUtil::stoi(value, contentLength_)) {
            callbacks_->onError("invalid content-length");
        }
    }
}

void HttpParser::onRemoteClose() {
    if (transferEncodingParser_ != nullptr) {
        transferEncodingParser_->onClose();
    }
}

void HttpParser::reset() {
    statusLine_.clear();
    buffer_.clear();
    bodyBuffer_.clear();
    transferEncoding_.clear();
    contentLength_ = -1;
    headerParser_.reset();
}

void HttpParser::onDecodedBodyData(const char *data, size_t len) {
    bodyBuffer_.append(data, len);
}

void HttpParser::onBodyData(const char *data, size_t len) {
    if (contentDecoder_ == nullptr) {
        bodyBuffer_.append(data, len);
        return;
    }

    contentDecoder_->parse(data, len);
}

void HttpParser::onBodyComplete() {
    callbacks_->onBody(bodyBuffer_);
    bodyBuffer_.clear();
}

void HttpParser::onError(const std::string &error) {
    callbacks_->onError(error);
}
