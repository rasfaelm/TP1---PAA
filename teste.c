#include <stdio.h>
#include "labirinto.h"
#include "Gamemod2.h"

int main(void) {
    Labirinto lab;

    if (lab_ler_arquivo("Entrada2.txt", &lab) != 0) {
        printf("Erro ao carregar o arquivo do mapa.\n");
        return 1;
    }

    int resultado = iniciarModo2(lab.grid, lab.linhas, lab.colunas);

    if (resultado == 1) {
        printf("Vitoria!\n");
    } else {
        printf("Derrota ou jogo encerrado.\n");
    }

    lab_liberar(&lab);

    return 0;
}