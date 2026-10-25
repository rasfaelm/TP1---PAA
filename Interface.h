#ifndef INTERFACE_H
#define INTERFACE_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <stdio.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
//função habilita o uso de cores e formatação ANSI no terminal do Windows
void habilitarANSI();
void Interface();

// Códigos de cores ANSI
#define pre    "\033[30m" // preto
#define verm   "\033[31m" // vermelho
#define ama    "\033[33m" // amarelo
#define azu    "\033[34m" // azul
#define mag    "\033[35m" // magenta
#define cia    "\033[36m" // ciano
#define verd   "\033[32m" // verde
#define nor    "\033[0m"  // normal

#endif