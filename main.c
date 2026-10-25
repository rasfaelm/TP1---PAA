#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "labirinto.h"
#include "Busca.h"
#include "Gamemod1.h"
#include "Gamemod2.h"
#include "GeradorDeMapa.h"
#include "Animacao.h"
#include "arte.h"
#include "Interface.h"

#ifdef ANALISE
static int modo_analise = 1;
#else
static int modo_analise = 0;
#endif

static int ler_int(const char *msg, int *v) {
    printf("%s", msg);
    return scanf("%d", v) == 1;
}

/* Copia o grid, pois o modo 1 remove as chaves do mapa durante o jogo. */
static char **copiar_grid(const Labirinto *lab) {
    char **g = malloc(lab->linhas * sizeof(char *));
    if (!g) return NULL;
    for (int i = 0; i < lab->linhas; i++) {
        g[i] = malloc(lab->colunas);
        if (!g[i]) {
            while (--i >= 0) free(g[i]);
            free(g);
            return NULL;
        }
        memcpy(g[i], lab->grid[i], lab->colunas);
    }
    return g;
}

static void liberar_grid(char **g, int linhas) {
    for (int i = 0; i < linhas; i++) free(g[i]);
    free(g);
}

static void resolver(const Labirinto *lab) {
    ResultadoBusca res;
    if (busca_resolver(lab, &res)) {
        fprintf(stderr, "Erro: falta de memoria.\n");
        return;
    }
    printf("\n");
    busca_imprimir_caminho(&res, stdout);
    if (res.encontrou) {
        printf("\nCaminho no mapa (* = percurso):\n");
        busca_imprimir_mapa(lab, &res, stdout);
    }
    if (modo_analise) {
        printf("\n");
        busca_imprimir_analise(&res, stdout);
    }
    busca_liberar(&res);
}

static void animar(const Labirinto *lab) {
    ResultadoBusca res;
    if (busca_resolver(lab, &res)) {
        fprintf(stderr, "Erro: falta de memoria.\n");
        return;
    }
    if (res.encontrou) animar_caminho(lab, &res, ANIMACAO_DELAY_MS);
    else busca_imprimir_caminho(&res, stdout);
    busca_liberar(&res);
}

static int carregar_arquivo(Labirinto *lab) {
    char nome[256];
    printf("Arquivo do mapa: ");
    if (scanf("%255s", nome) != 1) return 1;
    return lab_ler_arquivo(nome, lab);
}

static int gerar_mapa(Labirinto *lab) {
    int l, c, k;
    if (!ler_int("Linhas: ", &l) || !ler_int("Colunas: ", &c) ||
        !ler_int("Chaves: ", &k)) return 1;

    char **g = GeraMapa(k, l, c);
    if (!g) {
        fprintf(stderr, "Erro: parametros invalidos para gerar o mapa.\n");
        return 1;
    }
    memset(lab, 0, sizeof(*lab));
    lab->linhas = l;
    lab->colunas = c;
    lab->chaves_necessarias = k;
    lab->grid = g;
    if (lab_validar(lab)) {
        lab_liberar(lab);
        return 1;
    }
    return 0;
}

static void jogar(const Labirinto *lab, int modo) {
    char **copia = copiar_grid(lab);
    if (!copia) {
        fprintf(stderr, "Erro: falta de memoria.\n");
        return;
    }
    int venceu = (modo == 1) ? iniciarModo1(copia, lab->linhas, lab->colunas)
                             : iniciarModo2(copia, lab->linhas, lab->colunas);
    printf(venceu ? "Vitoria!\n" : "Derrota ou jogo encerrado.\n");
    liberar_grid(copia, lab->linhas);
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++)
        if (strcmp(argv[i], "--analise") == 0) modo_analise = 1;

    srand((unsigned)time(NULL));

    Labirinto lab;
    int tem_mapa = 0;
    int op;

    do {
        printf("\n===== LABIRINTO DAS CHAVES %s=====\n", modo_analise ? "[ANALISE] " : "");
        printf("Mapa atual: %s\n", tem_mapa ? "carregado" : "nenhum");
        printf("1) Carregar mapa de arquivo\n");
        printf("2) Digitar mapa manualmente\n");
        printf("3) Gerar mapa aleatorio\n");
        printf("4) Resolver (backtracking)\n");
        printf("5) Jogar - modo manual\n");
        printf("6) Jogar - modo monstro\n");
        printf("7) Salvar mapa em arquivo\n");
        printf("8) Animar caminho da solucao\n");
        printf("9) Desenho alternativo do mapa\n");
        printf("0) Sair\n> ");
        if (scanf("%d", &op) != 1) break;

        if (op >= 1 && op <= 3) {
            Labirinto novo;
            int erro = (op == 1) ? carregar_arquivo(&novo)
                     : (op == 2) ? lab_ler_manual(&novo)
                                 : gerar_mapa(&novo);
            if (!erro) {
                if (tem_mapa) lab_liberar(&lab);
                lab = novo;
                tem_mapa = 1;
                printf("\n");
                lab_imprimir(&lab, stdout);
            }
        } else if (op >= 4 && op <= 8) {
            if (!tem_mapa) {
                printf("Carregue ou gere um mapa primeiro.\n");
            } else if (op == 4) {
                resolver(&lab);
            } else if (op == 5 || op == 6) {
                jogar(&lab, op - 4); 
            } else if (op == 8) {
                animar(&lab);
            } else {
                char nome[256];
                printf("Nome do arquivo: ");
                if (scanf("%255s", nome) == 1 && lab_salvar(&lab, nome) == 0)
                    printf("Salvo em %s.\n", nome);
            }
        } else if (op == 9) {
                menu();
                printarMapa(lab.grid, lab.linhas, lab.colunas); 
        } else if (op != 0) {
            printf("Opcao invalida.\n");
        }
    } while (op != 0);

    if (tem_mapa) lab_liberar(&lab);
    return 0;
}