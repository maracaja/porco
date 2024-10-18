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
#include "tutorial.h"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// Funcoes uteis
#include "funcoes.h"
//#link "funcoes.c"

// Paleta padrao
const unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x0A,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x28 };

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
    const char* trechos[5] = { "ESTA e UMA OBRA DE FICcAO.",
                              "QUALQUER SEMELHANcA COM",
                              "NOMES, PESSOAS, FATOS OU",
                              "CLUBES DE FUTEBOL NAO PASSA",
                              "DE MERA COINCIDENCIA." };
    for (i = 0; i < 5; i++)
        escrita_centralizada(trechos[i], 2 * i + 10);
    ppu_on_all();
    delay(255);
}

void selecao(bool completo)
{
    oam_spr(52, 431 + (completo ? 24 : 0), 0x06, completo ? 0 : 3, 4);
    ppu_wait_nmi();
}

void apresentacao()
{
    unsigned int i = 0;
    vram_adr(NAMETABLE_C);
    vram_unrle(titulo);
    disclaimer();
    for (i = 0; i < 240; i++) 
    {
        ppu_wait_nmi();
        scroll(0, i);
    }
}

void main(void)
{
    char pad;
    bool completo = false, menu = true;
    setup_graphics();
    apresentacao();
    selecao(completo);
    // Controle do menu de jogo
    while (menu)
    {
        pad = pad_poll(0);
      	if (pad & PAD_DOWN && !completo) selecao(completo = true); 
        if (pad & PAD_UP && completo) selecao(completo = false);
        if (pad & PAD_START) menu = false;
    }
    ppu_off();
    oam_clear();
    set_vram_update(NULL);
    vram_adr(NTADR_A(1,1));
    vram_fill(0, 960);
    scroll(0,0);
    vram_adr(NAMETABLE_A);
    vram_unrle(tutorial);
    ppu_on_bg();   
    //vram_adr(NTADR_A(2, 18));
    escree_mensagem(completo ? "JOGO COMPLETO": " DEMONSTRAcAO", 2, 5);
    // infinite loop
    while(1) 
    {
      	
    }
}
