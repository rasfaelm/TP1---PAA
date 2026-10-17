#include <stdlib.h>
#include <string.h>
#include "labirinto.h"

static int alocar_grid(Labirinto *lab) {
    lab->grid = calloc(lab->linhas, sizeof(char *));
    if (!lab->grid) return 1;
    for (int i = 0; i < lab->linhas; i++) {
        lab->grid[i] = malloc(lab->colunas);
        if (!lab->grid[i]) {
            lab_liberar(lab);
            return 1;
        }
    }
    return 0;
}

void lab_liberar(Labirinto *lab) {
    if (!lab || !lab->grid) return;
    for (int i = 0; i < lab->linhas; i++) free(lab->grid[i]);
    free(lab->grid);
    lab->grid = NULL;
}

int lab_validar(Labirinto *lab) {
    int qtd_a = 0, qtd_x = 0, qtd_c = 0;

    if (lab->linhas <= 0 || lab->colunas <= 0) {
        fprintf(stderr, "Erro: dimensoes invalidas (%d x %d).\n", lab->linhas, lab->colunas);
        return 1;
    }
    if (lab->chaves_necessarias < 0) {
        fprintf(stderr, "Erro: quantidade de chaves negativa.\n");
        return 1;
    }

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            switch (lab->grid[i][j]) {
                case '0': case '1': break;
                case 'A': qtd_a++; lab->inicio_l = i; lab->inicio_c = j; break;
                case 'X': qtd_x++; lab->bau_l = i; lab->bau_c = j; break;
                case 'C': qtd_c++; break;
                default:
                    fprintf(stderr, "Erro: caractere invalido '%c' na posicao [%d,%d].\n",
                            lab->grid[i][j], i, j);
                    return 1;
            }
        }
    }

    if (qtd_a != 1) {
        fprintf(stderr, "Erro: deve haver exatamente um 'A' (encontrados %d).\n", qtd_a);
        return 1;
    }
    if (qtd_x != 1) {
        fprintf(stderr, "Erro: deve haver exatamente um 'X' (encontrados %d).\n", qtd_x);
        return 1;
    }
    if (qtd_c != lab->chaves_necessarias) {
        fprintf(stderr, "Erro: o bau exige %d chave(s), mas o mapa tem %d 'C'.\n",
                lab->chaves_necessarias, qtd_c);
        return 1;
    }
    lab->total_chaves = qtd_c;
    return 0;
}

/* Le o labirinto de um stream. Cada celula e um caractere nao-branco, entao
   funciona com "A 0 0 C" (espacado) e com "A00C" (colado). Se interativo != 0,
   exibe instrucoes e repete a leitura de uma linha invalida. */
static int ler_stream(FILE *f, Labirinto *lab, int interativo) {
    memset(lab, 0, sizeof(*lab));

    if (interativo) printf("Numero de linhas e de colunas (ex: 10 10): ");
    if (fscanf(f, "%d %d", &lab->linhas, &lab->colunas) != 2) {
        fprintf(stderr, "Erro: nao foi possivel ler as dimensoes.\n");
        return 1;
    }
    if (lab->linhas <= 0 || lab->colunas <= 0) {
        fprintf(stderr, "Erro: dimensoes invalidas (%d x %d).\n", lab->linhas, lab->colunas);
        return 1;
    }

    if (interativo) printf("Quantidade de chaves necessarias para abrir o bau: ");
    if (fscanf(f, "%d", &lab->chaves_necessarias) != 1) {
        fprintf(stderr, "Erro: nao foi possivel ler a quantidade de chaves.\n");
        return 1;
    }

    if (alocar_grid(lab)) {
        fprintf(stderr, "Erro: falta de memoria.\n");
        return 1;
    }

    if (interativo)
        printf("\nDigite cada linha do mapa (%d celulas por linha).\n"
               "  0 = sala vazia | 1 = parede | A = inicio | C = chave | X = bau\n"
               "  Pode separar por espaco ou digitar junto (ex: A00000000C).\n\n",
               lab->colunas);

    for (int i = 0; i < lab->linhas; i++) {
        if (interativo) printf("Linha %d: ", i);
        for (int j = 0; j < lab->colunas; j++) {
            char c;
            if (fscanf(f, " %c", &c) != 1) {
                fprintf(stderr, "Erro: fim inesperado ao ler a celula [%d,%d].\n", i, j);
                lab_liberar(lab);
                return 1;
            }
            lab->grid[i][j] = c;
        }
    }

    if (lab_validar(lab)) {
        lab_liberar(lab);
        return 1;
    }
    return 0;
}

int lab_ler_arquivo(const char *caminho, Labirinto *lab) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror(caminho);
        return 1;
    }
    int r = ler_stream(f, lab, 0);
    fclose(f);
    return r;
}

int lab_ler_manual(Labirinto *lab) {
    return ler_stream(stdin, lab, 1);
}

int lab_salvar(const Labirinto *lab, const char *caminho) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        perror(caminho);
        return 1;
    }
    fprintf(f, "%d %d\n%d\n", lab->linhas, lab->colunas, lab->chaves_necessarias);
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++)
            fprintf(f, "%c%c", lab->grid[i][j], j + 1 < lab->colunas ? ' ' : '\n');
    }
    fclose(f);
    return 0;
}

void lab_imprimir(const Labirinto *lab, FILE *saida) {
    fprintf(saida, "%d %d\n%d\n", lab->linhas, lab->colunas, lab->chaves_necessarias);
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++)
            fprintf(saida, "%c%c", lab->grid[i][j], j + 1 < lab->colunas ? ' ' : '\n');
    }
}
