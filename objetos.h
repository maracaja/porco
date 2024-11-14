#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// CONSTANTES
#define DIVISOR 17  // 51 níveis -> Divide o jogo em 3 grandes blocos
// Quantidades máximas simultâneas de sprites
#define N_ADVS  11
#define N_BOLAS 12
#define N_CARDS 6
// Posições iniciais do jogador
#define CX 124
#define CY 200
// Limites máximos de deslocamento na tela
#define XMIN 15
#define XMAX 233 
#define YMIN 30
#define YMAX 208
// Identificação dos bônus
#define BGOL 0x01
#define BARB 0x02
#define BNRG 0x04
#define BDIN 0x08
#define TEM_CARTAO 0x10
#define CARD_VERM 0x20

// Macros
#define pos(n) (n) >> 8
#define vermelho(n) (n) & CARD_VERM

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

#endif