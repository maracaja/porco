#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// Importação do nosso tileset
//#resource "files/chr_porco.chr"
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

void main(void)
{
  setup_graphics();
  // draw message  
  vram_adr(NTADR_A(3,10));
  vram_write("ESTA e UMA OBRA DE FICCAO.", 26);
  vram_adr(NTADR_A(4,12));
  vram_write("QUALQUER SEMELHANCA COM", 23);
  vram_adr(NTADR_A(4,14));
  vram_write("NOMES, PESSOAS, FATOS OU", 24);
  vram_adr(NTADR_A(2,16));
  vram_write("CLUBES DE FUTEBOL NAO PASSA", 28);
  vram_adr(NTADR_A(5,18));
  vram_write("DE MERA COINCIDENCIA.", 21);
  // enable rendering
  ppu_on_all();
  // infinite loop
  while(1) {
  }
}
