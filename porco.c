#include <stdlib.h>
#include <string.h>
#include <nes.h>
#include "neslib.h"
#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
#include "bcd.h"	// BCD arithmetic support
//#link "bcd.c"
//#link "famitone2.s"
#include "constantes.h" // Constantes usadas
#include "funcoes.h"	// Biblioteca de funções
//#link "funcoes.c"
#include "objetos.h"    // Definição dos atores do jogo e mais
//#link "objetos.c"

// Importação dos recursos gráficos
//#resource "titulo.h"
//#resource "niveis.h"
//#resource "chr_porco.chr"
//#link "tileset.s"
#include "sprites.h"
//#link "sprites.c"

// Importação dos recursos de áudio
//#link "abertura.s"
//#link "trilha.s"
extern char abertura[];
extern char trilha[];

// MACROS
// Trigonometria
#define SEN(x) senos[(x) & 0x1F]
#define COS(x) SEN((x) + 8)
// Bônus
#define temCartao bonus & TEM_CARTAO
#define cVermelho bonus & CARD_VERM
#define temTaca bonus & TEM_TACA
// Movimento do jogador
#define jogador_dir move & 0x1F
#define jogador_mov move & 0x20
// Condições para surgir bônus na arena
#define nrg_disponivel energia < 67
#define arb_disponivel !(temCartao && cVermelho)
#define gol_disponivel luvas < 3
// Tipo de nível
#define nivel_comum nivel == 0 || nivel % DIVISOR != 0


// Tabela de senos normalizados em 8 bits (x2)
const short const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const byte PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x19,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };
const byte PAL_EXTRA[16] = { 0x0c,0x16,0x36,0x30,0x0c,0x30,0x17,0x0f,0x0c,0x38,0x27,0x21,0x0c,0x30,0x06,0x0f };
// MAN-COR-TIG-SAN

// Tabela de níveis do modo demonstração
const byte const demo[7] = {0, 10, 25, 34, 50, 51, 99};

// Objetos
static byte arena;
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];

// Dados do jogo
static byte nivel;

// Dados do jogador
static word x, y;
static byte dinheiro;
static byte energia;
static byte luvas;
static byte vidas;

// Dados de presença dos bônus
static byte bonus;
static byte y_taca;

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
void posicionaJogador()
{
    x = CX << 8;
    y = CY << 8;
}

void inicializaJogador()
{
    posicionaJogador();
    dinheiro = 0;
    energia = 99;
    luvas = 0;
    vidas = 3;
    bonus = BGOL | BARB | BDIN;
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
        if (cVermelho) bonus &= ~CARD_VERM;
        else bonus &= ~TEM_CARTAO;
    }
    else energia = MAX(0, energia - 50);
}

void tomaEnergetico()
{
    energia = MIN(energia + 51, 99);
    bonus &= ~BNRG;
}

void escalaGoleiro()
{
    luvas = 5;
    bonus &= ~BGOL;
}

void compraArbitro()
{
    if (temCartao) bonus |= CARD_VERM;
    else bonus |= TEM_CARTAO & ~CARD_VERM;
    bonus &= ~BARB;
}

void ganhaDinheiro()
{
    dinheiro++;
    if (dinheiro > 99)
    {
        vidas++;
        dinheiro = 0;
    }
    bonus &= ~BDIN;
}

// void atira(byte dir); // CRIAÇÃO DO PODER DE CARTÃO

bool pegou_taca()
{
    byte i = pos(x), j = pos(y);
    return i >= XTACA - 9 && i <= XTACA + 5 && j >= y_taca - 21 && j <= y_taca + 13 && temTaca;
}

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
    // Valores adaptados dos desenhos criados (usa menos RAM)
    if (arena <= 2)
    {	// Casos comuns aos níveis normais
        if (j <= YMIN + 16) return i >= XMIN + 81 && i <= XMAX - 81;
      	if (j <= YMIN + 32) 
            return i <= XMIN + 57 || i >= XMAX - 57 || i >= XMIN + 81 && i <= XMAX - 81;
    }
    switch (arena)	
    {	// Casos específicos
      	case 0:
            if (j >= YMAX - 16) return false;
            if (j >= 112 && j <= 142) return i <= XMIN + 73 || i >= XMAX - 73;
            break;
      	case 1:
            if (j >= 128 && j <= 158) return i >= XMIN + 65 && i <= XMAX - 65;
            break;
        case 2:
            if (j >= 80 && j <= 110 || j >= YMAX - 32 && j < YMAX - 16)
              	return i >= XMIN + 17 && i <= XMAX - 17;
            if (j >= 152 && j <= 182)
                return i <= XMIN + 41 || i >= XMAX - 41 || i >= XMIN + 81 && i <= XMAX - 81;
            if (j >= YMAX - 16) return i >= XMIN + 33 && i <= XMAX - 33;
            break;
      	default:
            if (j <= YMIN + 16) return false;
            if (j >= 160 && j <= 190) return i >= XMIN + 49 && i <= XMAX - 49;
    }
    return true;
}

void troca_paleta(byte cor)
{
    byte i;
    for (i = 0; i < 4; i++) pal_col(24 + i, PAL_EXTRA[(cor << 2) | i]);
}

// Alternância de paleta conforme o nível
void pal_adv()
{
    pal_spr(PALETTE);
    troca_paleta(nivel & 0x03);
}

// Atualização do placar a cada quadro
void atualizaPlacar()
{
    placar(vidas, PLV, 1);
    placar(dinheiro, PLD, 2);
    placar(energia, PLE, 2);
    contaLuvas(luvas);
    if (temCartao) corCartao(cVermelho);
}

void main(void)
{
    char pad;	// Leitura do controle
    bool lado = false;	// Flags de animação
    byte move, adv;
    bool menu, completo = false, pausa = false;	// Modos de jogo
    byte prox;	// Variáveis de andamento do nível
    byte i = 0, j, k;	// Variáveis para uso em laços
    // ********* Debounce do controle, rand, comandos de dificuldade
    word dx, dy;	// Variáveis de deslocamento dos agentes
    set_rand(nesclock());
    // Configurações iniciais de áudio
    famitone_init(abertura);
    sfx_init(NULL);
    nmi_set_callback(famitone_update);
    while (1)
    {	// Loop infinito
        setup_graphics();
        menu = true;
        reset_pulo();
        music_play(0);
        apresentacao();
        music_stop();
        selecao(completo);
        while (menu)
        {   // Controle do menu de jogo
            pad = pad_poll(0);
            if (pad & PAD_DOWN && !completo) selecao(completo = true);
            if (pad & PAD_UP && completo) selecao(completo = false);
            if (pad & PAD_A) menu = false;
        }
        // Início da história
        setup_graphics();
        historinha();
        limpa_tela(NAMETABLE_A);
        //reset_pulo();
        nivel = completo ? 0 : demo[j = 0];
        inicializaJogador();
        while (vidas > 0 && nivel <= NIVEIS)
        {   // Loop do jogo
            // Início do nível
            // ************** Apresentaçao = nivel xx
            posicionaJogador();
            inicializaAgentes();
            // *************** Posicionar adversários (algo como abaixo)
            for (k = 0; k < N_ADVS; k++) // ************* LAÇO DE TESTE
            {
                if (k == 9) advs[k].ativo = true;
                advs[k].x = (40 + 8 * k) << 8;
                advs[k].y = (60 + 4 * k) << 8;
                advs[k].energia = 100;
            }
            
            
            // Define parâmetros de nível
            if (nivel_comum)
            {
                adv = spr_adv(nivel); // Adversários
            	pal_adv();
              	bonus |= TEM_TACA;   // Taça (fim de nível)
                y_taca = YTACA;
            }
            else
            {
                bonus &= ~TEM_TACA;
                y_taca = YTACA + 32;
                // ************** Aqui eu devo configurar os vilões 
            }
            famitone_init(trilha);
            music_play(0);
            ppu_off();
            arena = carrega_arena(nivel);
            scroll(0, 0);
            oam_meta_spr(pos(x), pos(y), CAIM, spr_jogador_parado);
            ppu_on_all();
            while (prox == false && energia > 0)
            {	// Loop do nível antes de passar ou perder vida
                pad = pad_poll(0);
                if (pausa && pad & PAD_START)
                {   // Sai da pausa
                    pausa = false;
                    ppu_off();
                    escrita_centralizada("       ", 2);
                    ppu_on_all();
                    espera(5);
                }
                else if (!pausa)
                {   // Regime normal do jogo
                    // Comando de animação
                    i++;
                    if (i == 4) i = 0;
                    if (pad & 0xF0 && i == 0) lado = !lado;
                    // Controle do deslocamento do jogador na tela
                    move = movimento(pad);
                    if (jogador_mov)
                    {
                        dx = COS(jogador_dir);
                      	dy = SEN(jogador_dir);
                        if (nao_bate_parede(x + dx, y)) x += dx;
                        if (nao_bate_parede(x, y + dy)) y += dy;
                    }
                    if (pad & PAD_START)
                    {	// Pausa
                        pausa = true;
                        ppu_off();
                        escrita_centralizada("PAUSADO", 2);
                        ppu_on_all();
                    }
                    if (pegou_taca()) prox = true;
                    // *** Captar possíveis interações
                    // *************** Falta, bolada, cartão, pegou bônus
                    // *** Rotinas aleatórias
                    // **************** Chute, movimentação do inimigo, bônus
                    // Atualização do quadro
                    oam_clear();
                    atualizaPlacar();
                    //dinheiro = pos(y);
                    //dinheiro = direcao(x - advs[9].x + 4, y - advs[9].y + 4);//TESTE
                    
                    if (temTaca) oam_meta_spr(XTACA, y_taca, TACA, spr_liberta);
                    oam_meta_spr(pos(x), pos(y), CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado);
                    for (k = 0; k < N_ADVS; k++)
                      	if (advs[k].ativo) oam_spr(pos(advs[k].x), pos(advs[k].y), adv, 2, ADV + 4*k);
                    ppu_wait_nmi();
                    if (pausa) espera(100);
                }
            }
            music_stop();
            if (energia <= 0)	// Morreu
            {	
            	vidas--;
                energia = 99;
            }
            else // Passou de nível
            {
                nivel = completo ? nivel + 1 : demo[++j];
                prox = false;
            }
        }
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