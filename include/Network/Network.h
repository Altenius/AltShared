#ifndef ALT_NETWORK_H
#define ALT_NETWORK_H

#ifdef _WIN32
    #define WIN32_LEAD_AND_MEAN
    #define _WIN32_WINNT 0x501 // WinXP and higher

    #include <Windows.h>
#else
    #include <netinet/in.h>
#endif


#endif
