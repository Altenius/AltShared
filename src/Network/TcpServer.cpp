#include <Network/TcpServer.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <Network/NetworkHandler.h>
#include <string.h>



void TcpServer::onConnection(evconnlistener *listener, evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr)
{
    TcpServer *server = reinterpret_cast<TcpServer *>(ptr);

    server->callbacks_.onConnection(sock);
}



void TcpServer::onAcceptError(evconnlistener *listener, void *ptr)
{
    TcpServer *server = reinterpret_cast<TcpServer *>(ptr);

    server->callbacks_.onError(evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
}



TcpServerPtr TcpServer::bind(Callbacks &callbacks, const std::string &bindAddress)
{
    sockaddr_storage addr;
    int socklen = sizeof(sockaddr_storage);

    if (evutil_parse_sockaddr_port(bindAddress.c_str(), reinterpret_cast<sockaddr *>(&addr), &socklen) != 0) {
        return nullptr;
    }

    TcpServerPtr server(new TcpServer(callbacks));

    if ((server->listener_ = evconnlistener_new_bind(NetworkHandler::get().event(), onConnection, server.get(),
                                                     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 255,
                                                     reinterpret_cast<sockaddr *>(&addr), socklen)) == NULL) {
        return nullptr;
    }

    evconnlistener_set_error_cb(server->listener_, onAcceptError);

    return server;
}



TcpServer::TcpServer(Callbacks &callbacks) : callbacks_(callbacks)
{

}