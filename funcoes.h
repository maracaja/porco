#ifndef FUNCOES_H
#define FUNCOES_H

/* Constantes */
#define NORMAL 4
#define CARD 0x01
#define BOLA 0x0A
#define CAIM 0x00
#define ENERG 0x08
#define JUIZ 0x10
#define GOLEIRO 0x18
// 0x20 - 0x77 = adversários (<=11)
// 0x78 - 0x8F = cartões disparados (<=10)
// 0x90 - 0xBF = bolas chutadas (<=32)
#define MOEDA 0xC0
#define TACA 0xD0
#define EXTRA 0xE8
#define TTX 24
#define TTY 32

/* Macros */
#define reset_pulo(void) *pulo = false

/* Variável de controle para escapar de introduções */
static bool *pulo;

// Escreve um texto de forma a centralizá-lo na tela (nametable A)
void escrita_centralizada(const char* str, unsigned char linha);

// Escreve uma mensagem de linha única aparecendo um caractere por vez
void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col);

// Limpa a tela do nametable selecionado
void limpa_tela(unsigned int adr);

// Abertura do jogo
void apresentacao(void);

// História inicial do jogo
void historinha(void);

#endif
