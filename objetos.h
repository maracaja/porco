#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Macros
#define pos(n) ((n) >> 8)
#define real(n) ((n) << 8)
#define vermelho(c) ((c).info & CARD_VERM)
#define card_ativo(c) ((c).info & CARD_ATIVO)

// Objetos 
typedef struct {
    bool ativo;
    word x, y;
    char energia;
} Adversario;

typedef struct {
    bool ativo;
    word x, y;
    byte dir;
} Bola;

typedef struct {
    byte info;	// Combina ativo com cor
    word x, y;
    byte dir;
} Cartao;

// Inicializações 
void inicializaAdv(Adversario* a);
void inicializaBol(Bola* b);
void inicializaCar(Cartao* c);

// Funções aplicadas a objetos 
void levaCartao(Adversario* a, Cartao* c, byte nivel);

// Encontra a direção para o adversário chutar
byte direcao(short dx, short dy);

// Função que define o movimento do jogador
byte movimento(char pad);

// Verifica se objeto nao bate nos blocos
bool nao_bate_parede(byte arena, word x, word y);

// Define o sprite do adversário a ser usado
byte spr_adv(byte nivel);

#endif