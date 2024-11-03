#include "objetos->h"

#define CX 120
#define CY 200

unsigned char i;

Jogador* inicializaJogador()
{
    Jogador *j;
    j->x = CX; j->y = CY;
    j->dinheiro = 0;
    j->energia = 99;
    j->luvas = 0;
    j->vidas = 3;
    j->cartoes = false;
    return j;
}

void inicializaAdv(Adversario* a)
{
    for (i = 0; i < N_ADVS; i++)
    {
        a[i].ativo = false;
        a[i].energia = 100;
    }
}

void inicializaBol(Bola* b)
{ for (i = 0; i < N_BOLAS; i++) b[i].ativo = false; }

void inicializaCar(Cartao* c)
{
    for (i = 0; i < N_CARDS; i++)
    {
        c[i].ativo = false;
        c[i].vermelho = false;
    }
}

void levaBolada(Jogador* j)
{
    if (j->luvas > 0) j->luvas--;
    else if (j->cartoes)
    {
        if (j->vermelho) j->vermelho = false;
        else j->cartoes = false;
    }
    else j->energia = MAX(0, j->energia - 19);
}

void sofreFalta(Jogador* j)
{ j->energia = MAX(0, j->energia - 50); }

void tomaEnergetico(Jogador* j)
{ j->energia = MIN(j->energia + 51, 99); }

void escalaGoleiro(Jogador* j)
{ j->luvas = 5; }

void compraArbitro(Jogador* j)
{
    if (!j->cartoes)
    {
        j->cartoes = true;
        j->vermelho = false;
    }
    else j->vermelho = true;
}

void levaCartao(Adversario* a, Cartao* c, unsigned char nivel)
{
    unsigned char bloco = nivel / DIVISOR;
    switch (bloco)
    {
        case 0: a->energia -= c->vermelho ? 100 : 50; break;
        case 1: a->energia -= c->vermelho ? 66 : 34; break;
        default: a->energia -= c-> vermelho ? 50 : 25;
    }
    if (a->energia <= 0) a->ativo = false;
}