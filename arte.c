#include <stdio.h>
#include "arte.h"

#define pre  "\033[30m"
#define VERM "\033[31m"
#define ama  "\033[33m"
#define AZU  "\033[34m"
#define mag  "\033[35m"
#define cia  "\033[36m"
#define VERD "\033[32m"
#define NOR  "\033[0m"

void menu(void) {
    printf(VERM "                          )       \\   /      (                         \n");
    printf("                         /|\\      )\\_/(     /|\\                        \n" NOR);
    printf("*" VERM "                       / | \\    (/\\|/\\)   / | \\                      " NOR "*\n");
    printf("|`.____________________" VERM "/" NOR "__" VERM "|" NOR "__o____" VERM "\\" NOR AZU "`" NOR VERM "|" NOR AZU "'" NOR VERM "/" NOR "___o__" VERM "|" NOR "__" VERM "\\" NOR "___________________.'|\n");
    printf("|                           '^`    " VERM "\\|/" NOR "   '^`                          |\n");
    printf("|                                   " VERM "V" NOR "                                 |\n");
    printf("|                  _______      " VERM " ________  " NOR "  _______                  |\n");
    printf("|                 |   _   \\    " VERM " |       | " NOR "  |   _   \\                 |\n");
    printf("|                 |  | \\   \\  " VERM "  |   ____|" NOR "   |  | \\   \\                |\n");
    printf("|                 |  |  \\   |  " VERM " |  |____  " NOR "  |  |  \\   |               |\n");
    printf("|                 |  |   |  |   " VERM "|   ____|  " NOR " |  |   |  |               |\n");
    printf("|                 |  |  /   /   " VERM "|  |____   " NOR " |  |  /   /               |\n");
    printf("|                 |  |_/   /    " VERM "|       |  " NOR " |  |_/   /                |\n");
    printf("|                 |_______/     " VERM "|_______|  " NOR " |_______/                 |\n");
    printf("|                                                                     |\n");
    printf("|" AZU "                   Projeto e Analise de Algoritmos                   " NOR "|\n");
    printf("|                                                                     |\n");
    printf("| ._________________________________________________________________. |\n");
    printf("|'               " VERM "l    /\\ /     \\\\            \\ /\\   l" NOR "                `|\n");
    printf("*                " VERM "l  /   V       ))            V   \\ l" NOR "                 *\n");
    printf("                 " VERM "l/            //                  \\I" NOR "                  \n");
}

static const char *desenho(char tipo, int linha) {
    static const char *vazio[] = {
        "       ", 
        "       ", 
        "       ", 
        "       ", 
        "       "
    };
    static const char *parede[] = {
        " _____ ", 
        "|_|_|_|", 
        "||_|_||", 
        "|_|_|_|", 
        "||_|_||"
    };
    static const char *personagem[] = {
        "       ", 
        "   o   ", 
        "  /|\\  ", 
        "  / \\  ", 
        "       "
    };
    static const char *chave[] = {
        " ( * ) ",
        "  | |  ",
        "  | >  ",
        "  | >  ",
        "   V   "
    };
    static const char *bau[] = {
        "       ", 
        "  ___  ", 
        " /___\\ ", 
        "| <o> |", 
        "|_____|"
    };
    static const char *bixo[] = {
        " ( O ) ", 
        "__| |__", 
        "|| X ||", 
        "mn X nm", 
        " // \\\\ "
    };
    static const char *caminhoPassado[] = {
        "       ", 
        "\\  /   ",  
        " \\/    ",  
        " /\\    ",  
        "/  \\   " 
    };

    switch (tipo) {
        case '1': return parede[linha];
        case 'A': return personagem[linha];
        case 'C': return chave[linha];
        case 'X': return bau[linha];
        case 'M': return bixo[linha];
        case '2': return caminhoPassado[linha];
        default:  return vazio[linha];
    }
}

void printarMapa(char **mapa, int linhas, int colunas) {
    int i, j, linha;

    printf("\n----------+LABIRINTO+----------\n\n");

    for (i = 0; i < linhas; i++) {
        for (linha = 0; linha < 5; linha++) {
            for (j = 0; j < colunas; j++) {
                const char *figura = desenho(mapa[i][j], linha);

                if (mapa[i][j] == '1')
                    printf( "%s"  " ", figura);
                else if (mapa[i][j] == 'A')
                    printf( "%s"  " ", figura);
                else if (mapa[i][j] == 'C')
                    printf(ama "%s" NOR " ", figura);
                else if (mapa[i][j] == 'X')
                    printf(ama "%s" NOR " ", figura);
                else if (mapa[i][j] == 'M')
                    printf(VERM "%s" NOR " ", figura);
                else if (mapa[i][j] == '2')
                    printf(VERM "%s" NOR " ", figura);
                else
                    printf("%s ", figura);
            }
            printf("\n");
        }
        printf("\n");
    }
}