#ifndef FUNCOES_H
#define FUNCOES_H

#include "neslib.h"

// Macros
#define reset_pulo(void) pulo = false

extern char abertura[];

// Variável de controle para escapar de introduções
static bool pulo;

// Delay sem travar processador
void espera(word n);

// Escreve um texto de forma a centralizá-lo na tela (nametable A)
void escrita_centralizada(const char* str, unsigned char linha);

// Escreve uma mensagem de linha única aparecendo um caractere por vez
void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col);

// Limpa a tela do nametable selecionado
void limpa_tela(unsigned int adr);

// Abertura do jogo
void apresentacao(void);

// Abertura do nível
void entrada_nivel(byte nivel);

// História inicial do jogo
void historinha(void);

// Tela de derrota
void game_over(void);

// Função que determina o cenário do jogo
byte carrega_arena(byte nivel);

// Funções de placar
void placar(byte n, byte col, byte dig);
void contaLuvas(byte n);
void corCartao(bool verm);

#endif
