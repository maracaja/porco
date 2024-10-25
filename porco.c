#include <stdlib.h>
#include <string.h>
#include "neslib.h"
#include <nes.h>

#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
#include "bcd.h"	// BCD arithmetic support
//#link "bcd.c"
#include "funcoes.h"	// Funcoes uteis
//#link "funcoes.c"

// Importacao dos recursos graficos
//#resource "chr_porco.chr"
//#link "tileset.s"
#include "titulo.h"
#include "sprites.h"

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
    bool completo = false, menu = true;
    unsigned char i;
    unsigned char caim_x, caim_y; // Coordenadas do personagem principal
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
    // Prepara inicio da historia
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
    for (i = NORMAL; i > 0;)
    {
        ppu_off();
        pal_bright(--i);
        ppu_wait_nmi();
        ppu_on_all();
        delay(5);
    }
    ppu_off();
    pal_bright(NORMAL);
    ppu_on_all();
    limpa_tela(NAMETABLE_A);
    // Inicio do jogo
    ppu_off();
    caim_x = 111; caim_y = 224;
    oam_meta_spr(caim_x, caim_y, CAIM, spr_caim_parado);
    desenha_tia();
    ppu_on_all();
    for (i = 0; i <= 86; i++)
    {      
        caim_x--; caim_y -= 2;
        oam_clear();
        desenha_tia();
        oam_meta_spr(caim_x, caim_y, CAIM, spr_caim[i % 2]);
        ppu_wait_nmi();        
        delay(4);
    }
    desenha_tia();
    oam_meta_spr(caim_x, caim_y, CAIM, spr_caim_parado);
    // Escrita das mensagens...
    // infinite loop
    while(1) 
    {
      	
    }
}
