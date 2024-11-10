#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Constantes 
#define N_ADVS  11
#define N_BOLAS 12
#define N_CARDS 6
#define DIVISOR 17  // 51 níveis -> Divide o jogo em 3 grandes blocos
#define CX 124	// Posições iniciais do jogador
#define CY 200
#define XMIN 15	// Limites máximos de deslocamento na tela
#define XMAX 233 
#define YMIN 30
#define YMAX 208

// Macros
#define pos(n) (n) >> 8

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
    bool ativo;
    bool vermelho;
    word x, y;
    byte dir;
} Cartao;

// Inicializações 
void inicializaAdv(Adversario* a);
void inicializaBol(Bola* b);
void inicializaCar(Cartao* c);

// Funções aplicadas a objetos 
void levaCartao(register Adversario* a, register Cartao* c, byte nivel);

#endif