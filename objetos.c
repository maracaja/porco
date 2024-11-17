#include "constantes.h"
#include "objetos.h"

// Tabela normalizada de tangentes
const short const TG[16] = {-2599, -844, -479, -312, -210, -137, -78, -25, 25, 78, 137, 210, 312, 479, 844, 2599};

byte i;	// Variável para iteração

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
{  for (i = 0; i < N_CARDS; i++) c[i].info = 0x00; }

void levaCartao(Adversario* a, Cartao* c, unsigned char nivel)
{
    unsigned char bloco = nivel / DIVISOR;
    switch (bloco)
    {
        case 0: a->energia -= vermelho(c->info) ? 100 : 50; break;
        case 1: a->energia -= vermelho(c->info) ? 66 : 34; break;
        default: a->energia -= vermelho(c->info) ? 50 : 25;
    }
    if (a->energia <= 0) a->ativo = false;
}

byte direcao(short dx, short dy)
{
    short delta = dx >> 8, tan;
    i = 0;
    if (delta == 0) return dy >= 0 ? 8 : 24;
    tan = dy / delta;
    while (tan >= TG[i] && i < 15) i++;
    i += dx > 0 ? 24 : 8;
    return i & 0x1F;
}

byte movimento(char pad)
{
    byte dir, mov = 0x20;
    if (pad & PAD_LEFT)
    {
        dir = 16;
        if (pad & PAD_UP) dir += 4;
        else if (pad & PAD_DOWN) dir -= 4;
    }
    else if (pad & PAD_RIGHT)
    {
        dir = 0;
        if (pad & PAD_UP) dir = 28;
        else if (pad & PAD_DOWN) dir += 4;
    }
    else if (pad & PAD_UP) dir = 24;
    else if (pad & PAD_DOWN) dir = 8;
    else mov = 0x00;
    return mov |= dir;
}

byte spr_adv(byte nivel)
{ return nivel & 0x02 ? JADV2 : JADV1; }