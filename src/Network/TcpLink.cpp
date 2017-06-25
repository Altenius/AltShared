#include <stdlib.h>
#include "Network/Network.h"
#include "Network/TcpLink.h"
#include "Network/NetworkHandler.h"
#include "Network/SecureTcpLink.h"



bool TcpLink::connect(const std::string &host, unsigned short port, Callbacks &callbacks, bool ssl)
{
    TcpLinkPtr link;
    if (ssl) {
        link.reset(new SecureTcpLink(callbacks, true, host.c_str()));
    } else {
        link.reset(new TcpLink(callbacks));
    }
    NetworkHandler::get().addLink(link);
    link->self_ = link;
    bufferevent_setcb(link->bufferevent_, readCallback, writeCallback, eventCallback, link.get());
    bufferevent_enable(link->bufferevent_, EV_READ | EV_WRITE);

    sockaddr_storage sa;
    int salen = sizeof(sa);

    if (evutil_parse_sockaddr_port(host.c_str(), reinterpret_cast<sockaddr *>(&sa), &salen) == 0) {
        if (sa.ss_family == AF_INET6) {
            reinterpret_cast<sockaddr_in6 *>(&sa)->sin6_port = htons(port);
        } else if (sa.ss_family == AF_INET) {
            reinterpret_cast<sockaddr_in *>(&sa)->sin_port = htons(port);
        }

        if (bufferevent_socket_connect(link->bufferevent_, reinterpret_cast<sockaddr *>(&sa), salen) == 0) {
            link->initialized_ = true;
            return true;
        }
    } else {
        if (bufferevent_socket_connect_hostname(link->bufferevent_, NetworkHandler::get().evdns(), AF_UNSPEC,
                                                host.c_str(), port) == 0) {
            link->initialized_ = true;
            return true;
        }
    }

    link->self_.reset();
    NetworkHandler::get().removeLink(link.get());
    callbacks.onError("failed to connect to remote host");

    return false;
}



TcpLink::TcpLink(Callbacks &callbacks) : callbacks_(callbacks), initialized_(false)
{
    bufferevent_ = bufferevent_socket_new(NetworkHandler::get().event(), -1, BEV_OPT_CLOSE_ON_FREE);
}



TcpLink::TcpLink(Callbacks &callbacks, evutil_socket_t sock) : callbacks_(callbacks), initialized_(true)
{
    bufferevent_ = bufferevent_socket_new(NetworkHandler::get().event(), sock, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bufferevent_, readCallback, writeCallback, eventCallback, this);
    bufferevent_enable(bufferevent_, EV_READ | EV_WRITE);

    self_.reset(this);
    NetworkHandler::get().addLink(self_);
}



TcpLink::~TcpLink()
{
    if (bufferevent_ != nullptr) {
        bufferevent_free(bufferevent_);
    }
    NetworkHandler::get().removeLink(this);
}



void TcpLink::readCallback(bufferevent *bev, void *data)
{
    TcpLink *link = reinterpret_cast<TcpLink *>(data);

    char buffer[1024];
    size_t res;
    while ((res = bufferevent_read(bev, buffer, 1024)) > 0) {
        link->onDataRead(buffer, res);
    }
}



void TcpLink::onError(const std::string &error)
{
    callbacks_.onError(error);
}



void TcpLink::writeCallback(bufferevent *bev, void *data)
{
    TcpLink *link = reinterpret_cast<TcpLink *>(data);

    link->onDataWrote();
}



void TcpLink::eventCallback(bufferevent *bev, short events, void *data)
{
    TcpLink *link = reinterpret_cast<TcpLink *>(data);

    if (events & BEV_EVENT_CONNECTED) {
        link->onConnected();
    }

    if (events & BEV_EVENT_ERROR) {
        int err = bufferevent_socket_get_dns_error(bev);
        if (err) {
            link->callbacks_.onError(evutil_gai_strerror(err));
        } else {
            link->callbacks_.onError("unknown error");
        }
    }

    if (events & BEV_EVENT_READING) {
        link->callbacks_.onError("error reading");
    }

    if (events & BEV_EVENT_WRITING) {
        link->callbacks_.onError("error writing");
    }

    if (events & BEV_EVENT_TIMEOUT) {
        link->callbacks_.onError("timeout");
    }

    if (events & BEV_EVENT_EOF) {
        link->callbacks_.onRemoteClose();
        link->self_.reset();
        return;
    }
}



void TcpLink::send(const char *data, size_t len)
{
    if (bufferevent_write(bufferevent_, data, len) != 0) {
        callbacks_.onError("failed to write to buffer");
    }
}



void TcpLink::close()
{
    bufferevent_free(bufferevent_);
    bufferevent_ = nullptr;
    self_.reset();
}



void TcpLink::onDataRead(const char *data, size_t len)
{
    callbacks_.onRead(data, len);
}



void TcpLink::onDataWrote()
{
    callbacks_.onWrite();
}



void TcpLink::onConnected()
{
    callbacks_.onConnect(*this);
}
