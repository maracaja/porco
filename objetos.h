#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Constantes 
#define N_ADVS  11
#define N_BOLAS 32
#define N_CARDS 10
#define DIVISOR 17  // 51 níveis -> Divide o jogo em 3 grandes blocos

// Macros
#define pos(n) (n) >> 8
#define posX(o) pos(*o->x)
#define posY(o) pos(*o->y)

// Objetos 
typedef struct {
    bool ativo;
    unsigned short x, y;
    char energia;
} Adversario;

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
    bool vermelho;
} Jogador;

// Inicializações 
Jogador* inicializaJogador(void);
void inicializaAdv(Adversario* a);
void inicializaBol(Bola* b);
void inicializaCar(Cartao* c);

// Funções aplicadas a objetos 
void levaBolada(Jogador* j);
void sofreFalta(Jogador* j);
void tomaEnergetico(Jogador* j);
void escalaGoleiro(Jogador* j);
void compraArbitro(Jogador* j);
void levaCartao(Adversario* a, Cartao* c, unsigned char nivel);

#endif