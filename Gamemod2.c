#include "gamemod2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
    #include <sys/time.h>
#endif

#define STAMINA_MAX 3
#define REGENERACAO_MS 1000.0
#define MOVIMENTO_BASE_MS 200.0
#define MONSTRO_LENTO_MS 3000.0
#define MONSTRO_RAPIDO_MS 750.0
#define DISTANCIA_PERSEGUICAO 3

#ifndef _WIN32
static struct termios termOrig;
static int termConfig = 0;
#endif

typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct {
    Posicao jogador;
    Posicao monstro;
    Posicao saida;
    int stamina;
    int chaves;
    int qtdChaves;
} EstadoJogo;

/* Limpa o terminal. Usada somente na inicializacao do jogo. */
static void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/* Posiciona o cursor sem apagar o conteudo da tela. Coordenadas iniciam em 1. */
static void posicionarCursor(int linha, int coluna) {
#ifdef _WIN32
    COORD posicao;
    posicao.X = (SHORT)(coluna - 1);
    posicao.Y = (SHORT)(linha - 1);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicao);
#else
    printf("\033[%d;%dH", linha, coluna);
#endif
}

/* Retorna o tempo atual em milissegundos. */
static double tempoAtualMs(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* Configura o terminal para receber teclas sem Enter. */
static int iniciarTerminal(void) {
#ifdef _WIN32
    return 1;
#else
    struct termios novo;

    if (tcgetattr(STDIN_FILENO, &termOrig) == -1)
        return 0;

    novo = termOrig;
    novo.c_lflag &= ~(ICANON | ECHO);
    novo.c_cc[VMIN] = 0;
    novo.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &novo) == -1)
        return 0;

    termConfig = 1;
    return 1;
#endif
}

/* Restaura o terminal ao estado original. */
static void encerrarTerminal(void) {
#ifndef _WIN32
    if (termConfig) {
        tcsetattr(STDIN_FILENO, TCSANOW, &termOrig);
        termConfig = 0;
    }
#endif
}

/* Retorna uma tecla disponível ou -1 se não houver nenhuma. */
static int lerTecla(void) {
#ifdef _WIN32
    if (_kbhit())
        return _getch();

    return -1;
#else
    fd_set conjunto;
    struct timeval espera = {0, 0};

    FD_ZERO(&conjunto);
    FD_SET(STDIN_FILENO, &conjunto);

    if (select(STDIN_FILENO + 1, &conjunto, NULL,
               NULL, &espera) > 0) {
        unsigned char tecla;

        if (read(STDIN_FILENO, &tecla, 1) == 1)
            return tecla;
    }

    return -1;
#endif
}

/* Pausa curta para evitar uso excessivo da CPU. */
static void esperarCiclo(void) {
#ifdef _WIN32
    Sleep(10);
#else
    struct timeval espera = {0, 10000};
    select(0, NULL, NULL, NULL, &espera);
#endif
}

static int distancia(Posicao a, Posicao b) {
    int dl = a.linha - b.linha;
    int dc = a.coluna - b.coluna;

    if (dl < 0) dl = -dl;
    if (dc < 0) dc = -dc;

    return dl + dc;
}

/* Distancia considerando a conexao entre bordas do mapa global. */
static int distanciaGlobal(Posicao a, Posicao b, int linhas, int colunas) {
    int dl = abs(a.linha - b.linha);
    int dc = abs(a.coluna - b.coluna);
    int vertical = linhas - dl;
    int horizontal = colunas - dc;

    if (vertical < dl) dl = vertical;
    if (horizontal < dc) dc = horizontal;

    return dl + dc;
}

static int dentroDoMapa(int l, int c, int linhas, int colunas) {
    return l >= 0 && l < linhas && c >= 0 && c < colunas;
}

static int ehParede(char **mapa, int l, int c) {
    return mapa[l][c] == '1';
}

/* Conecta as bordas opostas: permite atravessar somente se a celula
   correspondente do outro lado nao for parede. */
static int ajustarBorda(char **mapa, int *l, int *c, int linhas, int colunas) {
    if (*l < 0) *l = linhas - 1;
    else if (*l >= linhas) *l = 0;

    if (*c < 0) *c = colunas - 1;
    else if (*c >= colunas) *c = 0;

    return !ehParede(mapa, *l, *c);
}

/* Cria uma cópia para não modificar o mapa original. */
static char **copiarMapa(char **mapa, int linhas, int colunas) {
    char **copia = malloc((size_t)linhas * sizeof(char *));

    if (copia == NULL)
        return NULL;

    for (int i = 0; i < linhas; i++) {
        copia[i] = malloc((size_t)colunas + 1);

        if (copia[i] == NULL) {
            for (int j = 0; j < i; j++)
                free(copia[j]);

            free(copia);
            return NULL;
        }

        memcpy(copia[i], mapa[i], (size_t)colunas);
        copia[i][colunas] = '\0';
    }

    return copia;
}

static void liberarMapa(char **mapa, int linhas) {
    if (mapa == NULL)
        return;

    for (int i = 0; i < linhas; i++)
        free(mapa[i]);

    free(mapa);
}

/* Procura a entrada, saída e quantidade de chaves. */
static int inicializarEstado(char **mapa, int linhas, int colunas, EstadoJogo *jogo) {
    int qtdEntradas = 0;
    int qtdSaidas = 0;

    jogo->stamina = STAMINA_MAX;
    jogo->chaves = 0;
    jogo->qtdChaves = 0;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (mapa[i][j] == 'A') {
                jogo->jogador = (Posicao){i, j};
                mapa[i][j] = '0';
                qtdEntradas++;
            } else if (mapa[i][j] == 'X') {
                jogo->saida = (Posicao){i, j};
                qtdSaidas++;
            } else if (mapa[i][j] == 'C') {
                jogo->qtdChaves++;
            }
        }
    }

    return qtdEntradas == 1 && qtdSaidas == 1;
}

/*
 * Escolhe uma posição livre intermediária entre o jogador e a saída.
 * Só considera células '0'. Entre as melhores posições, escolhe uma
 * aleatoriamente.
 */
static int posicionarMonstro(char **mapa, int linhas, int colunas, EstadoJogo *jogo) {
    int melhorScore = 2147483647;
    int qtd = 0;
    int cap = linhas * colunas;

    Posicao *candidatas = malloc((size_t)cap * sizeof(Posicao));

    if (candidatas == NULL)
        return 0;

    int distTotal = distanciaGlobal(jogo->jogador, jogo->saida, linhas, colunas);
    int distAlvo = distTotal / 2;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (mapa[i][j] != '0')
                continue;

            Posicao p = {i, j};
            int dj = distanciaGlobal(p, jogo->jogador, linhas, colunas);
            int ds = distanciaGlobal(p, jogo->saida, linhas, colunas);

            if (dj < 2 || ds < 2)
                continue;

            int score = abs(dj - distAlvo) + abs(ds - distAlvo);

            if (score < melhorScore) {
                melhorScore = score;
                qtd = 0;
            }

            if (score == melhorScore)
                candidatas[qtd++] = p;
        }
    }

    if (qtd == 0) {
        free(candidatas);
        return 0;
    }

    jogo->monstro = candidatas[rand() % qtd];
    free(candidatas);

    return 1;
}

static void desenharMapa(char **mapa, int linhas, int colunas, const EstadoJogo *jogo) {
    limparTela();

    printf("========== LABIRINTO ==========\n\n");

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (i == jogo->jogador.linha && j == jogo->jogador.coluna) {
                printf("P ");
            } else if (i == jogo->monstro.linha && j == jogo->monstro.coluna) {
                printf("M ");
            } else {
                printf("%c ", mapa[i][j]);
            }
        }

        printf("\n");
    }

    printf("\nChaves: %d/%d\n", jogo->chaves, jogo->qtdChaves);
    printf("Stamina: [");

    for (int i = 0; i < STAMINA_MAX; i++)
        printf("%s", i < jogo->stamina ? "#" : "-");

    printf("] %d/%d\n", jogo->stamina, STAMINA_MAX);
    printf("Distancia do monstro: %d casas\n", distancia(jogo->jogador, jogo->monstro));
    printf("\nW/A/S/D: mover | Q: sair\n");
    printf("Cada movimento consome 1 carga de stamina.\n");
    printf("Uma carga recuperada por segundo.\n");

    fflush(stdout);
}

/* Desenha apenas uma celula do mapa. O mapa comeca na linha 3 do terminal. */
static void desenharCelula(int linha, int coluna, char simbolo) {
    posicionarCursor(3 + linha, 1 + coluna * 2);
    printf("%c ", simbolo);
    fflush(stdout);
}

/* Restaura uma celula usando o caractere atual do mapa. */
static void restaurarCelula(char **mapa, int linha, int coluna) {
    desenharCelula(linha, coluna, mapa[linha][coluna]);
}

/* Atualiza uma linha do painel sem limpar a tela inteira. */
static void atualizarLinhaPainel(int linha, const char *texto) {
    posicionarCursor(linha, 1);
    printf("%-100s", texto);
    fflush(stdout);
}

/* Atualiza somente os indicadores do painel que mudaram. */
static void atualizarPainel(int linhas, const EstadoJogo *jogo, int distMon, int updChaves, int updStam, int updDist) {
    char texto[128];
    int linhaChaves = linhas + 4;

    if (updChaves) {
        snprintf(texto, sizeof(texto), "Chaves: %d/%d", jogo->chaves, jogo->qtdChaves);
        atualizarLinhaPainel(linhaChaves, texto);
    }

    if (updStam) {
        char barra[STAMINA_MAX + 1];

        for (int i = 0; i < STAMINA_MAX; i++)
            barra[i] = i < jogo->stamina ? '#' : '-';
        barra[STAMINA_MAX] = '\0';

        snprintf(texto, sizeof(texto), "Stamina: [%s] %d/%d", barra, jogo->stamina, STAMINA_MAX);
        atualizarLinhaPainel(linhaChaves + 1, texto);
    }

    if (updDist) {
        snprintf(texto, sizeof(texto), "Distancia do monstro: %d casas", distMon);
        atualizarLinhaPainel(linhaChaves + 2, texto);
    }
}

/* Tenta mover o jogador uma casa. */
static int moverJogador(char **mapa, int linhas, int colunas, EstadoJogo *jogo, int tecla) {
    int nl = jogo->jogador.linha;
    int nc = jogo->jogador.coluna;

    switch (tolower((unsigned char)tecla)) {
        case 'w': nl--; break;
        case 's': nl++; break;
        case 'a': nc--; break;
        case 'd': nc++; break;
        default: return 0;
    }

    /* Se ultrapassar uma borda, tenta entrar pela borda oposta.
       A travessia so e permitida se a celula de destino nao for parede. */
    if (!dentroDoMapa(nl, nc, linhas, colunas)) {
        if (!ajustarBorda(mapa, &nl, &nc, linhas, colunas))
            return 0;
    }

    if (ehParede(mapa, nl, nc))
        return 0;

    if (jogo->stamina <= 0)
        return 0;

    char destino = mapa[nl][nc];

    if (destino == 'X' && jogo->chaves < jogo->qtdChaves)
        return 0;

    /* Entrar na célula do monstro causa derrota. */
    if (nl == jogo->monstro.linha && nc == jogo->monstro.coluna)
        return -1;

    jogo->jogador.linha = nl;
    jogo->jogador.coluna = nc;
    jogo->stamina--;

    if (destino == 'C') {
        mapa[nl][nc] = '0';
        jogo->chaves++;
    }

    return destino == 'X' ? 2 : 1;
}

/*
 * BFS do monstro até o jogador.
 * Retorna 1 se conseguiu mover, 0 se não existe caminho.
 */
static int moverMonstro(char **mapa, int linhas, int colunas, EstadoJogo *jogo) {
    int total = linhas * colunas;
    int *pais = malloc((size_t)total * sizeof(int));
    int *fila = malloc((size_t)total * sizeof(int));

    if (pais == NULL || fila == NULL) {
        free(pais);
        free(fila);
        return 0;
    }

    for (int i = 0; i < total; i++)
        pais[i] = -1;

    int inicio = jogo->monstro.linha * colunas + jogo->monstro.coluna;
    int alvo = jogo->jogador.linha * colunas + jogo->jogador.coluna;
    int frente = 0;
    int tras = 0;

    fila[tras++] = inicio;
    pais[inicio] = inicio;

    const int dl[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    while (frente < tras && pais[alvo] == -1) {
        int atual = fila[frente++];
        int l = atual / colunas;
        int c = atual % colunas;

        for (int d = 0; d < 4; d++) {
            int nl = l + dl[d];
            int nc = c + dc[d];

            /* O monstro tambem pode atravessar as bordas conectadas. */
            if (!dentroDoMapa(nl, nc, linhas, colunas)) {
                if (!ajustarBorda(mapa, &nl, &nc, linhas, colunas))
                    continue;
            }

            if (ehParede(mapa, nl, nc))
                continue;

            int vizinho = nl * colunas + nc;

            if (pais[vizinho] != -1)
                continue;

            pais[vizinho] = atual;
            fila[tras++] = vizinho;
        }
    }

    int moveu = 0;

    if (pais[alvo] != -1) {
        int passo = alvo;

        while (pais[passo] != inicio && passo != inicio)
            passo = pais[passo];

        jogo->monstro.linha = passo / colunas;
        jogo->monstro.coluna = passo % colunas;
        moveu = 1;
    }

    free(pais);
    free(fila);

    return moveu;
}

/* Atualiza a stamina sem bloquear o jogo. */
static void atualizarStamina(EstadoJogo *jogo, double agora, double *ultRecarga) {
    if (jogo->stamina >= STAMINA_MAX) {
        *ultRecarga = agora;
        return;
    }

    double tempoDec = agora - *ultRecarga;

    if (tempoDec >= REGENERACAO_MS) {
        int cargas = (int)(tempoDec / REGENERACAO_MS);

        jogo->stamina += cargas;

        if (jogo->stamina > STAMINA_MAX)
            jogo->stamina = STAMINA_MAX;

        *ultRecarga += cargas * REGENERACAO_MS;

        if (jogo->stamina == STAMINA_MAX)
            *ultRecarga = agora;
    }
}

int iniciarModo2(char **mapa, int linhas, int colunas) {
    if (mapa == NULL || linhas <= 0 || colunas <= 0) {
        printf("Erro: mapa invalido.\n");
        return 0;
    }

    char **copia = copiarMapa(mapa, linhas, colunas);

    if (copia == NULL) {
        printf("Erro ao copiar o mapa.\n");
        return 0;
    }

    EstadoJogo jogo;

    if (!inicializarEstado(copia, linhas, colunas, &jogo)) {
        printf("Erro: o mapa precisa de uma entrada A e uma saida X.\n");
        liberarMapa(copia, linhas);
        return 0;
    }

    srand((unsigned int)time(NULL));

    if (!posicionarMonstro(copia, linhas, colunas, &jogo)) {
        printf("Erro: nao foi encontrada uma posicao adequada para M.\n");
        liberarMapa(copia, linhas);
        return 0;
    }

    if (!iniciarTerminal()) {
        printf("Erro ao configurar a entrada do terminal.\n");
        liberarMapa(copia, linhas);
        return 0;
    }

    /*
     * O tamanho médio das dimensões define a escala.
     * 10x10 resulta em escala 1.0.
     */
    double escala = 10.0 / ((double)linhas + colunas) * 2.0;

    if (escala < 0.1)
        escala = 0.1;

    double tempoMovJogador = MOVIMENTO_BASE_MS * escala;
    double ultRecarga = tempoAtualMs();
    double ultMovJogador = 0.0;
    double ultMovMonstro = tempoAtualMs();

    int resultado = 0;
    int terminou = 0;

    /* O mapa completo e desenhado apenas uma vez. */
    desenharMapa(copia, linhas, colunas, &jogo);

    int stamAnt = jogo.stamina;
    int chavesAnt = jogo.chaves;
    int distAnt = distanciaGlobal(jogo.jogador, jogo.monstro, linhas, colunas);

    while (!terminou) {
        double agora = tempoAtualMs();
        Posicao jogAnt = jogo.jogador;
        Posicao monAnt = jogo.monstro;

        atualizarStamina(&jogo, agora, &ultRecarga);

        int tecla = lerTecla();

        if (tecla == 'q' || tecla == 'Q') {
            resultado = 0;
            break;
        }

        if (tecla != -1 && agora - ultMovJogador >= tempoMovJogador) {
            int movimento = moverJogador(copia, linhas, colunas, &jogo, tecla);

            if (movimento == -1) {
                resultado = -1;
                terminou = 1;
            } else if (movimento == 2) {
                resultado = 1;
                terminou = 1;
            } else if (movimento == 1) {
                ultMovJogador = agora;
            }
        }

        if (terminou)
            break;

        int distMon = distanciaGlobal(jogo.jogador, jogo.monstro, linhas, colunas);

        double tempoMovMonstro = (distMon > DISTANCIA_PERSEGUICAO) ? MONSTRO_LENTO_MS * escala : MONSTRO_RAPIDO_MS * escala;

        if (agora - ultMovMonstro >= tempoMovMonstro) {
            moverMonstro(copia, linhas, colunas, &jogo);
            ultMovMonstro = agora;

            if (jogo.jogador.linha == jogo.monstro.linha && jogo.jogador.coluna == jogo.monstro.coluna) {
                resultado = -1;
                terminou = 1;
            }
        }

        /* Atualiza somente as celulas ocupadas anteriormente ou agora. */
        if (jogo.jogador.linha != jogAnt.linha || jogo.jogador.coluna != jogAnt.coluna || jogo.monstro.linha != monAnt.linha || jogo.monstro.coluna != monAnt.coluna) {

            if (jogo.jogador.linha != jogAnt.linha || jogo.jogador.coluna != jogAnt.coluna) {
                restaurarCelula(copia, jogAnt.linha, jogAnt.coluna);
            }

            if (jogo.monstro.linha != monAnt.linha || jogo.monstro.coluna != monAnt.coluna) {
                restaurarCelula(copia, monAnt.linha, monAnt.coluna);
            }

            desenharCelula(jogo.jogador.linha, jogo.jogador.coluna, 'P');
            desenharCelula(jogo.monstro.linha, jogo.monstro.coluna, 'M');
        }

        /* O painel so e reescrito quando algum valor exibido muda. */
        int distAtual = distanciaGlobal(jogo.jogador, jogo.monstro, linhas, colunas);
        int mudouStam = jogo.stamina != stamAnt;
        int mudouChaves = jogo.chaves != chavesAnt;
        int mudouDist = distAtual != distAnt;

        if (mudouStam || mudouChaves || mudouDist) {
            atualizarPainel(linhas, &jogo, distAtual, mudouChaves, mudouStam, mudouDist);
            stamAnt = jogo.stamina;
            chavesAnt = jogo.chaves;
            distAnt = distAtual;
        }

        esperarCiclo();
    }

    encerrarTerminal();

    limparTela();

    liberarMapa(copia, linhas);

    return resultado == 1;
}