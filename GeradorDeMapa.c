#include "GeradorDeMapa.h"

#define CAMINHO '0'
#define PAREDE  '1'
#define AVENTUREIRO 'A'
#define CHAVE   'C'
#define SAIDA     'X'

char **GeraMapa(int totalChaves, int linhas, int colunas) {
    if (linhas <= 0 || colunas <= 0 || totalChaves < 0 || totalChaves > linhas * colunas - 2) {
        return NULL;
    }
    char **mapa = malloc(linhas * sizeof(char *));
    if (mapa == NULL) {
        return NULL;
    }
    for (int i = 0; i < linhas; i++) {
        mapa[i] = malloc(colunas * sizeof(char));
        if (mapa[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(mapa[j]);
            }
            free(mapa);
            return NULL;
        }
    }
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            mapa[i][j] = (rand() % 2 == 0) ? CAMINHO : PAREDE;
        }
    }
    int linha, coluna;
    do {
        linha = rand() % linhas;
        coluna = rand() % colunas;
    } while (mapa[linha][coluna] != CAMINHO && mapa[linha][coluna] != PAREDE);
    mapa[linha][coluna] = AVENTUREIRO;
    for (int i = 0; i < totalChaves; i++) {
        do {
            linha = rand() % linhas;
            coluna = rand() % colunas;
        } while (mapa[linha][coluna] != CAMINHO && mapa[linha][coluna] != PAREDE);
        mapa[linha][coluna] = CHAVE;
    }
    do {
        linha = rand() % linhas;
        coluna = rand() % colunas;
    } while (mapa[linha][coluna] != CAMINHO && mapa[linha][coluna] != PAREDE);
    mapa[linha][coluna] = SAIDA;
    return mapa;
}