#include <stdlib.h>
#include "constantes.h"
#include "objetos.h"

// Tabela normalizada de tangentes
const int const TG[16] = {-2599, -844, -479, -312, -210, -137, -78, -25, 25, 78, 137, 210, 312, 479, 844, 2599};

byte z;	// Variável para iteração

void inicializa_adv(Adversario* a)
{
    for (z = 0; z < N_ADVS; z++)
    {
        a[z].ativo = false;
        a[z].energia = 100;
    }
}

void inicializa_bol(Bola* b)
{ for (z = 0; z < N_BOLAS; z++) b[z].ativo = false; }

void inicializa_car(Cartao* c)
{  for (z = 0; z < N_CARDS; z++) c[z].info = 0x00; }

byte direcao(int dx, int dy)
{
    int delta = pos(dx), tan;
    if (delta == 0) return dy >= 0 ? 8 : 24;
    tan = dy / delta;
    z = tan > 0 ? 8 : 0;
    while (tan >= TG[z] && z < 15) z++;
    z += (dx > 0 ? 24 : 8);
    return z & 0x1F;
}

byte direcao2(word x1, word x2, word y1, word y2)
{ return direcao(DIV2(x1) - DIV2(x2), DIV2(y1) - DIV2(y2)); }

byte movimento(char pad)
{
    byte dir;
    if (pad & PAD_LEFT)
    {
        if (pad & PAD_UP) dir = 20;
        else if (pad & PAD_DOWN) dir = 12;
        else dir = 16;
    }
    else if (pad & PAD_RIGHT)
    {
        if (pad & PAD_UP) dir = 28;
        else if (pad & PAD_DOWN) dir = 4;
        else dir = 0;
    }
    else if (pad & PAD_UP) dir = 24;
    else if (pad & PAD_DOWN) dir = 8;
    else return 0x00;
    return 0x20 | dir;
}

// Verifica se sprites não colidem com os blocos desenhados das arenas.
// Valores foram calculados a partir das constantes XMIN, XMAX, YMIN e YMAX
bool nao_bate_parede(byte arena, word x, word y, bool jog)
{
    byte px = pos(x), py = pos(y);
    if (px <= XMIN || px >= XMAX || py <= YMIN || py >= YMAX) return false;
    // Valores adaptados dos desenhos criados (usa menos RAM)
    if (arena <= 2)
    {	// Casos comuns aos níveis normais
        if (py <= (jog ? 46 : 38)) return px >= 96 && px <= 152;
      	if (py <= (jog ? 62 : 54)) return px <= 72 || px >= 176 || px >= 96 && px <= 152;
    }
    switch (arena)	
    {	// Casos específicos
      	case 0:
            if (py >= 208) return false;
            if (py >= 112 && py <= (jog ? 142 : 134)) return px <= 88 || px >= 160;
            break;
      	case 1:
            if (py >= 128 && py <= (jog ? 158 : 150)) return px >= 80 && px <= 168;
            break;
        case 2:
            if (py >= 80 && py <= (jog ? 110 : 102) || py >= 192 && py <= (jog ? 208 : 200))
              	return px >= 32 && px <= 216;
            if (py >= 152 && py <= (jog ? 182 : 174))
                return px <= 56 || px >= 192 || px >= 96 && px <= 152;
            if (py >= (jog ? 208 : 200)) return px >= 48 && px <= 200;
            break;
      	default:
            if (py <= (jog ? 46 : 38)) return false;
            if (py >= 160 && py <= (jog ? 190 : 182)) return px >= 64 && px <= 184;
    }
    return true;
}

bool pode_chutar(byte nivel, byte id_bola)
{ return id_bola < N_BOLAS && rand8() <= nivel; }

byte x_bonus(byte arena)
{
    switch (arena)
    {
      	case 3: return 80 + rand16() % 96;
      	default: return 25 + rand16() % 206;
    }
}

byte y_bonus(byte arena)
{
    switch (arena)
    {
      	case 0: return 144 + rand16() % 40; 	// YMAX - 80 = 144
      	case 1: return 184 + rand16() % 40;	// YMAX - 40 = 184
      	default: return 184 + rand16() % 30;
    }
}
