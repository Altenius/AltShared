#ifndef ALT_TCPSERVER_H
#define ALT_TCPSERVER_H

#include <string>
#include <memory>
#include <Network/TcpLink.h>
#include <event2/listener.h>

class TcpServer;
typedef std::shared_ptr<TcpServer> TcpServerPtr;

class TcpServer { 
public:
    class Callbacks {
    public:
        virtual void onConnection(evutil_socket_t sock) { evutil_closesocket(sock); };
        virtual void onError(const char *error) {};
    };
    
    /* Attempts to create a listening socket binded to bindAddress. Returns
     * a server object on success and nullptr on failure */
    static TcpServerPtr bind(Callbacks &callbacks, const std::string &bindAddress);
protected:
    TcpServer(Callbacks &callbacks);
    Callbacks &callbacks_;
    
private:
    evconnlistener *listener_;

public:
    static void onConnection(evconnlistener *listener, evutil_socket_t sock, struct sockaddr *addr, int len, void *ptr);
    static void onAcceptError(evconnlistener *listener, void *ptr);
};

#endif //ALTMANAGE_TCPSERVER_H
