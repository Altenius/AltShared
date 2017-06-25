#include <iostream>
#include "Network/NetworkHandler.h"

NetworkHandler &NetworkHandler::get() {
    static NetworkHandler net;
    return net;
}

NetworkHandler::NetworkHandler() : hasTerminated_(true) {
    
}

NetworkHandler::~NetworkHandler() {
    if (!hasTerminated_) {
        terminate();
    }
}

void NetworkHandler::init() {
    hasTerminated_ = false;
    event_ = event_base_new();
    evdns_ = evdns_base_new(event_, 1);
    
    event_base_priority_init(event_, 1);
    
    run_thread_ = std::thread(runLoop, this);
}

void NetworkHandler::terminate() {
    if (!hasTerminated_) {
        hasTerminated_ = true;
        event_base_loopbreak(event_);
        run_thread_.detach();
        event_base_free(event_);
        evdns_base_free(evdns_, 1);
    }
}

void NetworkHandler::runLoop(NetworkHandler *handler) {
    event_base_loop(handler->event_, EVLOOP_NO_EXIT_ON_EMPTY);
}

void NetworkHandler::addLink(TcpLinkPtr link) {
    connections_.push_back(link);
}

void NetworkHandler::removeLink(TcpLink *link) {
    for (auto it = connections_.begin(); it != connections_.end(); it++) {
        if (it->get() == link) {
            connections_.erase(it);
            return;
        }
    }
}
