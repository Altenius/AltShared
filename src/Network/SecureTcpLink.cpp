#include <mbedtls/error.h>
#include "Network/SecureTcpLink.h"

void SecureTcpLink::onDataRead(const char *data, size_t len) {
    ssl_.onReceive(data, len);
}

void SecureTcpLink::onDataWrote() {
    ssl_.onWrite();
    TcpLink::onDataWrote();
}


int TcpSslContext::sslSend(const unsigned char *buf, size_t len) {
    link_->sendRaw(reinterpret_cast<const char*>(buf), len);
    return static_cast<int>(len);
}

int TcpSslContext::sslReceive(unsigned char *buf, size_t len) {
    if (readBuffer_.empty()) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }
    
    len = len > readBuffer_.size() ? readBuffer_.size() : len;
    
    readBuffer_.copy(reinterpret_cast<char*>(buf), len, 0);
    readBuffer_.erase(0, len);
    return static_cast<int>(len);
}

void SecureTcpLink::onConnected() {
    if (ssl_.handshake() == 0) {
        onHandshaken();
    }
}

void SecureTcpLink::send(const char *data, size_t len) {
    ssl_.send(reinterpret_cast<const unsigned char*>(data), len);
}

void SecureTcpLink::sendRaw(const char *data, size_t len) {
    TcpLink::send(data, len);
}

TcpSslContext::TcpSslContext(SecureTcpLink *link) : link_(link) {
    
}

void TcpSslContext::onWrite() {
    if (!handshaken_) {
        if (handshake() == 0) {
            link_->onHandshaken();
        } else {
            return;
        }
    }
    
    if (!writeBuffer_.empty()) {
        int ret = SslContext::send(reinterpret_cast<const unsigned char *>(writeBuffer_.data()), writeBuffer_.size());
        if (ret > 0) {
            writeBuffer_.erase(0, ret);
        }
    }
}

int TcpSslContext::send(const unsigned char *buf, size_t len) {
    if (!writeBuffer_.empty()) {
        int ret = SslContext::send(reinterpret_cast<const unsigned char*>(writeBuffer_.data()), writeBuffer_.size());
        if (ret > 0) {
            writeBuffer_.erase(0, ret);
        } else if (ret < 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                char data[1024];
                mbedtls_strerror(ret, data, 1024);
                link_->onError(std::string(data));
            }
        }
        if (!writeBuffer_.empty()) {
            writeBuffer_.append(reinterpret_cast<const char*>(buf), len);
            return static_cast<int>(len);
        }
    }
    int ret = SslContext::send(buf, len);
    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
        readBuffer_.append(reinterpret_cast<const char*>(buf), len);
        return static_cast<int>(len);
    }
    
    if (ret != len) {
        readBuffer_.append(reinterpret_cast<const char *>(buf) + ret, len - ret);
    }
    
    return static_cast<int>(len);
}

void SecureTcpLink::onHandshaken() {
    callbacks_.onConnect(*this);
}

void TcpSslContext::onReceive(const char *buf, size_t len) {
    readBuffer_.append(buf, len);
    char data[1024];
    int ret;
    while ((ret = SslContext::receive(reinterpret_cast<unsigned char*>(data), 1024)) > 0) {
        link_->onSslRead(data, ret);
    } 
    
    if (ret < 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            char data[1024];
            mbedtls_strerror(ret, data, 1024);
            link_->onError(std::string(data));
        }
    }
}

void SecureTcpLink::onSslRead(const char *buf, size_t len) {
    TcpLink::onDataRead(buf, len);
}

SecureTcpLink::SecureTcpLink(TcpLink::Callbacks &callbacks, bool client, const char *hostname) : TcpLink(callbacks), ssl_(this) {
    valid_ = ssl_.init(client, MBEDTLS_SSL_TRANSPORT_STREAM, nullptr, hostname);
}

void TcpSslContext::onHandshaken() {
    link_->onHandshaken();
}
