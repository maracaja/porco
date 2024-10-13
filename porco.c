#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// Importacao do nosso tileset
//#resource "chr_porco.chr"
//#link "tileset.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0c,0x0f,0x30,0x16,0x0c,0x1a,0x36,0x30,0x0c,0x04,0x36,0x07,0x0c,0x27,0x10,0x38 };

/*
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};*/

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

void disclaimer()
{
  unsigned char i, tam, col;
  char *trechos[5] = {"ESTA e UMA OBRA DE FICCAO.", "QUALQUER SEMELHANCA COM", "NOMES, PESSOAS, FATOS OU", "CLUBES DE FUTEBOL NAO PASSA", "DE MERA COINCIDENCIA."};
  for (i = 0; i < 5; i++)
  {
    tam = strlen(trechos[i]);
    col = 16 - tam/2 - tam % 2;
    vram_adr(NTADR_A(col, 2 * i + 10));
    vram_write(trechos[i], tam);  
  }
}

void main(void)
{
  setup_graphics();
  disclaimer();
  ppu_on_all();
  // infinite loop
  while(1) {
  }
}
