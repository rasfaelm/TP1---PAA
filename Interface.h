
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>

#ifdef _WIN32
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif

    #include <windows.h>

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#endif

void habilitarANSI(void);
void Interface(void);

// Códigos de cores ANSI
#define pre    "\033[30m"
#define verm   "\033[31m"
#define ama    "\033[33m"
#define azu    "\033[34m"
#define mag    "\033[35m"
#define cia    "\033[36m"
#define verd   "\033[32m"
#define nor    "\033[0m"

#endif
