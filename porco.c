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

/*{pal:"nes",layout:"nes"}*/
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

// draw a message on the screen
void escreve_mensagem(const char* msg, unsigned char col_ini, unsigned char col_fim) 
{
  char ch;
  byte x,y;
  x = 2;


  // repeat until end of string (0) is read
  while ((ch = *charptr++)) {
    while (y >= 60) y -= 60; // compute (y % 60)
    // newline character? go to start of next line
    if (ch == '\n') {
      x = 2;
      y++;
    } else {
      // put character into nametable
      vrambuf_put(getntaddr(x, y), &ch, 1);
      x++;
    }
    // typewriter sound
    sfx_play(SND_HIT,0);
    // flush buffer and wait a few frames
    vrambuf_flush();
    delay(5);
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
    pal_col(25, 0x0A);
    ppu_on_spr();
    oam_meta_spr(32, 24, 24, spr_titia);
    ppu_on_bg();
    
    
    vram_adr(NTADR_A(2, 18));
    vram_write(completo ? "JOGO COMPLETO": " DEMONSTRAcAO", 13);
    // infinite loop
    while(1) 
    {
      	
    }
}
