#ifndef VOCABBOT_SSLCONTEXT_H
#define VOCABBOT_SSLCONTEXT_H
#include <mbedtls/ssl.h>
#include <memory>
#include "CtrDrbgContext.h"

class SslContext {
public:
    SslContext();
    ~SslContext();
    
    bool init(bool client, int transport, CtrDrbgContextPtr ctrDrbg = nullptr, const char *hostname = nullptr);
    
    static inline int sslSend(void *ctx, const unsigned char *buf, size_t len) {
        return reinterpret_cast<SslContext *>(ctx)->sslSend(buf, len);
    }
    
    static inline int sslReceive(void *ctx, unsigned char *buf, size_t len) {
        return reinterpret_cast<SslContext *>(ctx)->sslReceive(buf, len);
    }
    
    virtual int send(const unsigned char *buf, size_t len);
    virtual int receive(unsigned char *buf, size_t len);
    
    int handshake();
    
    
    virtual int sslSend(const unsigned char *buf, size_t len) =0;
    virtual int sslReceive(unsigned char *buf, size_t len) =0;

protected:
    virtual void onHandshaken() {};
    
    bool handshaken_;
    
private:
    
    
    mbedtls_ssl_context context_;
    mbedtls_ssl_config config_;
    
    CtrDrbgContextPtr ctrDrbg_;
};


#endif //VOCABBOT_SSLCONTEXT_H
