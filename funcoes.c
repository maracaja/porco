#include "neslib.h"
#include "vrambuf.h"
#include "funcoes.h"
#include <string.h>
#include "titulo.h"

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
    unsigned char x = col, y = lin, tam = MIN(strlen(msg), 28), i = 0;
    // Repete ate encontrar caractere 0x00 ('\0')
    while (i < tam) 
    {
        vrambuf_put(NTADR_A(x + i, y), &msg[i], 1);
        ppu_wait_nmi();
        set_vram_update(updbuf);
        i++;
        delay(5);
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
    const char* trechos[5] = { "ESTA e UMA OBRA DE FICcAO.",
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