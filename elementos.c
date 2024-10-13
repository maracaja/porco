#include "neslib.h"

void disclaimer()
{
    unsigned char i, tam, col;
    const char* trechos[5] = { "ESTA e UMA OBRA DE FICCAO.",
                              "QUALQUER SEMELHANCA COM",
                              "NOMES, PESSOAS, FATOS OU",
                              "CLUBES DE FUTEBOL NAO PASSA",
                              "DE MERA COINCIDENCIA." };
    for (i = 0; i < 5; i++)
    {
        tam = strlen(trechos[i]);
        col = 16 - tam / 2 - tam % 2;
        vram_adr(NTADR_A(col, 2 * i + 10));
        vram_write(trechos[i], tam);
    }
}