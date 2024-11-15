#include <stdlib.h>
#include <string.h>
#include <nes.h>
#include "neslib.h"
#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
#include "bcd.h"	// BCD arithmetic support
//#link "bcd.c"
//#link "famitone2.s"
#include "funcoes.h"	// Funcoes úteis
//#link "funcoes.c"
#include "objetos.h"    // Definição dos atores do jogo
//#link "objetos.c"

// Importação dos recursos gráficos
//#resource "chr_porco.chr"
//#link "tileset.s"
#include "sprites.h"
//#link "sprites.c"
//#resource "titulo.h"
#include "niveis.h"

// Importação dos recursos de áudio
//#link "abertura.s"
//#link "trilha.s"

#define temCartao(void) bonus & TEM_CARTAO
#define cVermelho(void) bonus & CARD_VERM

extern char abertura[];
extern char trilha[];

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const byte PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x19,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };
const byte PAL_EXTRA[16] = { 0x0c,0x16,0x36,0x30,0x0c,0x30,0x17,0x0f,0x0c,0x38,0x27,0x21,0x0c,0x30,0x06,0x0f };
// MAN-COR-TIG-SAN

// Tabela de senos normalizados em 8 bits
const short const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Tabela de níveis do modo demonstração
const byte const demo[6] = {0, 10, 25, 34, 50, 51};

// Objetos
static byte cenario;
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];

// Dados do jogador
static word x, y;
static byte dinheiro;
static byte energia;
static byte luvas;
static byte vidas;

// Dados de presença dos bônus
static byte bonus;

// setup PPU and tables
void setup_graphics() 
{  
    oam_clear();   // clear sprites
    pal_bg(PALETTE);	// set palette colors
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
    bonus = 0x00;
}

void levaBolada()
{
    if (luvas > 0) luvas--;
    else energia = MAX(0, energia - 19);
}

void sofreFalta()
{
    if (bonus & TEM_CARTAO)
    {
        if (cVermelho()) bonus &= ~CARD_VERM;
        else bonus &= ~TEM_CARTAO;
    }
    else energia = MAX(0, energia - 50);
}

void tomaEnergetico()
{ energia = MIN(energia + 51, 99); }

void escalaGoleiro()
{ luvas = 5; }

void compraArbitro()
{
    if (temCartao()) bonus |= CARD_VERM;
    else bonus |= TEM_CARTAO & ~CARD_VERM;
}

// void atira(byte dir); // CRIAÇÃO DO PODER DE CARTÃO

// Inicializações
void inicializaAgentes()
{
    inicializaAdv(advs);
    inicializaBol(bolas);
    inicializaCar(cards);
}

// Verifica se objeto nao bate nos blocos
bool nao_bate_parede(word x, word y)
{
    byte i = pos(x), j = pos(y);
    if (i <= XMIN || i >= XMAX || j <= YMIN || j >= YMAX) return false;
    switch (cenario)	// Valores adaptados dos desenhos criados (usa menos RAM)
    {
      	case 0:
            if (j <= YMIN + 16) return i >= XMIN + 81 && i <= XMAX - 81;
            if (j <= YMIN + 32) 
              	return i <= XMIN + 57 || i >= XMAX - 57 || i >= XMIN + 81 && i <= XMAX - 81;
            if (j >= 112 && j <= 142) return i <= XMIN + 73 || i >= XMAX - 73;
            break;
      	case 1:
        case 2:
      	default: break; // PROVISORIO
    }
    return true;
}

// Atualização do placar a cada quadro
void atualizaPlacar()
{
    placar(vidas, PLV, 1);
    placar(dinheiro, PLD, 2);
    placar(energia, PLE, 2);
    contaLuvas(luvas);
    if (temCartao()) corCartao(cVermelho());
}

void main(void)
{
    char pad;	// Leitura do controle
    bool move, lado = false;	// Flags de animação
    bool menu, completo = false, pausa = false;	// Modos de jogo
    byte dir, nivel, prox;	// Variáveis de andamento do nível
    byte i = 0, j = 0, k;	// Variáveis para uso em laços
    // Debounce do controle, rand, comandos de dificuldade
    word dx, dy;	// Variáveis de deslocamento dos agentes
    set_rand(nesclock());
    // Configurações iniciais de áudio
    famitone_init(abertura);
    sfx_init(NULL);
    nmi_set_callback(famitone_update);
    while (1)   // Loop infinito
    {
        setup_graphics();
        menu = true;
        reset_pulo();
        music_play(0);
        apresentacao();
        music_stop();
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
        j = 0; nivel = 0;
        inicializaJogador();
        while (vidas > 0 && nivel <= NIVEIS)
        {
            // Preparando o início do nível
            // Apresentaçao = nivel xx
            inicializaAgentes();
            famitone_init(trilha);
            music_play(0);
            for (k = 0; k < N_ADVS; k++) // LAÇO DE TESTE
            {
                if (k == 9) advs[k].ativo = true;
                advs[k].x = (40 + 8 * k) << 8;
                advs[k].y = (60 + 4 * k) << 8;
                advs[k].energia = 100;
            }
            ppu_off();
            vram_adr(NAMETABLE_A);
            vram_unrle(nivel_a);
            cenario = 0;
            scroll(0, 0);
            oam_meta_spr(pos(x), pos(y), CAIM, spr_jogador_parado);
            ppu_on_all();
            while (prox == false && energia > 0)
            {
                pad = pad_poll(0);
                if (pausa && pad & PAD_START)
                {
                    pausa = false;
                    ppu_off();
                    escrita_centralizada("       ", 2);
                    ppu_on_all();
                    espera(5);
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
                        if (pad & PAD_UP) dir = 28;
                        else if (pad & PAD_DOWN) dir += 4;
                    }
                    else if (pad & PAD_UP) dir = 24;
                    else if (pad & PAD_DOWN) dir = 8;
                    else move = false;
                    if (move)
                    {
                        dx = COS(dir); dy = SEN(dir);
                        if (nao_bate_parede(x + dx, y)) x += dx;
                        if (nao_bate_parede(x, y + dy)) y += dy;
                    }
                    // TESTE DE SAÍDA DEPOIS DE PERDER
                    if (pad & PAD_B){
                      	vidas = 0;
                        energia = 0;
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
                    atualizaPlacar();
                    energia = direcao(x - advs[9].x, y - advs[9].y);
                    oam_meta_spr(XTACA, YTACA, TACA, spr_liberta);
                    oam_meta_spr(pos(x), pos(y), CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado);
                    for (k = 0; k < N_ADVS; k++)
                      	if (advs[k].ativo) oam_spr(pos(advs[k].x), pos(advs[k].y) , JADV, 0, ADV + 4*k);
                    ppu_wait_nmi();
                    if (pausa) espera(100);
                }
            }
            vidas--; // ENERGIA CAIU (ENEL)
        }
        music_stop();
        if (vidas <= 0) // Game Over
        {
            limpa_tela(NAMETABLE_A);
	    game_over();
            pad = pad_poll(0);
            while (!(pad & PAD_START))
            {
              	pad = pad_poll(0);
                ppu_wait_nmi();
            }
            limpa_tela(NAMETABLE_A);
            limpa_tela(NAMETABLE_C);
        } 
        else ; // Vitória
    }
}
