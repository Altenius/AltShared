#ifndef ALT_NETWORK_HANDLER_H
#define ALT_NETWORK_HANDLER_H
#include <event2/event.h>
#include <event2/dns.h>
#include <thread>
#include "TcpLink.h"

class NetworkHandler {
public:
    static NetworkHandler &get();
    void init();
    void terminate();

    inline event_base *event() {
        return event_;
    }
    
    inline evdns_base *evdns() {
        return evdns_;
    }
    
    void addLink(TcpLinkPtr link);
    void removeLink(TcpLink *link);
    
    inline void join() {
        run_thread_.join();
    }
protected:
    NetworkHandler();
    ~NetworkHandler();
    
    static void runLoop(NetworkHandler *handler);
    
private:
    event_base *event_;
    evdns_base *evdns_;
    
    bool hasTerminated_;
    std::thread run_thread_;
    
    std::vector<TcpLinkPtr> connections_;
};


#endif //VOCABBOT_NETWORK_HANDLER_H
