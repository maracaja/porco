#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Macros
#define pos(n) ((n) >> 8)
#define real(n) ((n) << 8)
#define vermelho(c) ((c).info & CARD_VERM)
#define card_ativo(c) ((c).info & CARD_ATIVO)
#define card_dir(c) ((c).info & 0x1F)

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
    byte info;	// Ativo + cor + direção
    word x, y;
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

#endif