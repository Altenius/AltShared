#ifndef ALT_TCPCONNECTION_H
#define ALT_TCPCONNECTION_H

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <vector>
#include <memory>
#include "../SSL/SslContext.h"

class TcpLink;


class TcpLink
{
    typedef std::shared_ptr<TcpLink> TcpLinkPtr;
public:
    class Callbacks
    {
    public:
        virtual void onConnect(TcpLink &link)
        {};



        virtual void onError(const std::string &msg)
        {};



        virtual void onRead(const char *data, size_t len)
        {};



        virtual void onWrite()
        {};



        virtual void onRemoteClose()
        {};
    };

    typedef std::shared_ptr<Callbacks> CallbacksPtr;


    virtual ~TcpLink();

    /* Attempts to connect to a host. If ssl is true, creates a TLS socket.
     * Returns false if a connection could not be created. */
    static bool connect(const std::string &host, unsigned short port, Callbacks &callbacks, bool ssl);

    /* Attempts to send data of size len through the socket. */
    virtual void send(const char *data, size_t len);

    /* Closes the socket and frees resources */
    void close();

    /* Creates a TcpLink with an existing socket. callbacks MUST NOT be null. */
    TcpLink(Callbacks &callbacks, evutil_socket_t sock);

protected:
    TcpLink(Callbacks &callbacks);

    /* readCallback, writeCallback, and eventCallback are
     * used by libevent. */
    static void readCallback(bufferevent *bev, void *data);

    static void writeCallback(bufferevent *bev, void *data);

    static void eventCallback(bufferevent *bev, short events, void *data);

    /* Called when data of size len has been read from the socket */
    virtual void onDataRead(const char *data, size_t len);

    /* Called when data from the output buffer was sent */
    virtual void onDataWrote();

    /* Called when the socket has connected to the remote host */
    virtual void onConnected();

    bufferevent *bufferevent_;
    Callbacks &callbacks_;

    TcpLinkPtr self_;

    bool initialized_;
private:

public:
    /* Called when an error occurs */
    virtual void onError(const std::string &error);
};

typedef std::shared_ptr<TcpLink> TcpLinkPtr;


#endif //VOCABBOT_TCPCONNECTION_H
