#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

typedef struct {
    bool ativo = false;
    unsigned short x, y;
    unsigned char dir;
} Bola;

typedef struct {
    bool ativo = false;
    bool vermelho = false;
    unsigned short x, y;
    unsigned char dir;
} Cartao;

typedef struct {
    unsigned short x, y;
    unsigned char dinheiro;
    unsigned char energia;
    unsigned char luvas;
    unsigned char vidas;
    bool cartoes = false;
} Jogador;

typedef struct {
    bool ativo = false;
    unsigned short x, y;
    unsigned char energia;
} Adversario;

#endif

