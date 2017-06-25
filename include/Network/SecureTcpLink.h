#ifndef ALT_SECURETCPLINK_H
#define ALT_SECURETCPLINK_H

#include <memory>
#include "TcpLink.h"


class SecureTcpLink;

class TcpSslContext : public SslContext
{
public:
    TcpSslContext(SecureTcpLink *link);

    virtual int send(const unsigned char *buf, size_t len) override; // always returns len
    void onReceive(const char *buf, size_t len);

    void onWrite(); // called from SecureTcpLink to try handshaking

protected:
    virtual int sslSend(const unsigned char *buf, size_t len) override;

    virtual int sslReceive(unsigned char *buf, size_t len) override;

    virtual void onHandshaken() override;

private:
    std::string writeBuffer_, readBuffer_;

    SecureTcpLink *link_;
};

typedef std::shared_ptr<TcpSslContext> TcpSslContextPtr;


class SecureTcpLink : public TcpLink
{
public:
    virtual void send(const char *data, size_t len) override;

    void sendRaw(const char *data, size_t len);

    void onHandshaken();

    void onSslRead(const char *buf, size_t len);



    inline bool valid()
    {
        return valid_;
    }



protected:
    friend TcpLink;

    SecureTcpLink(Callbacks &callbacks, bool client, const char *hostname = nullptr);

private:
    virtual void onDataRead(const char *data, size_t len) override;

    virtual void onDataWrote() override;

    virtual void onConnected() override;

    TcpSslContext ssl_;

    bool valid_;
};


#endif //VOCABBOT_SECURETCPLINK_H
