#ifndef ANIMACAO_H
#define ANIMACAO_H

#include "labirinto.h"
#include "Busca.h"
#define ANIMACAO_DELAY_MS 300

/*
 * Imprime o mapa e, a cada delay_ms, pinta de 🟥 a sala por onde o
 * aventureiro passa, seguindo res->caminho (gerado por busca_resolver).
 */
void animar_caminho(const Labirinto *lab, const ResultadoBusca *res, int delay_ms);

#endif