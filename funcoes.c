#include "neslib.h"
#include "funcoes.h"
#include <string.h>

void escrita_centralizada(const char* str, unsigned char linha)
{
    unsigned char i, tam, col;
    tam = strlen(str);
    col = 16 - tam / 2 - tam % 2;
    vram_adr(NTADR_A(col, linha));
    vram_write(str, tam);
}

void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col)
{
    char ch;
    unsigned char x = col, y = lin;
    // Repete ate encontrar caractere 0x00 ('\0')
    while ((ch = *msg++) || y <= 30) {
        vrambuf_put(getntaddr(x, y), &ch, 1);
        x++;
    }
    vrambuf_flush();
    delay(5);
}