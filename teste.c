
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "GeradorDeMapa.h"
#include "Gamemod1.h"

#define LINHAS 10
#define COLUNAS 10

int main(void) {
    srand(time(NULL));

    char **mapa = GeraMapa(2, LINHAS, COLUNAS);

    if (mapa == NULL) {
        printf("Erro ao gerar o mapa.\n");
        return 1;
    }

    int x = iniciarModo1(mapa, LINHAS, COLUNAS);

    if(x == 0){
        printf("Vitoria");
    }else{
        printf("Derrota");
    }

    /* Libera a memória alocada. */
    for (int i = 0; i < LINHAS; i++) {
        free(mapa[i]);
    }

    free(mapa);

    return 0;
}
