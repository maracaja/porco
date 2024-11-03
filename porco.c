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
#include "objetos.h"    // Definição dos atores do jogo

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x0A,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };

//extern unsigned char spr_arbitro[];

Adversario advs[11];
Bola bolas[32];
Cartao cards[10];

// setup PPU and tables
void setup_graphics() 
{  
    oam_clear();   // clear sprites
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
    bool completo = false, menu, lado = false, pausa = false;
    unsigned char i = 0;
    unsigned char caim_x = CX, caim_y = CY;
    while (1)   // Loop infinito
    {
        setup_graphics();
        menu = true;
        reset_pulo();
        apresentacao();
        selecao(completo);
        while (menu)    // Controle do menu de jogo
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
        oam_meta_spr(caim_x, caim_x, CAIM, spr_jogador_parado);
        while (1)
        {
            pad = pad_poll(0);
            if (pausa && pad & PAD_START)
            {
              	pausa = false;
                ppu_off();
                escrita_centralizada("       ", 2);
                ppu_on_all();
                delay(5);
            }
            else if (!pausa)
            {
                i++;
                if (i == 4) i = 0;
                if (pad & 0xF0 && i == 0) lado = !lado;
                if (pad & PAD_DOWN)
                    if (caim_y < 216) caim_y++;
                if (pad & PAD_UP)
                    if (caim_y > 24) caim_y--;
                if (pad & PAD_LEFT)
                    if (caim_x > 10) caim_x--;
                if (pad & PAD_RIGHT)
                    if (caim_x < 238) caim_x++;
                if (pad & PAD_B)
                {
                    limpa_tela(NAMETABLE_A);
                    limpa_tela(NAMETABLE_C);
                    ppu_off();
                    break;// TESTE DE SAÍDA DEPOIS DE PERDER
                }
                if (pad & PAD_START)
                {
                    pausa = true;
                    ppu_off();
                    escrita_centralizada("PAUSADO", 2);
                    ppu_on_all();
                }
                oam_clear();
                oam_meta_spr(caim_x, caim_y, CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado);
                ppu_wait_nmi();   // Desfazer para fazer o cálculo quadro a quadro
                if (pausa) delay(30);
            }
        }
    }
}