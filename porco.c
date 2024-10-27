#include <stdlib.h>
#include <string.h>
#include "neslib.h"
#include <nes.h>

#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
#include "bcd.h"	// BCD arithmetic support
//#link "bcd.c"
#include "funcoes.h"	// Funcoes úteis
//#link "funcoes.c"

// Importacao dos recursos gráficos
//#resource "chr_porco.chr"
//#link "tileset.s"
//#resource "sprites.h"
//#resource "titulo.h"

/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x0A,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };

// setup PPU and tables
void setup_graphics() 
{
    // clear sprites
    oam_clear();
    // set palette colors
    pal_bg(PALETTE);
    pal_spr(PALETTE);
}

void selecao(bool completo)
{
    oam_spr(52, 175 + (completo ? 24 : 0), 0x06, completo ? 0 : 3, 4);
    ppu_wait_nmi();
}

void main(void)
{
    char pad;
    bool completo = false, menu = true;
    setup_graphics();
    reset_pulo();
    apresentacao();
    selecao(completo);
    // Controle do menu de jogo
    while (menu)
    {
        pad = pad_poll(0);
      	if (pad & PAD_DOWN && !completo) selecao(completo = true); 
        if (pad & PAD_UP && completo) selecao(completo = false);
        if (pad & PAD_A) menu = false;
    }
    // Prepara início da história
    setup_graphics();
    historinha();
  
    if (*pulo) limpa_tela(NAMETABLE_A);
    reset_pulo();
    
    // infinite loop
    while(1) 
    {
      	
    }
}
