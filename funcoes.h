#ifndef FUNCOES_H
#define FUNCOES_H

#include "neslib.h"

#define desenha_tia(void) oam_meta_spr(TTX, TTY, EXTRA, spr_titia)
#define desenha_jogador_intro(void) oam_meta_spr(SPRX, TTY + 18, CAIM, spr_jogador_parado)

// Máquinas de Estados
typedef enum {INICIO, MENU, INTRO, JOGO, VITORIA, DERROTA} Estado;
typedef enum {ENTRADA, LOOP, PAUSA, PERDE, AVANCA} EstadoJogo;

extern const char* const trechos_titia[];
extern const char* const trechos_tigre[];
extern const char* const trechos_edson[];
extern const char* const trechos_devil[];

// Valor absoluto
int vabs(int x);

// Delay sem travar processador
void espera(word n);

// Escreve um texto de forma a centralizá-lo na tela (nametable A)
void escrita_centralizada(const char* str, unsigned char linha);

// Limpa a tela do nametable selecionado
void limpa_tela(unsigned int adr);

// Abertura do jogo
void apresentacao(void);

// Abertura do nível
void entrada_nivel(byte nivel);

// Tela de derrota
void game_over(void);

// Função que determina o cenário do jogo
byte carrega_arena(byte nivel);

// Seleciona modo de jogo
void selecao(bool completo);

// Funções de placar
void placar(byte n, byte col, byte dig);
void cor_cartao(bool verm);

#endif