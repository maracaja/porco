#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// Importacao dos recursos graficos
//#resource "chr_porco.chr"
//#link "tileset.s"
#include "titulo.h"
#include "sprites.h"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x1A,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x28 };

// setup PPU and tables
void setup_graphics() {
    // clear sprites
    oam_clear();
    // set palette colors
    pal_bg(PALETTE);
    pal_spr(PALETTE);
}

void disclaimer()
{
    unsigned char i, tam, col;
    const char* trechos[5] = { "ESTA e UMA OBRA DE FICcAO.",
                              "QUALQUER SEMELHANcA COM",
                              "NOMES, PESSOAS, FATOS OU",
                              "CLUBES DE FUTEBOL NAO PASSA",
                              "DE MERA COINCIDENCIA." };
    for (i = 0; i < 5; i++)
    {
        tam = strlen(trechos[i]);
        col = 16 - tam / 2 - tam % 2;
        vram_adr(NTADR_A(col, 2 * i + 10));
        vram_write(trechos[i], tam);
    }
    ppu_on_all();
    delay(255);
}

void selecao(bool completo)
{
    ppu_wait_frame();
    vram_adr(NTADR_A(6, completo ? 22 : 25));
    vram_put(0x00);
    pal_col(3, PALETTE[completo ? 3 : 15]);
    vram_adr(NTADR_A(6, completo ? 25 : 22));
    vram_put(0x06);
}

void apresentacao()
{
    unsigned int i = 0;
    vram_adr(NAMETABLE_C);
    vram_unrle(titulo);
    disclaimer();
    for (i = 0; i < 240; i++) 
    {
        ppu_wait_frame();
        scroll(0, i);
    }
    ppu_off();
    delay(1);
    vram_adr(NAMETABLE_A);
    vram_unrle(titulo);
    scroll(0, 0);
    ppu_on_all();
}

void main(void)
{
    char pad;
    bool completo = false, menu = true;
    setup_graphics();
    apresentacao();
    selecao(false);
    // Controle do menu de jogo
    while (menu)
    {
        pad = pad_poll(0);
      	if (pad & PAD_DOWN && !completo) 
        {
            completo = true;
            selecao(true);
        }  
        if (pad & PAD_UP && completo)
        {
            completo = false;
            selecao(false);
        }
        if (pad & PAD_START) menu = false;
    }
    //vram_adr(NTADR_A(2, 2));
    oam_meta_spr(32, 24, 0, spr_titia);
    vram_adr(NTADR_A(2, 18));
    vram_write(completo ? "JOGO COMPLETO": " DEMONSTRAcAO", 13);
    // infinite loop
    while(1) 
    {
      	
    }
}
