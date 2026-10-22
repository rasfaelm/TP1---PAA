#ifndef BUSCA_H
#define BUSCA_H

#include <stdio.h>
#include "labirinto.h"

typedef struct {
    int l, c;
} Celula;

typedef struct {
    int encontrou; /* 1 se existe caminho válido */
    Celula *caminho; /* sequencia de celulas de A ate X */
    int tamanho;  /* quantidade de celulas do caminho */
    long chamadas_recursivas; /* modo análise */
    int nivel_max; /* modo análise: maior profundidade da recursao */
} ResultadoBusca;

/* Resolve o labirinto por backtracking. Nao altera lab->grid.
   Retorna 0 em sucesso (mesmo sem solucao) e 1 em erro de memoria. */
int busca_resolver(const Labirinto *lab, ResultadoBusca *res);

/* Imprime "[l,c],[l,c],...". */
void busca_imprimir_caminho(const ResultadoBusca *res, FILE *saida);

void busca_imprimir_mapa(const Labirinto *lab, const ResultadoBusca *res, FILE *saida);

void busca_imprimir_analise(const ResultadoBusca *res, FILE *saida);

void busca_liberar(ResultadoBusca *res);

#endif
