#ifndef OBJETOS_H
#define OBJETOS_H

#include "neslib.h"

// Macros
#define DIV2(x) ((x) >> 1)
#define pos(n) ((n) >> 8)
#define real(n) ((n) << 8)
#define vermelho(c) ((c).info & CARD_VERM)
#define card_ativo(c) ((c).info & CARD_ATIVO)
#define card_dir(c) ((c).info & 0x1F)

// Objetos 
typedef struct {
    bool ativo;
    word x, y;
    sbyte energia;
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
  
typedef enum {TIGRE, EDISON, DIABITO} Nome;

typedef struct {
    bool ativo;
    Nome nome;
    word x, y;
    sbyte energia;
} Vilao;

// Inicializações 
void inicializa_adv(Adversario* a);
void inicializa_bol(Bola* b);
void inicializa_car(Cartao* c);

// Encontra a direção para o adversário chutar
byte direcao(short dx, short dy);

// Encontra a direção, mas dividindo os termos por 2
byte direcao2(word x1, word x2, word y1, word y2);

// Função que define o movimento do jogador
byte movimento(char pad);

// Verifica se objeto nao bate nos blocos
bool nao_bate_parede(byte arena, word x, word y, bool jog);

// Seleciona um número aleatório entre 0 e 255 e muda a semente
byte aleatorio(void);

// Define, dada uma probabilidade, se o adversário pode chutar ou não uma bola
bool pode_chutar(byte nivel);

// Define, aleatoriamente, as posições para o surgimento de um bônus
byte x_bonus(byte arena);
byte y_bonus(byte arena);

#endif