#include "neslib.h"
#include <string.h>
#include "vrambuf.h"
#include "sprites.h"
#include "titulo.h"
#include "funcoes.h"

//extern const unsigned char titulo[];

void escrita_centralizada(const char* str, unsigned char linha)
{
    unsigned tam, col;
    tam = strlen(str);
    col = 16 - tam / 2 - tam % 2;
    vram_adr(NTADR_A(col, linha));
    vram_write(str, tam);
}

void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col)
{
    unsigned char x = col, y = lin, tam = MIN(strlen(msg), 28), i = 0, j = 0;
    char pad;
    while (!*pulo && i < tam) 
    {
        if (j >= 4)
        {
            vrambuf_put(NTADR_A(x + i, y), &msg[i], 1);
            set_vram_update(updbuf);
            i++; j = 0;
            vrambuf_flush();
        }
        else
        {
            ppu_wait_nmi();
            j++;
        }
        // Sai do diálogo se o jogador pressionar A
        pad = pad_poll(0);
        if (pad & PAD_A) *pulo = true;
    }
}

void limpa_tela(unsigned int adr)
{
    ppu_off();
    oam_clear();
    set_vram_update(NULL);
    vram_adr(adr);
    vram_fill(0, 1024);
    ppu_on_all();
}

void disclaimer()
{
    byte i;
    const char* const trechos[5] = { "ESTA e UMA OBRA DE FICcAO.",
                              	     "QUALQUER SEMELHANcA COM",
                                     "NOMES, PESSOAS, FATOS OU",
                                     "CLUBES DE FUTEBOL NAO PASSA",
                                     "DE MERA COINCIDENCIA." };
    for (i = 0; i < 5; i++) 
        escrita_centralizada(trechos[i], 2 * i + 10);
    ppu_on_all();
    delay(255);
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

void conversa()
{
    byte i;
    const char* const trechos[] = { "OLA, CAIM! SOU A TITIA,",
                                    "DONA E PATROCINADORA DO",
                                    "PORCARIAS FC. FUI PRESA",
                                    "PELO TERRIVEL FUTEBOWSER",
                                    "NUMA REALIDADE ONDE MEU ",
                                    "TIME NAO TEM MUNDIAL.",
                                    "TE CONTRATEI PARA NOS",
                                    "LIBERTAR DESSA MALDIcAO.",
                                    " " };
    for (i = 0; i < 9; i++)
        escreve_mensagem(trechos[i], 2 * i + 2, 5);
}

void historinha()
{
    unsigned char i;
    unsigned char caim_x, caim_y; // Coordenadas do personagem principal
    char pad;
    limpa_tela(NAMETABLE_C);
    limpa_tela(NAMETABLE_A);
    scroll(0,0);
    ppu_off();
    escrita_centralizada("BROOKLIN", 13);
    escrita_centralizada("1984", 15);
    ppu_on_all();
    delay(180);
    limpa_tela(NAMETABLE_A);
    ppu_off();
    pal_bright(NORMAL);
    ppu_on_all();
    limpa_tela(NAMETABLE_A);
    ppu_off();
    caim_x = 111; caim_y = 224;
    oam_meta_spr(caim_x, caim_y, CAIM, spr_caim_parado);
    desenha_tia();
    ppu_on_all();
    for (i = 0; i <= 86 && !*pulo; i++)
    {      
        caim_x--; caim_y -= 2;
        oam_clear();
        desenha_tia();
        oam_meta_spr(caim_x, caim_y, CAIM, spr_caim(i % 2));
        ppu_wait_nmi();
        // Pula introdução
        pad = pad_poll(0);
        if (pad & PAD_A) *pulo = true;
        delay(4);
    }
    desenha_tia();
    oam_meta_spr(caim_x, caim_y, CAIM, spr_caim_parado);
    conversa();
}
