#include <stdlib.h>
#include <string.h>
#include "neslib.h"
#include <nes.h>

#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
#include "bcd.h"	// BCD arithmetic support
//#link "bcd.c"

// Importacao dos recursos gráficos
//#resource "chr_porco.chr"
//#link "tileset.s"
#include "sprites.h"
//#link "sprites.c"
//#resource "titulo.h"

#include "funcoes.h"	// Funcoes úteis
//#link "funcoes.c"

/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x0A,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };

extern unsigned char spr_arbitro[];

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
    oam_spr(52, 175 + (completo ? 24 : 0), CARD, completo ? 0 : 3, 4);
    ppu_wait_nmi();
}

void main(void)
{
    char pad;
    bool completo = false, menu = true, lado = false;
    unsigned char caim_x, caim_y;
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
    limpa_tela(NAMETABLE_A);
    reset_pulo();
    // Teste de nível
    vram_adr(NAMETABLE_A);
    oam_meta_spr(120, 200, CAIM, spr_caim_parado);
    
    // infinite loop
    while(1) 
    {
      	pad = pad_poll(0);
        if (pad) lado = !lado;
      	if (pad & PAD_DOWN) 
          if (caim_y < 200) caim_y++; 
        if (pad & PAD_UP)
          if (caim_y > 24) caim_y--;
        if (pad & PAD_LEFT) 
          if (caim_x > 10) caim_x--; 
        if (pad & PAD_RIGHT) 
          if (caim_x < 230) caim_x++;
        //if (pad & PAD_A) menu = false;
        //ppu_off();
        oam_clear();
        oam_meta_spr(caim_x, caim_y, CAIM, pad ? spr_caim(lado) : spr_caim_parado);
        //ppu_on_all();
        delay(4);
    }
}
