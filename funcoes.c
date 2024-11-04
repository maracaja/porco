#include "neslib.h"
#include <string.h>
#include "vrambuf.h"
#include "sprites.h"
#include "titulo.h"
#include "funcoes.h"

#define desenha_tia(void) oam_meta_spr(TTX, TTY, EXTRA, spr_titia)
#define desenha_jogador_intro(void) oam_meta_spr(SPRX, TTY + 18, CAIM, spr_jogador_parado)
#define ESPACO " "
#define COL_INTRO 5

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
    for (i = 0; i < 3; i++) delay(127);
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

void troca_spr_intro()
{
    ppu_off();
    oam_clear();
    desenha_jogador_intro();
    desenha_tia();
    ppu_on_all();
}

void transicao_intro()
{
    if (!*pulo)
    {
        delay(60);
        limpa_tela(NAMETABLE_A);
        troca_spr_intro();
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
                                    ESPACO,
                                    "TE CONTRATEI PARA NOS",
                                    "LIBERTAR DESSA MALDIcAO.",
                                    ESPACO,
                                    "USE OS GOLEIROS PARA SE",
                                    "PROTEGER DOS ATAQUES DOS",
                                    "ADVERSARIOS.",
                                    ESPACO, 
                                    "CONSEGUI TAMBEM ARBITROS",
                                    "QUE TE AJUDARAO A TIRAR",
                                    "NOSSOS RIVAIS DA FRENTE.",
                                    ESPACO,
                                    "SEMPRE QUE PRECISAR, USE",
                                    "ENERGETICOS QUE LEMBRAM",
                                    "A NOSSA CONQUISTA DE 51.",
                                    ESPACO,
                                    "LEMBRANDO: USE A GRANA ",
                                    "PRA GANHAR A VIDA, NUNCA",
                                    "O CONTRARIO. E COMO SOU",
                                    "RYCA, VOU VOAR AGORA NO",
                                    "MEU, DIGO, NOSSO AVIAO.",
                                    ESPACO,
                                    "PARA CONQUISTAR O MUNDO,",
                                    "e PRECISO ATRAVESSA-LO.",
                                    "BOA SORTE!"};
    for (i = 0; i < 7; i++)
        escreve_mensagem(trechos[i], 2 * i + 2, COL_INTRO);
    if (!*pulo) oam_meta_spr(TTX, SPRY, TACA, spr_liberta);
    for (; i < 10; i++)
        escreve_mensagem(trechos[i], 2 * i + 2, COL_INTRO);
    transicao_intro();
    if (!*pulo) oam_meta_spr(SPRX, SPRY, GOLEIRO, spr_goleiro);
    for (; i < 14; i++)
        escreve_mensagem(trechos[i], 2 * i - 18, COL_INTRO);
    if (!*pulo)
    {
        troca_spr_intro();
        oam_meta_spr(SPRX, SPRY, JUIZ, spr_arbitro);
    }
    for (; i < 18; i++)
        escreve_mensagem(trechos[i], 2 * i - 18, COL_INTRO);
    if (!*pulo) 
    {
      	troca_spr_intro();
        oam_meta_spr(SPRX, SPRY, ENERG, spr_nrg);
    }
    for (; i < 22; i++)
        escreve_mensagem(trechos[i], 2 * i - 18, COL_INTRO);
    transicao_intro();
    oam_meta_spr(TTX, SPRY, MOEDA, spr_moeda);
    for (; i < 31; i++)
        escreve_mensagem(trechos[i], 2 * i - 42, COL_INTRO);
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
    delay(30);
    for (i = 0; i < 150 && !*pulo; i++)
    {
        pad = pad_poll(0);
        if (pad & PAD_A) *pulo = true;
        ppu_wait_nmi();
    }
    limpa_tela(NAMETABLE_A);
    ppu_off();
    caim_x = 100; caim_y = 210;
    oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador_parado);
    desenha_tia();
    ppu_on_all();
    while (caim_x > TTX - 4 && !*pulo)
    {      
        caim_x--; caim_y -= 2;
        oam_clear();
        oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador(caim_x % 2));
        desenha_tia();
        ppu_wait_nmi();
        // Pula introdução
        pad = pad_poll(0);
        if (pad & PAD_A) *pulo = true;
        delay(4);
    }
    oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador_parado);
    desenha_tia();
    if (!*pulo) conversa();
}
