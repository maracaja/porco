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
#define SEN(x) (senos[(x) & 0x1F])
#define COS(x) SEN((x) + 8)
// Bônus
#define temCartao (bonus & TEM_CARTAO)
#define cVermelho (bonus & CARD_VERM)
#define temTaca (bonus & TEM_TACA)
// Movimento do jogador
#define jogador_dir (move & 0x1F)
#define jogador_mov (move & 0x20)
// Condições para surgir bônus na arena
#define nrg_disponivel (energia < 60)
#define arb_disponivel !(temCartao && cVermelho)
#define gol_disponivel (luvas < 3)
#define din_disponivel (vidas < 9 || energia < 99)
// Tipo de nível
#define nivel_comum (nivel == 0 || nivel % DIVISOR != 0)
#define veloc (nivel > 25 ? 3 : 2)

// Tabela de senos normalizados em 8 bits (x2)
const short const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const byte PALETTE[16] = { 0x0C,0x0F,0x30,0x16,0x0C,0x19,0x36,0x30,0x0C,0x04,0x36,0x07,0x0C,0x27,0x10,0x38 };
const byte PAL_EXTRA[20] = { 0x0c,0x16,0x36,0x30,0x0c,0x30,0x17,0x0f,0x0c,0x38,0x27,0x21,0x0c,0x30,0x06,0x0f,0x0c,0x30,0x36,0x16 };

// Tabela de níveis do modo demonstração
const byte const demo[7] = {0, 10, 25, 34, 50, 51, 99};

// Objetos
static byte arena;
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];

// Dados do jogo
static char pad;  // Leitura do controle
static byte nivel;
static byte deb;  // Delay (em quadros) a cada tiro
static byte dec;  // Delay (em quadros) a cada chute
//static byte rec;  // Delay (em quadros) da falta
static byte ata;   // Jogador que vai chutar (estava sendo o mesmo)
static byte poup;  // Quantidade de moedas que podem aparecer

// Dados do jogador
static word x, y;
static byte dinheiro;
static byte energia;
static byte luvas;
static byte vidas;
static byte move;
static bool lado = false;  // Flag de animação

// Dados de presença dos bônus
static byte bonus;
static byte y_taca;
// Posições onde se localizarão (ordem de definição das respectivas constantes)
static byte xb[4] = {91, 157, 128, 128};	
static byte yb[4] = {150, 150, 150, 150};

// setup PPU and tables
void setup_graphics() 
{  
    oam_clear();   // clear sprites
    pal_bg(PALETTE);	// set palette colors
    pal_spr(PALETTE);
}

// Funções referentes ao jogador
void posicionaJogador()
{
    x = real(CX);
    y = real(CY);
}

void inicializaJogador()
{
    posicionaJogador();
    dinheiro = 0;
    energia = 99;
    luvas = 0;
    vidas = 3;
    bonus = BGOL | BARB | BDIN;
    poup = 0;
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

byte ativaCartao()
{
    byte i;
    for (i = 0; i < N_CARDS; i++)
    {
        if (!card_ativo(cards[i])) 
        {
            cards[i].info = CARD_ATIVO | (cVermelho ? CARD_VERM : 0);
            return i;
        }
    }
    return 0;
}

void desativaCartao(byte i)
{ cards[i].info &= ~CARD_ATIVO; }

void atira()
{
    byte i = ativaCartao();
    if (jogador_mov && jogador_dir == 8)
    {
        cards[i].y = y;
        if (pos(x) > 126)
        {
            cards[i].x = x - real(10);
            cards[i].info |= 0x10;
        }
        else cards[i].x = x + real(10);
    }
    else
    {
        cards[i].x = x;
        cards[i].y = y - real(16);
        cards[i].info |= !jogador_mov || jogador_dir > 16 ? 24 : (jogador_dir < 8 ? 28 : 20);
    }
    deb = 0;
}

byte ativaBola()
{
    byte i;
    for (i = 0; i < N_BOLAS; i++)
    {
        if (!bolas[i].ativo)
        {
            bolas[i].ativo = true;
            return i;
        }
    }
    return 0;
}

void chuta(byte a)
{
    if (pode_chutar(nivel))
    {
        byte i = ativaBola();
        bolas[i].x = advs[a].x;
        bolas[i].y = advs[a].y + real(8);
        bolas[i].dir = direcao(DIV2(x) - DIV2(bolas[i].x), DIV2(y) - DIV2(bolas[i].y));  
    }
    dec = 0;
}

bool pegou_taca()
{
    byte i = pos(x), j = pos(y);
    return i >= XTACA - 9 && i <= XTACA + 5 && j >= y_taca - 21 && j <= y_taca + 13 && temTaca;
}

// Efeito do tiro sobre o adversário
void levaCartao(byte a, byte c, unsigned char nivel)
{
    switch (nivel / DIVISOR)
    {
        case 0: advs[a].energia -= (vermelho(cards[c]) ? 100 : 50); break;
        case 1: advs[a].energia -= (vermelho(cards[c]) ? 66 : 34); break;
        default: advs[a].energia -= (vermelho(cards[c]) ? 50 : 25);
    }
    if (advs[a].energia <= 0) advs[a].ativo = false;
    cards[c].info &= ~CARD_ATIVO;
}

// Alternância de paleta conforme o nível
void troca_paleta(byte cor)
{
    byte i;
    for (i = 0; i < 4; i++) pal_col(24 + i, PAL_EXTRA[(cor << 2) | i]);
}

void pal_adv()
{  troca_paleta(nivel % 5); }

// Define o metasprite do adversário a ser usado
byte* spr_adv(byte nivel, bool lado)
{
    switch (nivel % 5)
    {
        case 0:
      	case 1:
            return spr_adv1(lado);
      	case 3:
            return spr_adv3(lado);
        default: return spr_adv2(lado);
    }
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

// Atualização dos sprites a cada quadro
void atualizaSprites()
{
    byte i, j, xa, ya, xj = pos(x), yj = pos(y);
    short dx, dy;
    oam_meta_spr(xj, yj, CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado); // Jogador
    if (temTaca) oam_meta_spr(XTACA, y_taca, TACA, spr_liberta); // Taça
    // Disparos inimigos
    for (i = 0; i < N_BOLAS; i++)
    {
        if (bolas[i].ativo)
	{
	    dx = veloc * COS(bolas[i].dir);
	    dy = veloc * SEN(bolas[i].dir);
            if (ABS(xj - pos(bolas[i].x)) < 8 && pos(bolas[i].y) > yj - 16 && pos(bolas[i].y) < yj + 8)
            {
                levaBolada();
                bolas[i].ativo = false;
            }
	    else if (nao_bate_parede(arena, bolas[i].x + dx, bolas[i].y + dy, false))
	    {
		bolas[i].x += dx;
                bolas[i].y += dy;
	    }
	    else bolas[i].ativo = false;
	    oam_spr(pos(bolas[i].x), pos(bolas[i].y), BOLA, 0, TIRO + 4 * i);
	}
    }    
    // Cartões
    for (i = 0; i < N_CARDS; i++)
    {
    	if (card_ativo(cards[i]))
        {
            dx = 3 * COS(card_dir(cards[i]));
            dy = 3 * SEN(card_dir(cards[i]));
	    for (j = 0; j < N_ADVS; j++)
	    {
		if (advs[j].ativo)
		{
		    xa = pos(advs[j].x);
		    ya = pos(advs[j].y);
		    if (ABS(xa - pos(cards[i].x)) <= 6 && pos(cards[i].y) > ya - 14 && pos(cards[i].y) < ya + 6)
		    {
			levaCartao(j, i, nivel);
		    }
		}	
	    }
            if (nao_bate_parede(arena, cards[i].x + dx, cards[i].y + dy, false))
            {
                cards[i].x += dx;
                cards[i].y += dy;
            }
            else desativaCartao(i);
            oam_spr(pos(cards[i].x), pos(cards[i].y), CARD, vermelho(cards[i]) ? 0 : 3, CARTAO + 4 * i);
        }
    }
    // Adversários
    for (i = 0; i < N_ADVS; i++)
    {
    	if (advs[i].ativo)
        {
	    // Jogador tenta se posicionar entre o jogador e a taça
	    // DEFINIR ÁREAS DE ATUAÇÃO ANTES
	    // direcao(x - real(XTACA), y - real(y_taca)) <> direcao(advs[i].x - real(XTACA), advs[i].y - real(y_taca))
            oam_meta_spr(pos(advs[i].x), pos(advs[i].y), ADV + 8 * i, spr_adv(nivel, lado));
            
        }
    }
    // Bônus a coletar
    if (bonus & BGOL)
    {
        if (ABS(xj - xb[0]) < 8 && ABS(yj - yb[0]) < 16)
        {
            escalaGoleiro();
            bonus &= ~BGOL;
        }
      	oam_meta_spr(xb[0], yb[0], GOLEIRO, spr_goleiro);
    }
    if (bonus & BARB)
    {
      	if (ABS(xj - xb[1]) < 8 && ABS(yj - yb[1]) < 16)
        {
            compraArbitro();
            bonus &= ~BARB;
        }
      	oam_meta_spr(xb[1], yb[1], JUIZ, spr_arbitro);
    }
    if (bonus & BNRG)
    {
      	if (ABS(xj - xb[2]) < 7 && ABS(yj - yb[2]) < 15)
        {
            tomaEnergetico();
            bonus &= ~BNRG;
        }
      	oam_meta_spr(xb[2], yb[2], ENERG, spr_nrg);
    }
    if (bonus & BDIN)
    {
        if(xj - xb[3] < 7 && xb[3] - xj < 15 && ABS(yj - yb[3]) < 15)
        {
            ganhaDinheiro();
            bonus &= ~BDIN;
        }
      	oam_meta_spr(xb[3], yb[3], MOEDA, spr_moeda);
    }
    // Prepara novo chute
    if (advs[ata].ativo && dec > DELAY_CHUTE) chuta(ata);
    else ata = (ata >= N_ADVS) ? 0 : ata + 1;
}

// Inicializações
void inicializaAgentes()
{
    byte i;
    ata = 0;
    inicializaAdv(advs);
    inicializaBol(bolas);
    inicializaCar(cards);
    // Posicionamento inicial dos adversários
    for (i = 0; i < N_ADVS; i++)
    {
        advs[i].ativo = true;
      	advs[i].energia = 100;
      	advs[i].x = real(64 + 60 * (i % 3));
      	advs[i].y = real(YMIN + (i < 3 ? 40 + 5 * arena : 70 + 15 * arena));
    }
}

void main(void)
{
    bool menu, completo = false, pausa = false;	// Modos de jogo
    byte prox;	// Variáveis de andamento do nível
    byte i = 0, j;	// Variáveis para uso em laços
    short dx, dy;	// Variáveis de deslocamento do jogador
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
            if (pad & PAD_A)
            {
                menu = false;
                srand(nesclock());
            }
        }
        // Início da história
        setup_graphics();
        historinha();
        limpa_tela(NAMETABLE_A);
        nivel = completo ? 0 : demo[j = 0];
        inicializaJogador();
        while (vidas > 0 && nivel <= NIVEIS)
        {   // Loop do jogo
            // Início do nível
            entrada_nivel(nivel);
            famitone_init(trilha);
            music_play(0);
            ppu_off();
            posicionaJogador();
            arena = carrega_arena(nivel);
            scroll(0, 0);
            // Define parâmetros de nível
            if (nivel_comum)
            {
                inicializaAgentes();
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
                    music_pause(0);
                } 
                else if (!pausa)
                {   // Regime normal do jogo
                    // Comando de animação
                    i++;
                    if (i >= 5)
                    {
                        i = 0;
                    	lado = !lado;
                    }
                    // Controle do deslocamento do jogador na tela
                    move = movimento(pad);
                    if (jogador_mov)
                    {
                        dx = COS(jogador_dir);
                      	dy = SEN(jogador_dir);
                        if (nao_bate_parede(arena, x + dx, y, true)) x += dx;
                        if (nao_bate_parede(arena, x, y + dy, true)) y += dy;
                    }
                    // Atira cartão (se tiver esse poder)
                    if (pad & PAD_A && temCartao && deb > DEBOUNCE) atira();
                    if (pad & PAD_START)
                    {	// Pausa
                        pausa = true;
                        ppu_off();
                        escrita_centralizada("PAUSADO", 2);
                        music_pause(*trilha);
                        ppu_on_all();
                    }
                    if (pegou_taca())
                    {
                        ppu_off();
                        music_stop();
                        oam_clear();
                        escrita_centralizada("PARABENS!", 8);
                        escrita_centralizada("VOCE PASSOU DE NIVEL!", 10);
                        prox = true;
                        ppu_on_all();
                        espera(300);
                    }
                    // *** Captar possíveis interações
                    // *************** Falta, bolada, cartão, pegou bônus
                    // *** Rotinas aleatórias
                    // **************** , movimentação do inimigo (em campo próprio), bônus
                    // Atualização do quadro
                    oam_clear();
                    atualizaPlacar();
                    atualizaSprites();
                    if (deb <= DEBOUNCE) deb++;
                    if (dec <= DELAY_CHUTE) dec++;
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
                nivel = (completo ? nivel + 1 : demo[++j]);
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
