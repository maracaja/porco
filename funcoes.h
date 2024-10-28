#ifndef FUNCOES_H
#define FUNCOES_H

/* Constantes */
#define NORMAL 4
#define CAIM 0x84
#define MOEDA 0x04
#define ENERG 0x94
#define CARD 0x01
#define BOLA 0x24
#define TACA 0x14
#define TITIA 0x64
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
