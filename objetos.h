#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Constantes 
#define N_ADVS  11
#define N_BOLAS 12
#define N_CARDS 6
#define DIVISOR 17  // 51 níveis -> Divide o jogo em 3 grandes blocos
#define CX 120
#define CY 200
#define XMIN 8
#define XMAX 236
#define YMIN 16
#define YMAX 224

// Macros
#define pos(n) (n) >> 8
#define MAP_ADR(x,y) (((y)-2) << 4)|(x)

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

// Inicializações 
void inicializaAdv(Adversario* a);
void inicializaBol(Bola* b);
void inicializaCar(Cartao* c);

// Funções aplicadas a objetos 
// Verifica se o personagem vai bater ou não na parede
bool bate_parede(word x, word y);
void levaCartao(register Adversario* a, register Cartao* c, unsigned char nivel);

#endif