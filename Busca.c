#include <stdlib.h>
#include <string.h>
#include "Busca.h"

typedef struct {
    const Labirinto *lab;
    char *visitado;          /* linhas*colunas, 1 = sala ja esta no caminho atual */
    Celula *caminho;         /* caminho atual */
    ResultadoBusca *res;
} Contexto;

static const int DL[4] = {-1, 1, 0, 0};
static const int DC[4] = {0, 0, -1, 1};

/*
 * Backtracking.
 *  (l,c)   : sala onde o aventureiro ja tá
 *  chaves  : chaves coletadas ate aqui, contando a sala atual
 *  nivel   : posicao no caminho (1 = sala inicial)
 *  retorna 1 ao achar o bau com todas as chaves; 0 caso contrario.
 */
static int explorar(Contexto *ctx, int l, int c, int chaves, int nivel) {
    const Labirinto *lab = ctx->lab;

    ctx->res->chamadas_recursivas++;
    if (nivel > ctx->res->nivel_max) ctx->res->nivel_max = nivel;

    /* escolhe a sala: marca e entra no caminho */
    ctx->visitado[l * lab->colunas + c] = 1;
    ctx->caminho[nivel - 1] = (Celula){l, c};

    if (lab->grid[l][c] == 'C') chaves++;

    if (lab->grid[l][c] == 'X') {
        /* Bau: so abre com todas as chaves... Se n é beco sem saída. */
        if (chaves == lab->chaves_necessarias) {
            ctx->res->tamanho = nivel;
            return 1;
        }
    } else {
        for (int d = 0; d < 4; d++) {
            int nl = l + DL[d];
            int nc = c + DC[d];

            if (nl < 0 || nl >= lab->linhas || nc < 0 || nc >= lab->colunas) continue;
            if (lab->grid[nl][nc] == '1') continue;                    /* parede */
            if (ctx->visitado[nl * lab->colunas + nc]) continue;      /* ja visitada */

            if (explorar(ctx, nl, nc, chaves, nivel + 1)) return 1;
        }
    }

    ctx->visitado[l * lab->colunas + c] = 0;
    return 0;
}

int busca_resolver(const Labirinto *lab, ResultadoBusca *res) {
    size_t total = (size_t)lab->linhas * lab->colunas;
    Contexto ctx;

    memset(res, 0, sizeof(*res));

    ctx.lab = lab;
    ctx.res = res;
    ctx.visitado = calloc(total, 1);
    ctx.caminho = malloc(total * sizeof(Celula));
    if (!ctx.visitado || !ctx.caminho) {
        free(ctx.visitado);
        free(ctx.caminho);
        return 1;
    }

    res->encontrou = explorar(&ctx, lab->inicio_l, lab->inicio_c, 0, 1);

    if (res->encontrou) {
        res->caminho = ctx.caminho;   /* o resultado vira dono do vetor */
    } else {
        free(ctx.caminho);
        res->tamanho = 0;
    }
    free(ctx.visitado);
    return 0;
}

void busca_imprimir_caminho(const ResultadoBusca *res, FILE *saida) {
    if (!res->encontrou) {
        fprintf(saida, "Não há um caminho possível para a entrada de dados fornecida\n");
        return;
    }
    for (int i = 0; i < res->tamanho; i++)
        fprintf(saida, "[%d,%d]%s", res->caminho[i].l, res->caminho[i].c,
                i + 1 < res->tamanho ? "," : "\n");
}

void busca_imprimir_mapa(const Labirinto *lab, const ResultadoBusca *res, FILE *saida) {
    if (!res->encontrou) return;

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            char ch = lab->grid[i][j];
            if (ch == '0') {
                for (int k = 0; k < res->tamanho; k++)
                    if (res->caminho[k].l == i && res->caminho[k].c == j) { ch = '*'; break; }
            }
            fprintf(saida, "%c%c", ch, j + 1 < lab->colunas ? ' ' : '\n');
        }
    }
}

void busca_imprimir_analise(const ResultadoBusca *res, FILE *saida) {
    fprintf(saida, "[ANALISE] Chamadas recursivas: %ld\n", res->chamadas_recursivas);
    fprintf(saida, "[ANALISE] Nivel maximo de recursao: %d\n", res->nivel_max);
}

void busca_liberar(ResultadoBusca *res) {
    free(res->caminho);
    res->caminho = NULL;
}