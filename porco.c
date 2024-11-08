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
#include "nivel_a.h"

#include "funcoes.h"	// Funcoes úteis
//#link "funcoes.c"
#include "objetos.h"    // Definição dos atores do jogo
//#link "objetos.c"

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
unsigned char PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x19,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };
const unsigned char PAL_EXTRA[16] = { 0x0c,0x16,0x36,0x30,0x0c,0x30,0x17,0x0f,0x0c,0x38,0x27,0x21,0x0c,0x30,0x06,0x0f };
// MAN-COR-TIG-SAN

// Tabela de senos normalizados em 8 bits
const short const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Tabela de níveis do modo demonstração
const unsigned char const demo[6] = {0, 10, 25, 34, 50, 51};

// Objetos
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];

// Dados do jogador
unsigned short x, y;
unsigned char dinheiro;
unsigned char energia;
unsigned char luvas;
unsigned char vidas;
bool cartoes;
bool vermelho;

// setup PPU and tables
void setup_graphics() 
{  
    oam_clear();   // clear sprites
    // set palette colors
    pal_bg(PALETTE);
    pal_spr(PALETTE);
}

// Seleciona modo de jogo
void selecao(bool completo)
{
    oam_spr(52, 175 + (completo ? 24 : 0), CARD, completo ? 0 : 3, 4);
    ppu_wait_nmi();
}

// Funções referentes ao jogador
void inicializaJogador()
{
    x = CX << 8;
    y = CY << 8;
    dinheiro = 0;
    energia = 99;
    luvas = 0;
    vidas = 3;
    cartoes = false;
}

void levaBolada()
{
    if (luvas > 0) luvas--;
    else if (cartoes)
    {
        if (vermelho) vermelho = false;
        else cartoes = false;
    }
    else energia = MAX(0, energia - 19);
}

void sofreFalta()
{ energia = MAX(0, energia - 50); }

void tomaEnergetico()
{ energia = MIN(energia + 51, 99); }

void escalaGoleiro()
{ luvas = 5; }

void compraArbitro()
{
    if (!cartoes)
    {
        cartoes = true;
        vermelho = false;
    }
    else vermelho = true;
}

void main(void)
{
    char pad;
    bool move, lado = false;	// Flags de animação
    bool menu, completo = false, pausa = false;	// Modos de jogo
    unsigned char dir, nivel;
    unsigned char i = 0, j = 0;
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
        inicializaJogador();
        inicializaAdv(advs);
        inicializaBol(bolas);
        inicializaCar(cards);
        ppu_off();
        vram_adr(NAMETABLE_A);  // LEVAR PARA DENTRO DO LAÇO E CRIAR UM A MAIS PARA MANTER O NIVEL
        vram_unrle(nivel_a);
        scroll(0, 0);
        oam_meta_spr(pos(x), pos(y), CAIM, spr_jogador_parado);
        ppu_on_all();
        while (vidas > 0 && nivel <= NIVEIS)
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
                // Comando de animação
                i++; move = true;
                if (i == 4) i = 0; 
                if (pad & 0xF0 && i == 0) lado = !lado;
                // Controles de direção
                if (pad & PAD_LEFT)
                {
                    dir = 16;
                    if (pad & PAD_UP) dir += 4;
                    else if (pad & PAD_DOWN) dir -= 4;
                }
                else if (pad & PAD_RIGHT)
                {
                    dir = 0;
                    if (pad & PAD_UP) dir -= 4;
                    else if (pad & PAD_DOWN) dir += 4;
                }
                else if (pad & PAD_UP) dir = 24;
                else if (pad & PAD_DOWN) dir = 8;
                else move = false;
                if (move)
                {
                    x += COS(dir); 
                    y += SEN(dir);
                }
                // TESTE DE SAÍDA DEPOIS DE PERDER
                if (pad & PAD_B)
                {
                    limpa_tela(NAMETABLE_A);
                    limpa_tela(NAMETABLE_C);
                    ppu_off();
                    break;
                }
                // Pause
                if (pad & PAD_START)
                {
                    pausa = true;
                    ppu_off();
                    escrita_centralizada("PAUSADO", 2);
                    ppu_on_all();
                }
                // Atualização do quadro
                oam_clear();
                oam_meta_spr(pos(x), pos(y), CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado);
                ppu_wait_nmi();
                if (pausa) delay(100);
            }
        }
        if (vidas <= 0) ; // Game Over
        else if (nivel > NIVEIS) ; // Vitória
    }
}