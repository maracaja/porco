#ifndef FUNCOES_H
#define FUNCOES_H

/* Constantes */
#define CAIM 0x09
#define MOEDA 0x03
#define ENERG 0x05
#define CARD 0x06
#define BOLA 0x07
#define TACA 0x13
#define TITIA 0x60
#define TTX 24
#define TTY 32

// Escreve um texto de forma a centraliza-lo na tela (A)
void escrita_centralizada(const char* str, unsigned char linha);

// Escreve uma mensagem de linha unica aparecendo um caractere por vez
void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col);

// Limpa a tela selecionada
void limpa_tela(unsigned int adr);

// Parte da abertura do jogo
void disclaimer(void);

#endif
