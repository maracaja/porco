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

void selecao(bool completo)
{
    oam_spr(52, 175 + (completo ? 24 : 0), 0x06, completo ? 0 : 3, 4);
    ppu_wait_nmi();
}

void main(void)
{
    char pad;
    bool completo = false, menu = true, lado = false;
    unsigned char i, j;
    unsigned char caim_x, caim_y, caim_id, tt; // Coordenadas do personagem principal
    const unsigned char titia_x = 24, titia_y = 32;
    setup_graphics();
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
    // Prepara apresentacao do jogo
    setup_graphics();
    limpa_tela(NAMETABLE_C);
    limpa_tela(NAMETABLE_A);
    scroll(0,0);
    ppu_off();
    escrita_centralizada("BROOKLIN", 13);
    escrita_centralizada("1984", 14);
    ppu_on_all();
    delay(180);
    limpa_tela(NAMETABLE_A);
    vram_adr(NAMETABLE_A);
    // Inicio do jogo
    caim_x = 111; caim_y = 224;
    caim_id = oam_meta_spr(caim_x, caim_y, 0, spr_caim_parado);
    tt = oam_meta_spr(titia_x, titia_y, 1, spr_titia);
    j = 0;
    for (i = 0; i <= 86; )
    {
        if (j % 4 == 0)
        {
          caim_x--; caim_y -= 2;
          //oam_clear();
          /oam_meta_spr(titia_x, titia_y, tt, spr_titia);
          caim_id = oam_meta_spr(caim_x, caim_y, 0, spr_caim[lado = !lado]);
          i++;
        }
        ppu_wait_nmi();
        j++;
    }
    //oam_clear();
    oam_hide_rest(caim_id);
    //oam_meta_spr(titia_x, titia_y, tt, spr_titia);
    caim_id = oam_meta_spr(caim_x, caim_y, 0, spr_caim_parado);
    // infinite loop
    while(1) 
    {
      	
    }
}
