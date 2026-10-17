#ifndef LABIRINTO_H
#define LABIRINTO_H

#include <stdio.h>

typedef struct {
    int linhas;
    int colunas;
    int chaves_necessarias;
    int total_chaves;
    int inicio_l, inicio_c;
    int bau_l, bau_c;
    char **grid;
} Labirinto;


/* Leitura a partir de arquivo texto no formato da especificacao. */
int lab_ler_arquivo(const char *caminho, Labirinto *lab);

/* Geracao manual: pede os dados no terminalpasso a passo. */
int lab_ler_manual(Labirinto *lab);

/* Salva o labirinto no formato da especificacao. */
int lab_salvar(const Labirinto *lab, const char *caminho);

/* Confere: dimensoes > 0, caracteres validos, exatamente um 'A' e um 'X',
   e total de 'C' igual a chaves_necessarias. Preenche inicio/bau/total. */
int lab_validar(Labirinto *lab);

void lab_imprimir(const Labirinto *lab, FILE *saida);
void lab_liberar(Labirinto *lab);

#endif
