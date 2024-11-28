#include <stdlib.h>
#include "constantes.h"
#include "objetos.h"

// Tabela normalizada de tangentes
const short const TG[16] = {-2599, -844, -479, -312, -210, -137, -78, -25, 25, 78, 137, 210, 312, 479, 844, 2599};

byte i;	// Variável para iteração

void inicializa_adv(Adversario* a)
{
    for (i = 0; i < N_ADVS; i++)
    {
        a[i].ativo = false;
        a[i].energia = 100;
    }
}

void inicializa_bol(Bola* b)
{ for (i = 0; i < N_BOLAS; i++) b[i].ativo = false; }

void inicializa_car(Cartao* c)
{  for (i = 0; i < N_CARDS; i++) c[i].info = 0x00; }

byte direcao(short dx, short dy)
{
    short delta = pos(dx), tan;
    if (delta == 0) return dy >= 0 ? 8 : 24;
    tan = dy / delta;
    i = tan > 0 ? 8 : 0;
    while (tan >= TG[i] && i < 15) i++;
    i += (dx > 0 ? 24 : 8);
    return i & 0x1F;
}

byte direcao2(word x1, word x2, word y1, word y2)
{ return direcao(DIV2(x1) - DIV2(x2), DIV2(y1) - DIV2(y2)); }

byte movimento(char pad)
{
    byte dir;
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
    else return 0x00;
    return 0x20 | dir;
}

bool nao_bate_parede(byte arena, word x, word y, bool jog)
{
    byte i = pos(x), j = pos(y);
    if (i <= XMIN || i >= XMAX || j <= YMIN || j >= YMAX) return false;
    // Valores adaptados dos desenhos criados (usa menos RAM)
    if (arena <= 2)
    {	// Casos comuns aos níveis normais
        if (j <= YMIN + (jog ? 16 : 8)) return i >= XMIN + 81 && i <= XMAX - 81;
      	if (j <= YMIN + (jog ? 32 : 24))
            return i <= XMIN + 57 || i >= XMAX - 57 || i >= XMIN + 81 && i <= XMAX - 81;
    }
    switch (arena)	
    {	// Casos específicos
      	case 0:
            if (j >= YMAX - (jog ? 16 : 24)) return false;
            if (j >= 112 && j <= (jog ? 142 : 134)) return i <= XMIN + 73 || i >= XMAX - 73;
            break;
      	case 1:
            if (j >= 128 && j <= (jog ? 158 : 150)) return i >= XMIN + 65 && i <= XMAX - 65;
            break;
        case 2:
            if (j >= 80 && j <= (jog ? 110 : 102) || j >= YMAX - 32 && j <= YMAX - (jog ? 16 : 24))
              	return i >= XMIN + 17 && i <= XMAX - 17;
            if (j >= 152 && j <= (jog ? 182 : 174))
                return i <= XMIN + 41 || i >= XMAX - 41 || i >= XMIN + 81 && i <= XMAX - 81;
            if (j >= YMAX - (jog ? 16 : 24)) return i >= XMIN + 33 && i <= XMAX - 33;
            break;
      	default:
            if (j <= YMIN + (jog ? 16 : 8)) return false;
            if (j >= 160 && j <= (jog ? 190 : 182)) return i >= XMIN + 49 && i <= XMAX - 49;
    }
    return true;
}

bool pode_chutar(byte nivel)
{ return rand() / (nivel + 1) <= 256; }

byte x_bonus(byte arena)
{
    switch (arena)
    {
      	case 3: return 61 + rand() % 134;
      	default: return 25 + rand() % 206;
    }
}

byte y_bonus(byte arena)
{
    switch (arena)
    {
      	case 0: return YMAX - 77 + rand() % 40;
      	case 1: return YMAX - 40 + rand() % 40;
      	default: return YMAX - 40 + rand() % 30;
    }
}