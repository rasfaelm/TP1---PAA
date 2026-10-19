#include "GeradorDeMapa2.h"

char **GeraMapa2(int totalChaves, int linhas, int colunas, int qtdMonstros){
    char **mapa = (char **)malloc(linhas * sizeof(char *));
    for(int i = 0; i < linhas; i++){
        mapa[i] = (char *)malloc(colunas * sizeof(char));
    }
    char  caminho[3] = "0", parede[3] = "1", A[3] = "A", C[3] = "C", S[3] = "S", M[3] = "M";
    int elemento, aventureiro = 0, chaves = 0, saida = 0, monstros = 0, linhaAtual, colunaAtual;
    for(int i = 0; i < linhas; i++){
        for(int k = 0; k < colunas; k++){
            elemento = rand() % 2;
            if(elemento == 0) mapa[i][k] = caminho[0];
            else mapa[i][k] = parede[0];
        }
    }
    while(aventureiro == 0){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == caminho[0] || mapa[linhaAtual][colunaAtual] == parede[0]){
            mapa[linhaAtual][colunaAtual] = A[0];
            aventureiro = 1;
        }
    }
    while(chaves < totalChaves){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == caminho[0] || mapa[linhaAtual][colunaAtual] == parede[0]){
            mapa[linhaAtual][colunaAtual] = C[0];
            chaves += 1;
        }
    }
    while(saida == 0){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == caminho[0] || mapa[linhaAtual][colunaAtual] == parede[0]){
            mapa[linhaAtual][colunaAtual] = S[0];
            saida = 1;
        }
    }
    while(monstros < qtdMonstros){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == caminho[0] || mapa[linhaAtual][colunaAtual] == parede[0]){
            mapa[linhaAtual][colunaAtual] = M[0];
            monstros += 1;
        }
    }

    return mapa;
}