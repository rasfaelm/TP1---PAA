#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Animacao.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#endif

#define EMO_PASSOU  "🟥"
#define EMO_PAREDE  "⬛"
#define EMO_VAZIO   "⬜"
#define EMO_CHAVE   "🔑"
#define EMO_BAU     "🎁"
#define EMO_INICIO  "🟩"

static void esperar_ms(int ms) {
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    struct timespec ts = { ms / 1000, (long)(ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
#endif
}

/* Windows: console em UTF-8 e com suporte a sequencias ANSI. */
static void preparar_terminal(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    if (GetConsoleMode(h, &modo))
        SetConsoleMode(h, modo | 0x0004); /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
#endif
}

/* Desenha o mapa; passou[i*colunas+j] = 1 pinta a sala de vermelho. */
static void desenhar(const Labirinto *lab, const char *passou) {
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            const char *emo;
            if (passou[i * lab->colunas + j]) {
                emo = EMO_PASSOU;
            } else {
                switch (lab->grid[i][j]) {
                    case '1': emo = EMO_PAREDE; break;
                    case 'C': emo = EMO_CHAVE;  break;
                    case 'X': emo = EMO_BAU;    break;
                    case 'A': emo = EMO_INICIO; break;
                    default:  emo = EMO_VAZIO;  break;
                }
            }
            fputs(emo, stdout);
        }
        fputc('\n', stdout);
    }
    fflush(stdout);
}

void animar_caminho(const Labirinto *lab, const ResultadoBusca *res, int delay_ms) {
    if (!res->encontrou) {
        printf("Nao ha caminho para animar.\n");
        return;
    }

    char *passou = calloc((size_t)lab->linhas * lab->colunas, 1);
    if (!passou) {
        fprintf(stderr, "Erro: falta de memoria.\n");
        return;
    }

    preparar_terminal();
    printf("\n");
    desenhar(lab, passou);

    for (int k = 0; k < res->tamanho; k++) {
        esperar_ms(delay_ms);
        passou[res->caminho[k].l * lab->colunas + res->caminho[k].c] = 1;
        printf("\033[%dA", lab->linhas);   /* sobe o cursor para o inicio do mapa */
        desenhar(lab, passou);
    }

    printf("\nPercurso concluido: %d passos.\n", res->tamanho);
    free(passou);
}