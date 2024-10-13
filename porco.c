#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// Importacao do nosso tileset
//#resource "files/chr_porco.chr"
//#link "tileset.s"

// Importacao dos demais recursos graficos
#include "graphics/titulo.h"
#include "graphics/sprites.h"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0c,0x0f,0x30,0x16,0x0c,0x1a,0x36,0x30,0x0c,0x04,0x36,0x07,0x0c,0x27,0x10,0x38 };

// setup PPU and tables
void setup_graphics() {
    // clear sprites
    oam_clear();
    // set palette colors
    pal_all(PALETTE);
}

// Frase inicial, com centralizacao automatica
void disclaimer()
{
    const unsigned char N_LIN = 5;
    unsigned char i, tam, col;
    const char* trechos[N_LIN] = {"ESTA e UMA OBRA DE FICCAO.", "QUALQUER SEMELHANCA COM", "NOMES, PESSOAS, FATOS OU", "CLUBES DE FUTEBOL NAO PASSA", "DE MERA COINCIDENCIA."};
    for (i = 0; i < N_LIN; i++)
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
