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
    unsigned char x = col, y = lin, tam = strlen(msg), i = 0;
    // Repete ate encontrar caractere 0x00 ('\0')
    while (i <= tam) 
    {
        //ppu_off();
        //set_vram_update(NULL);
        //vram_adr(NTADR_A(x + i, y));
        //vram_put(msg[i]);
        vrambuf_put(NTADR_A(x + i, y), &msg[i], 1);
        flush_vram_update(updbuf);
        //vrambuf_flush();
        i++;
        //ppu_on_all();
        delay(5);
    }
}