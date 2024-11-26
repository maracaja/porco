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
//#link "trilha_viloes.s"
//#link "game_over_1.s"
extern char abertura[];
extern char trilha[];
extern char trilha_fifa[];
extern char tema_fim[];

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
#define gol_disponivel (luvas < 3)
#define arb_disponivel !(temCartao && cVermelho)
#define nrg_disponivel (energia < 60)
#define din_disponivel (vidas < 9 || energia < 99)
// Tipo de nível
#define nivel_comum (nivel % DIVISOR != 0 || nivel == 0)
#define veloc (nivel > 25 ? 2 : 1)

// Tabela de senos normalizados em 8 bits (x2)
const short const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const byte PALETTE[16] = { 0x01,0x0F,0x30,0x16,0x01,0x19,0x36,0x30,0x01,0x04,0x36,0x07,0x01,0x27,0x10,0x38 };
const byte PAL_EXTRA[20] = { 0x01,0x16,0x36,0x30,0x01,0x30,0x17,0x0f,0x01,0x38,0x27,0x21,0x01,0x30,0x06,0x0f,0x01,0x30,0x36,0x16 };

// Tabela de níveis do modo demonstração
const byte const demo[7] = {/*0, 10, 25, */34, 50, 51, 99};

// Objetos
static byte arena;
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];
Vilao v;

// Dados do jogo
static char pad;  // Leitura do controle
static byte nivel;
static byte deb;  // Delay (em quadros) a cada tiro
static byte dec;  // Delay (em quadros) a cada chute
static byte rec;  // Delay (em quadros) entre dois choques do jogador contra oponentes
static byte ata;   // Jogador que vai chutar (estava sendo o mesmo)
static byte banco; // Número máximo de adversários por nível 
static byte res;   // Tempo (em quadros) para aparecer o reserva
static byte poup;  // Quantidade de moedas que podem aparecer
static word tbo[4];  // Conta o tempo que o bônus aparece na tela
static word dbo[4];  // Tempo sem bônus na tela

// Dados do jogador
static word x, y;
static byte dinheiro;
static byte energia;
static byte luvas;
static byte vidas;
static byte move;
static bool lado = false;  // Flag de animação
static bool corre = false;

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
    if (gol_disponivel) dbo[0] = 0;
}

void sofreFalta()
{
    if (rec > RECUPERACAO)
    {
        if (bonus & TEM_CARTAO)
        {
            if (cVermelho) bonus &= ~CARD_VERM;
            else bonus &= ~TEM_CARTAO;
        }
        else energia = MAX(0, energia - 50);
        posicionaJogador();
        rec = 0;
        dbo[1] = 0; dbo[2] = 0;
    }
}

void tomaEnergetico()
{
    energia = MIN(energia + 51, 99);
    bonus &= ~BNRG;
}

void escalaGoleiro()
{
    luvas = nivel < 51 ? 5 : 2;
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
        cards[i].info |= (!jogador_mov || jogador_dir > 16) ? 24 : (jogador_dir < 8 ? 28 : 20);
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
        bolas[i].dir = direcao2(x, bolas[i].x, y, bolas[i].y);  
    }
    dec = 0;
}

bool pegou_taca()
{
    byte i = pos(x), j = pos(y);
    return i >= XTACA - 9 && i <= XTACA + 5 && j >= y_taca - 21 && j <= y_taca + 13 && temTaca;
}

// Efeito do tiro sobre o adversário
void levaCartao(byte a, byte c)
{
    switch (nivel / DIVISOR)
    {
        case 0: advs[a].energia -= (vermelho(cards[c]) ? 100 : 50); break;
        case 1: advs[a].energia -= (vermelho(cards[c]) ? 66 : 34); break;
        default: advs[a].energia -= (vermelho(cards[c]) ? 50 : 25);
    }
    if (advs[a].energia <= 0)
    {
      	advs[a].ativo = false;
        res = 0;
        poup++;
    }
    desativaCartao(c);
}

void levaCartaoVilao(byte c)
{
    v.energia -= vermelho(cards[c]) ? 2 : 1;
    if (v.energia <= 0)
    {
        v.ativo = false;
        bonus |= TEM_TACA;
    }
    desativaCartao(c);
}

// Alternância de paleta conforme o nível
void troca_paleta(byte cor)
{
    byte i;
    for (i = 0; i < 4; i++) pal_col(24 + i, PAL_EXTRA[(cor << 2) | i]);
}

void pal_adv()
{
    switch (nivel)
    {
      	case 17: troca_paleta(2); break;
        case 34: troca_paleta(3); break;
        case 51: troca_paleta(0); break;
      	default: troca_paleta(nivel % 5);
    }
}

// Define o metasprite do adversário a ser usado
byte* spr_adv()
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

// Cria um novo adversário na tela
void novoAdversario(byte i)
{
    if (res >= RESERVA) 
    {
        advs[i].ativo = true;
        advs[i].energia = 100;
        advs[i].x = i < 3 ? real(98 + 24 * (i % 3)) : real(64 + 60 * (i % 3));
        advs[i].y = real(YMIN + (i < 3 ? 40 : 70 + 15 * arena));
        banco--;
    }
}

// Função para adversário correr atrás do jogador
void correAtras(byte i)
{
    if (advs[i].ativo)
    {
        byte da = direcao2(x, advs[i].x, y, advs[i].y);
        short dx = COS(da) << 1, dy = SEN(da) << 1; 
        if (nao_bate_parede(arena, advs[i].x + dx, advs[i].y, true)) advs[i].x += dx;
        if (nao_bate_parede(arena, advs[i].x, advs[i].y + dy, true)) advs[i].y += dy;
    }
}

// Atualização do placar a cada quadro
void atualizaPlacar()
{
    placar(vidas, PLV, 1);
    placar(dinheiro, PLD, 2);
    placar(energia, PLE, 2);
    placar(luvas, PLL, 1);
    if (temCartao) corCartao(cVermelho);
}

// Atualização dos sprites a cada quadro
void atualizaSprites()
{
    byte i, j;
    short dx, dy, xa, ya, xc, yc, xj = pos(x), yj = pos(y);
    oam_meta_spr(xj, yj, CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado); // Jogador
    if (temTaca) oam_meta_spr(XTACA, y_taca, TACA, spr_liberta); // Taça
    // Adversários
    if (nivel_comum)
    {
        for (i = 0; i < N_ADVS; i++)
        {
            if (advs[i].ativo)
            {
                xa = (short) pos(advs[i].x);
                ya = (short) pos(advs[i].y);
                if (vabs(xj - xa) < 8 && vabs(yj - ya) < 16) sofreFalta();
                oam_meta_spr(pos(advs[i].x), pos(advs[i].y), ADV + 8 * i, spr_adv());
            }
            else if (banco > 0) novoAdversario(i);
        }
        if (corre)
        {
            corre = false;
            if (nivel < 17 || nivel > 34) correAtras(4);
            if (nivel > 17)
            {
                correAtras(5);
                correAtras(3);
            }
            for (i = 0; i < 3; i++) advs[i].x += real(lado ? 4 : -4);
        }
    }
    else
    {
        ////// AÇÕES DO VILÃO
        oam_meta_spr(pos(v.x), pos(v.y), EXTRA, v.nome == TIGRE ? spr_tigre(lado) : (v.nome == EDISON ? spr_edson(lado) : spr_diabito(lado)));
    }
    // Cartões
    for (i = 0; i < N_CARDS; i++)
    {
    	if (card_ativo(cards[i]))
        {
            dx = COS(card_dir(cards[i])) << 2;
            dy = SEN(card_dir(cards[i])) << 2;
            xc = (short) pos(cards[i].x);
            yc = (short) pos(cards[i].y);
            if (nivel_comum)
            {
                for (j = 0; j < N_ADVS; j++)
                {
                    if (advs[j].ativo)
                    {
                        xa = (short) pos(advs[j].x);
                        ya = (short) pos(advs[j].y);
                        if (vabs(xa - xc) < 6 && yc > ya - 14 && yc < ya + 7)
                            levaCartao(j, i);
                    }	
                }
            }
            else if (v.ativo)
            {
                xa = (short) pos(v.x);
                ya = (short) pos(v.y);
                if (xa >= xc - 4 && xa <= xc + 8 && ya >= yc - 16 && ya <= yc + 14)
                    levaCartaoVilao(i);
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
    // Disparos inimigos
    for (i = 0; i < N_BOLAS; i++)
    {
        if (bolas[i].ativo)
	{
	    dx = COS(bolas[i].dir) << veloc;
	    dy = SEN(bolas[i].dir) << veloc;
            if (vabs(xj - pos(bolas[i].x)) < 8 && pos(bolas[i].y) > yj - 16 && pos(bolas[i].y) < yj + 8)
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
    // Bônus a coletar
    if (bonus & BGOL)
    {
        if (vabs(xj - xb[0]) < 8 && vabs(yj - yb[0]) < 16)
        {
            escalaGoleiro();
            bonus &= ~BGOL;
            dbo[0] = 0; tbo[0] = 0;
        }
        else tbo[0]++;
      	oam_meta_spr(xb[0], yb[0], GOLEIRO, spr_goleiro);
    }
    else dbo[0]++; 
    if (bonus & BARB)
    {
      	if (vabs(xj - xb[1]) < 8 && vabs(yj - yb[1]) < 16)
        {
            compraArbitro();
            bonus &= ~BARB;
            dbo[1] = 0; tbo[1] = 0;
        }
        else tbo[1]++;
      	oam_meta_spr(xb[1], yb[1], JUIZ, spr_arbitro);
    }
    else dbo[1]++;
    if (bonus & BNRG)
    {
      	if (vabs(xj - xb[2]) < 7 && vabs(yj - yb[2]) < 15)
        {
            tomaEnergetico();
            bonus &= ~BNRG;
            dbo[2] = 0; tbo[2] = 0;
        }
        else tbo[2]++;
      	oam_meta_spr(xb[2], yb[2], ENERG, spr_nrg);
    }
    else dbo[2]++;
    if (bonus & BDIN)
    {
        if(xj - xb[3] < 7 && xb[3] - xj < 15 && vabs(yj - yb[3]) < 15)
        {
            ganhaDinheiro();
            bonus &= ~BDIN;
            dbo[3] = 0; tbo[3] = 0;
        }
        else tbo[3]++;
      	oam_meta_spr(xb[3], yb[3], MOEDA, spr_moeda);
    }
    else dbo[3]++;
    // Prepara novo chute
    if (advs[ata].ativo && dec > DELAY_CHUTE) chuta(ata);
    else ata = (ata >= N_ADVS) ? 0 : ata + 1;
}

void atualizaBonus()
{
    if (tbo[0] >= DELAY_BONUS)
    {
        bonus &= ~BGOL;
        tbo[0] = 0;
    }
    else if (gol_disponivel && dbo[0] >= DELAY_BONUS)
    {
      	bonus |= BGOL;
        xb[0] = x_bonus(arena);
        yb[0] = y_bonus(arena);
        dbo[0] = 0; tbo[0] = 0;
    }
    if (tbo[1] >= DELAY_BONUS)
    {
        bonus &= ~BARB;
        tbo[1] = 0;
    }
    else if (arb_disponivel && dbo[1] >= DELAY_BONUS)
    {
        bonus |= BARB;
        xb[1] = x_bonus(arena);
        yb[1] = y_bonus(arena);
        dbo[1] = 0; tbo[1] = 0;
    }
    if (tbo[2] >= DELAY_BONUS)
    {
        bonus &= ~BNRG;
        tbo[2] = 0;
    }
    else if (nrg_disponivel && dbo[2] >= DELAY_BONUS)
    {
        bonus |= BNRG;
        xb[2] = x_bonus(arena);
        yb[2] = y_bonus(arena);
        dbo[2] = 0; tbo[2] = 0;
    }
    if (tbo[3] >= DELAY_BONUS)
    {
      	bonus &= ~BDIN;
        tbo[3] = 0;
    }
    else if (din_disponivel && poup > 0 && dbo[3] >= DELAY_BONUS)
    {
        bonus |= BDIN;
        xb[3] = x_bonus(arena);
        yb[3] = y_bonus(arena);
        dbo[3] = 0; tbo[3] = 0;
    }
}

// Inicializações
void inicializaAgentes()
{
    ata = 0;
    deb = DEBOUNCE;
    dec = DELAY_CHUTE;
    inicializaAdv(advs);
    inicializaBol(bolas);
    inicializaCar(cards);
}

void inicializaVilao()
{
    v.ativo = true;
    v.energia = 100;
    v.x = real(VLX);
    v.y = real(VLY);
    switch (nivel)
    {
      	case 17: v.nome = TIGRE; break;
      	case 34: v.nome = EDISON; break;
      	case 51: v.nome = DIABITO;
    }
}

// Posicionamento inicial dos adversários
void posicionaAdvs()
{
    byte i;
    for (i = 0; i < N_ADVS; i++) novoAdversario(i);
}

void main(void)
{
    bool menu, completo = false, pausa = false;	// Modos de jogo
    byte prox;	// Variáveis de andamento do nível
    byte i = 0, j, k;	// Variáveis para uso em laços
    short dx, dy;	// Variáveis de deslocamento do jogador
    // Configurações iniciais de áudio
    famitone_init(abertura);
    sfx_init(NULL);
    nmi_set_callback(famitone_update);
    while (1)
    {	// Loop infinito
        setup_graphics();
        menu = true;
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
            pal_adv();
            deb = DEBOUNCE;
            dec = DELAY_CHUTE;
            rec = RECUPERACAO;
            for (k = 0; k < 4; k++) tbo[k] = 0;
            // Define parâmetros de nível
            if (nivel_comum)
            {
                ppu_off();
                res = RESERVA;
                banco = 10 + nivel % DIVISOR;
                posicionaJogador();
                arena = carrega_arena(nivel);
                scroll(0, 0);
                inicializaAgentes();
                posicionaAdvs();
              	bonus |= TEM_TACA;   // Taça (fim de nível)
                y_taca = YTACA;
                // ************famitone_init(trilha);
            	// ************music_play(0);
                oam_meta_spr(pos(x), pos(y), CAIM, spr_jogador_parado);
            	ppu_on_all();
            }
            else
            {
              	ppu_off();
                pulo = false;
                bonus &= ~TEM_TACA;
                y_taca = YTACA + 32;
                //*****************famitone_init(trilha_fifa);
                oam_meta_spr(CX, CY, CAIM, spr_jogador_parado);
                //*******************music_play(0);
		switch (nivel)
		{
		   case 17:
			oam_meta_spr(VLX, VLYI, EXTRA, spr_tigre_parado);
                        ppu_on_all();
			conversa_tigre();
			break;
		   case 34: 
			oam_meta_spr(VLX, VLYI, EXTRA, spr_edson_parado);
                        ppu_on_all();
			conversa_edson();
			break;
		   default: 
			oam_meta_spr(VLX, VLYI, EXTRA, spr_diabito_parado);
                        ppu_on_all();
			conversa_devil();
		}
                espera(120);
                ppu_off();
                posicionaJogador();
                inicializaVilao();
                arena = carrega_arena(nivel);
                scroll(0, 0);
                ppu_on_all();
                
            }
            while (prox == false && energia > 0)
            {	// Loop do nível antes de passar ou perder vida
                pad = pad_poll(0);
                if (pausa && pad & PAD_START)
                {   // Sai da pausa
                    pausa = false;
                    ppu_off();
                    escrita_centralizada("       ", 2);
                    ppu_on_all();
                    espera(15);
                    //**********music_pause(0);
                } 
                else if (!pausa)
                {   // Regime normal do jogo
                    // Comando de animação
                    i++;
                    if (i > 4)
                    {
                        i = 0;
                    	lado = !lado;
                        corre = true;
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
                        //*********music_pause(*trilha);
                        ppu_on_all();
                    }
                    if (pegou_taca())
                    {
                        ppu_off();
                        music_stop();
                        oam_clear();
                        escrita_centralizada(" PARABENS!", 8);
                        escrita_centralizada(" VOCE PASSOU DE FASE!", 10);
                        ppu_on_all();
                        prox = true;
                        espera(300);
                    }
                    // *** Rotinas aleatórias
                    // **************** , movimentação do inimigo (em campo próprio)
                    // Atualização do quadro
                    else
                    {
                        oam_clear();
                        atualizaPlacar();
                        atualizaSprites();
                        atualizaBonus();
                        if (deb <= DEBOUNCE) deb++;
                        if (dec <= DELAY_CHUTE) dec++;
                        if (res <= RESERVA) res++;
                        if (rec <= RECUPERACAO) rec++;
                    }
                    ppu_wait_nmi();
                    if (pausa) espera(100);
                }
            }
            music_stop();
            if (energia <= 0) // Morreu
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
