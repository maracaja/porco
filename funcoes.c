#include "neslib.h"
#include "vrambuf.h"
#include "funcoes.h"
#include <string.h>

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