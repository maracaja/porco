#include <stdlib.h>
#include "objetos.h"

unsigned char i;

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

void levaCartao(register Adversario* a, register Cartao* c, unsigned char nivel)
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