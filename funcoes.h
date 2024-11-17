#ifndef FUNCOES_H
#define FUNCOES_H

#include "neslib.h"

// CONSTANTES
#define NIVEIS 51
#define DIVISOR 17  	// 51 níveis -> Divide o jogo em 3 grandes blocos
#define ZERO 0x30	// Sprite '0'
#define CARD 0x01	// Sprite cartão
#define BOLA 0x0A	// Sprite bola
#define LUVA 0x62	// Sprite luva
// Sprites adversário parado
#define JADV1 0x1A
#define JADV2 0x1D
// Endereços OAM
#define CAIM 0x00
#define ENERG 0x08
#define JUIZ 0x10
#define GOLEIRO 0x18
#define ADV 0x20
#define PL_VIDA 0x4C
#define PL_NRG 0x50
#define PL_DIN 0x58
#define PL_LUVA 0x60
#define PL_CARD 0x74
#define CARTAO 0x78
#define TIRO 0x90
#define MOEDA 0xC0
#define TACA 0xD0
#define EXTRA 0xE8
// Posição da apresentação do jogo
#define TTX 24
#define TTY 32
#define SPRX TTX - 4
#define SPRY TTY + 60
// Placar
#define PL_LIN 7
#define PLV 6
#define PLD 10
#define PLE 20
#define PLL 23
#define PLC 240

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

// Define o sprite do adversário a ser usado
byte spr_adv(byte nivel);

#endif
