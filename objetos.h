#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

#define N_ADVS  11
#define N_BOLAS 32
#define N_CARDS 10

typedef struct {
    bool ativo;
    unsigned short x, y;
    unsigned char dir;
} Bola;

typedef struct {
    bool ativo;
    bool vermelho;
    unsigned short x, y;
    unsigned char dir;
} Cartao;

typedef struct {
    unsigned short x, y;
    unsigned char dinheiro;
    unsigned char energia;
    unsigned char luvas;
    unsigned char vidas;
    bool cartoes;
} Jogador;

typedef struct {
    bool ativo;
    unsigned short x, y;
    unsigned char energia;
} Adversario;

#endif

