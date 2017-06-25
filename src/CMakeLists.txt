cmake_minimum_required(VERSION 2.9)
project(AltShared)

set(ALTSHARED_SOURCE
        Network/Cookie.cpp
        Network/HeaderParser.cpp
        Network/HttpClient.cpp
        Network/HttpHeader.cpp
        Network/HttpParser.cpp
        Network/NetworkHandler.cpp
        Network/SecureTcpLink.cpp
        Network/TcpLink.cpp
        Network/TransferEncodingParser.cpp
        Network/UrlParser.cpp
        Network/TcpServer.cpp
        Network/ContentDecoder.cpp
        Network/TcpServer.cpp
        
        SSL/CtrDrbgContext.cpp
        SSL/EntropyContext.cpp
        SSL/SslContext.cpp

        StringUtil.cpp
        WriteBuffer.cpp
)

add_library(AltShared ${ALTSHARED_SOURCE})
target_link_libraries(AltShared mbedtls event_core event_extra zlibstatic ${CMAKE_THREAD_LIBS_INIT})