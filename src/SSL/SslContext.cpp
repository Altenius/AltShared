#include "SSL/SslContext.h"



SslContext::SslContext()
{
    mbedtls_ssl_init(&context_);
    mbedtls_ssl_config_init(&config_);
}



SslContext::~SslContext()
{
    mbedtls_ssl_free(&context_);
    mbedtls_ssl_config_free(&config_);
}



bool SslContext::init(bool client, int transport, CtrDrbgContextPtr ctrDrbg, const char *hostname)
{
    ctrDrbg_ = ctrDrbg;
    if (ctrDrbg_ == nullptr) {
        ctrDrbg_ = CtrDrbgContextPtr(new CtrDrbgContext);
        ctrDrbg_->init();
    }

    if (mbedtls_ssl_config_defaults(&config_, client ? MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER, transport,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        return false;
    }
    mbedtls_ssl_conf_authmode(&config_, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&config_, mbedtls_ctr_drbg_random, ctrDrbg_->context());

    mbedtls_ssl_set_bio(&context_, this, &SslContext::sslSend, &SslContext::sslReceive, NULL);
    if (hostname != nullptr) {
        mbedtls_ssl_set_hostname(&context_, hostname);
    }

    return mbedtls_ssl_setup(&context_, &config_) == 0;
}



int SslContext::send(const unsigned char *buf, size_t len)
{
    if (!handshaken_) {
        int res;
        if ((res = handshake()) != 0) {
            return res;
        }
    }

    return mbedtls_ssl_write(&context_, buf, len);

}



int SslContext::receive(unsigned char *buf, size_t len)
{
    if (!handshaken_) {
        int res;
        if ((res = handshake()) != 0) {
            return res;
        }
    }

    return mbedtls_ssl_read(&context_, buf, len);
}



int SslContext::handshake()
{
    int res = mbedtls_ssl_handshake(&context_);
    if (res == 0) {
        handshaken_ = true;
        onHandshaken();
    }

    return res;
}
