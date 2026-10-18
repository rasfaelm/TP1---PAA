#include "GeradorDeMapa2.h"

char **GeraMapa(int totalChaves, int linhas, int colunas, int qtdMonstros){
    char **mapa;
    int elemento, aventureiro = 0, chaves = 0, saida = 0, monstros = 0, linhaAtual, colunaAtual;
    for(int i = 0; i < linhas; i++){
        for(int k = 0; k < colunas; k++){
            elemento = rand() % 2;
            if(elemento == 0) mapa[i][k] = "0";
            else mapa[i][k] = "1";
        }
    }
    while(aventureiro == 0){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] =="0" || mapa[linhaAtual][colunaAtual] == "1"){
            mapa[linhaAtual][colunaAtual] = "A";
            aventureiro = 1;
        }
    }
    while(chaves < totalChaves){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == "0" || mapa[linhaAtual][colunaAtual] == "1"){
            mapa[linhaAtual][colunaAtual] = "C";
            chaves += 1;
        }
    }
    while(saida == 0){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == "0" || mapa[linhaAtual][colunaAtual] == "1"){
            mapa[linhaAtual][colunaAtual] = "S";
            saida = 1;
        }
    }
    while(monstros < qtdMonstros){
        linhaAtual = rand() % linhas;
        colunaAtual = rand() % colunas;
        if(mapa[linhaAtual][colunaAtual] == "0" || mapa[linhaAtual][colunaAtual] == "1"){
            mapa[linhaAtual][colunaAtual] = "M";
            aventureiro = 1;
        }
    }

    return mapa;
}