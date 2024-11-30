#include <string.h>
#include "vrambuf.h"
#include "sprites.h"
#include "titulo.h"
#include "niveis.h"
#include "constantes.h"
#include "funcoes.h"

int vabs(int x)
{ return x >= 0 ? x : -x; }

void espera(word n)
{	
    word z;
    for (z = 0; z < n; z++) ppu_wait_nmi();
}

void escrita_centralizada(const char* str, unsigned char linha)
{
    byte tam, col;
    tam = strlen(str);
    col = 16 - tam / 2 - tam % 2;
    vram_adr(NTADR_A(col, linha));
    vram_write(str, tam);
}

void limpa_tela(unsigned int adr)
{
    ppu_off();
    oam_clear();
    set_vram_update(NULL);
    vram_adr(adr);
    vram_fill(0, 1024);
    ppu_on_all();
}

void disclaimer()
{
    byte z;
    const char* const trechos[5] = { "ESTA e UMA OBRA DE FICcAO.",
                              	     "QUALQUER SEMELHANcA COM",
                                     "NOMES, PESSOAS, FATOS OU",
                                     "CLUBES DE FUTEBOL NAO PASSA",
                                     "DE MERA COINCIDENCIA." };
    ppu_off();
    for (z = 0; z < 5; z++) 
        escrita_centralizada(trechos[z], 2 * z + 10);
    ppu_on_all();
    espera(380);
}

void apresentacao()
{
    byte z = 0;
    ppu_off();
    vram_adr(NAMETABLE_C);
    vram_unrle(titulo);
    ppu_on_all();
    disclaimer();
    for (z = 0; z < 240; z++) 
    {
        ppu_wait_nmi();
        scroll(0, z);
    }
}

void selecao(bool completo)
{ oam_spr(52, 175 + (completo ? 24 : 0), CARD, completo ? 0 : 3, 4); }

void entrada_nivel(byte nivel)
{
    limpa_tela(NAMETABLE_A);
    ppu_off();
    vram_adr(NTADR_A(12, 13));
    vram_write("NIVEL", 5);
    vram_adr(NTADR_A(18, 13));
    vram_put(ZERO + nivel / 10);
    vram_adr(NTADR_A(19, 13));
    vram_put(ZERO + nivel % 10);
    if (nivel % DIVISOR == 0)
    {
        switch (nivel / DIVISOR)
        {
            case 0: escrita_centralizada("AQUECIMENTO", 15); break;
            case 1: escrita_centralizada("EL TIGRE", 15); break;
            case 2: escrita_centralizada("EDISON", 15); break;
            default: escrita_centralizada("A GRANDE FINAL", 15);
        }
    }
    ppu_on_all();
    espera(120);
    limpa_tela(NAMETABLE_A);
}

void game_over()
{
    limpa_tela(NAMETABLE_A);
    ppu_off();
    escrita_centralizada("ELIMINADO!", 13);
    escrita_centralizada("A PIADA CONTINUA", 15);
    ppu_on_all();
}

byte carrega_arena(byte nivel)
{
    byte c;
    vram_adr(NAMETABLE_A);
    switch (nivel)
    {
        case 17:
        case 34:
        case 51:            
            vram_unrle(nivel_v);
            return 3;
      	default:
            c = nivel % 3;
            switch (c)
            {
             	case 0: vram_unrle(nivel_a); break;
              	case 1: vram_unrle(nivel_b); break;
              	default: vram_unrle(nivel_c);
            }
            return c;
    }
}

void placar(byte n, byte col, byte dig)
{
    byte sprid;
    sbyte z;
    switch (col)
    {
        case PLV: sprid = PL_VIDA; break;
      	case PLD: sprid = PL_DIN; break;
      	case PLE: sprid = PL_NRG; break;
        case PLL: sprid = PL_LUVA; break;
        default: sprid = PL_LUVA + 4;
    }
    for (z = dig - 1; z >= 0; z--)
    {
      	oam_spr((col + z) << 3, PL_LIN, ZERO + n % 10, 0, sprid + 4 * z);
        n /= 10;
    }
}

void cor_cartao(bool verm)
{ oam_spr(PLC, PL_LIN, CARD, verm ? 0 : 3, PL_CARD); }
