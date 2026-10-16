
#include <stdio.h>
#include "gamemod1.h"
#include "gamemod2.h"

#define LINHAS 10
#define COLUNAS 10

int main(void) {
    char linha0[] = {'A','0','0','0','0','0','0','0','0','C','\0'};
    char linha1[] = {'0','1','1','1','1','0','1','1','1','0','\0'};
    char linha2[] = {'0','0','0','0','0','0','0','0','1','0','\0'};
    char linha3[] = {'0','1','0','1','0','1','0','0','1','0','\0'};
    char linha4[] = {'0','1','0','1','0','1','0','1','1','0','\0'};
    char linha5[] = {'0','1','0','1','0','1','0','0','0','0','\0'};
    char linha6[] = {'1','1','0','1','0','1','0','0','1','0','\0'};
    char linha7[] = {'0','0','0','1','0','1','0','0','1','0','\0'};
    char linha8[] = {'C','1','1','1','0','1','1','1','1','1','\0'};
    char linha9[] = {'0','0','0','0','0','0','0','0','0','X','\0'};

    char *mapa[LINHAS] = {
        linha0,
        linha1,
        linha2,
        linha3,
        linha4,
        linha5,
        linha6,
        linha7,
        linha8,
        linha9
    };

    printf("====================================\n");
    printf("       LABIRINTO\n");
    printf("====================================\n");
    printf("Colete as duas chaves (C) e alcance X.\n");
    printf("Use W, A, S, D para se movimentar.\n");
    printf("Pressione Q para sair.\n");
    printf("\nPressione qualquer tecla para comecar...");

    getchar();

    int resultado = iniciarModo2(mapa, LINHAS, COLUNAS);
  
    if (resultado) {
        printf("\nVoce venceu o labirinto!\n");
    } else {
        printf("\nPerdeu\n");
    }

    return 0;
}
