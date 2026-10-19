#include "gamemod1.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

typedef struct {
    int linha;
    int coluna;
    int chavesColetadas;
    int totalChaves;
} JogadorManual;

/* Leitura de uma tecla sem exigir Enter. */
static int lerTecla(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios antigo, novo;
    int tecla;

    if (tcgetattr(STDIN_FILENO, &antigo) == -1)
        return -1;

    novo = antigo;
    novo.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &novo) == -1)
        return -1;

    tecla = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &antigo);
    return tecla;
#endif
}

/* Limpa o terminal. */
static void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/* Procura a entrada e conta as chaves do mapa. */
static int inicializarJogador(char **mapa, int linhas, int colunas, JogadorManual *jogador) {
    int encontrouEntrada = 0;

    jogador->linha = -1;
    jogador->coluna = -1;
    jogador->chavesColetadas = 0;
    jogador->totalChaves = 0;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (mapa[i][j] == 'A') {
                if (encontrouEntrada)
                    return 0;

                jogador->linha = i;
                jogador->coluna = j;
                encontrouEntrada = 1;
            }

            if (mapa[i][j] == 'C')
                jogador->totalChaves++;
        }
    }

    return encontrouEntrada;
}

/* Desenha o mapa sem modificar sua matriz original. */
static void desenharMapa(char **mapa, int linhas, int colunas, const JogadorManual *jogador) {
    limparTela();

    printf("=== MODO MANUAL ===\n\n");

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (i == jogador->linha &&
                j == jogador->coluna) {
                printf("P ");
            } else {
                printf("%c ", mapa[i][j]);
            }
        }
        printf("\n");
    }

    printf("\nChaves: %d/%d\n", jogador->chavesColetadas, jogador->totalChaves);
    printf("W: cima | A: esquerda | ");
    printf("S: baixo | D: direita | Q: sair\n");
}

/* Tenta mover o personagem em uma direcao. */
static int movimentarJogador(char **mapa, int linhas, int colunas, JogadorManual *jogador, int tecla) {
    int novaLinha = jogador->linha;
    int novaColuna = jogador->coluna;

    switch (tolower((unsigned char)tecla)) {
        case 'w':
            novaLinha--;
            break;
        case 's':
            novaLinha++;
            break;
        case 'a':
            novaColuna--;
            break;
        case 'd':
            novaColuna++;
            break;
        default:
            return 0;
    }

    /*
     * Ao sair por uma borda, o jogador reaparece na borda oposta,
     * na mesma linha ou coluna. O movimento ainda depende de a
     * celula de destino ser transitavel.
     */
    if (novaLinha < 0)
        novaLinha = linhas - 1;
    else if (novaLinha >= linhas)
        novaLinha = 0;

    if (novaColuna < 0)
        novaColuna = colunas - 1;
    else if (novaColuna >= colunas)
        novaColuna = 0;

    char destino = mapa[novaLinha][novaColuna];

    /* Paredes nao podem ser atravessadas. */
    if (destino == '1')
        return 0;

    /* A saida so pode ser acessada apos todas as chaves. */
    if (destino == 'X' && jogador->chavesColetadas < jogador->totalChaves) {
        return 0;
    }

    jogador->linha = novaLinha;
    jogador->coluna = novaColuna;

    /* Cada chave coletada e removida do mapa. */
    if (destino == 'C') {
        mapa[novaLinha][novaColuna] = '0';
        jogador->chavesColetadas++;
    }

    /* Retorna 1 quando o personagem alcanca a saida. */
    return destino == 'X';
}

int iniciarModo1(char **mapa, int linhas, int colunas) {
    if (mapa == NULL || linhas <= 0 || colunas <= 0)
        return 0;

    JogadorManual jogador;

    if (!inicializarJogador(mapa, linhas, colunas, &jogador)) {
        printf("Erro: mapa sem entrada A valida.\n");
        return 0;
    }

    int venceu = 0;

    while (!venceu) {
        desenharMapa(mapa, linhas, colunas, &jogador);

        int tecla = lerTecla();

        if (tecla == -1)
            return 0;

        if (tolower((unsigned char)tecla) == 'q')
            break;

        venceu = movimentarJogador(
            mapa, linhas, colunas, &jogador, tecla);

        if (!venceu)
            printf("\nMovimento invalido ou realizado.\n");
    }

    limparTela();

    if (venceu) {
        return 1;
    }

    return 0;
}