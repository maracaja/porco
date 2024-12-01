#include <stdlib.h>
#include <string.h>
#include <nes.h>
#include "neslib.h"
#include "vrambuf.h"	// VRAM update buffer
//#link "vrambuf.c"
//#link "famitone2.s"
#include "constantes.h" // Constantes usadas
#include "funcoes.h"	// Biblioteca de funções
//#link "funcoes.c"
#include "objetos.h"    // Definição dos atores do jogo e mais
//#link "objetos.c"

// Importação dos recursos gráficos
//#resource "chr_porco.chr"
//#link "tileset.s"
//#resource "titulo.h"
//#resource "niveis.h"
#include "tela_fim.h"
#include "sprites.h"
//#link "sprites.c"

// Importação dos recursos de áudio
//#link "abertura.s"
//#link "trilha.s"
//#link "trilha_viloes.s"
//#link "sons.s"
extern char abertura[];
extern char trilha[];
extern char trilha_fifa[];
extern char sons[];

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
const int const senos[32] = {0,49,97,142,181,212,236,251,256,251,236,212,181,142,97,49,0,-50,-98,-143,-182,-213,-237,-252,-256,-252,-237,-213,-182,-143,-98,-50};

// Paleta padrão
/*{pal:"nes",layout:"nes"}*/
const byte PALETTE[16] = { 0x01,0x0F,0x30,0x16,0x01,0x19,0x36,0x30,0x01,0x04,0x36,0x07,0x01,0x27,0x10,0x38 };
const byte PAL_EXTRA[20] = { 0x01,0x16,0x36,0x30,0x01,0x30,0x17,0x0f,0x01,0x38,0x27,0x21,0x01,0x30,0x06,0x0f,0x01,0x30,0x36,0x16 };

// Tabela de níveis do modo demonstração
const byte const demo[7] = /*{51, 99}; */{0, 11, 27, 34, 49, 51, 99};

// Objetos
byte arena;
Adversario advs[N_ADVS];
Bola bolas[N_BOLAS];
Cartao cards[N_CARDS];
Vilao v;

// Dados do jogo
Estado estado;	// Máquina de estados da aplicação
EstadoJogo est_jogo;	// Máquina de estados de jogo
bool completo = false;	// Modo de jogo
char pad;  // Leitura do controle
byte nivel;
byte deb;  // Delay (em quadros) a cada tiro
byte dec;  // Delay (em quadros) a cada chute
byte rec;  // Delay (em quadros) entre dois choques do jogador contra oponentes
byte ata;   // Jogador que vai chutar (estava sendo o mesmo)
byte banco; // Número máximo de adversários por nível 
byte res;   // Tempo (em quadros) para aparecer o reserva
byte poup;  // Quantidade de moedas que podem aparecer
word tbo[4];  // Conta o tempo que o bônus aparece na tela
word dbo[4];  // Tempo sem bônus na tela
bool lado = false;  // Flags de animação
bool corre = false;
byte i = 0, j, k;   // Variáveis para uso em laços
bool pulo;    // Variável de controle para escapar de introduções

// Dados do jogador
word x, y;
byte dinheiro;
sbyte energia;
byte luvas;
byte vidas;
byte move;

// Dados de presença dos bônus
byte bonus;
byte y_taca;
// Posições onde se localizarão (ordem de definição das respectivas constantes)
byte xb[4] = {91, 157, 128, 128};	
byte yb[4] = {150, 150, 150, 150};

// setup PPU and tables
void setup_graphics() 
{  
    oam_clear();   // clear sprites
    pal_bg(PALETTE);	// set palette colors
    pal_spr(PALETTE);
}

// Definições iniciais
void setup()
{
    famitone_init(&trilha);
    sfx_init(&sons);
    nmi_set_callback(famitone_update);
    estado = INICIO;    
}

// FUNÇÕES DE INTRODUÇÃO
// Escreve uma mensagem de linha única aparecendo um caractere por vez
void escreve_mensagem(const char* msg, unsigned char lin, unsigned char col)
{
    byte a = col, b = lin, tam = MIN(strlen(msg), 28), m = 0, n = 0;
    char pad;
    while (!pulo && m < tam) 
    {
        if (n >= 4)
        {
            vrambuf_put(NTADR_A(a + m, b), &msg[m], 1);
            set_vram_update(updbuf);
            m++; n = 0;
            vrambuf_flush();
        }
        else
        {
            ppu_wait_nmi();
            n++;
        }
        // Sai do diálogo se o jogador pressionar A
        pad = pad_poll(0);
        if (pad & BOTAO_PULO) pulo = true;
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
    if (!pulo)
    {
        espera(60);
        limpa_tela(NAMETABLE_A);
        troca_spr_intro();
    }
}

void conversa()
{
    byte z;
    for (z = 0; z < 7; z++)
        escreve_mensagem(trechos_titia[z], 2 * z + 2, COL_INTRO);
    if (!pulo) oam_meta_spr(TTX, SPRY, TACA, spr_liberta);
    for (; z < 10; z++)
        escreve_mensagem(trechos_titia[z], 2 * z + 2, COL_INTRO);
    if (!pulo) transicao_intro();
    if (!pulo) oam_meta_spr(SPRX, SPRY, GOLEIRO, spr_goleiro);
    for (; z < 14; z++)
        escreve_mensagem(trechos_titia[z], 2 * z - 18, COL_INTRO);
    if (!pulo)
    {
        troca_spr_intro();
        oam_meta_spr(SPRX, SPRY, JUIZ, spr_arbitro);
    }
    for (; z < 18; i++)
        escreve_mensagem(trechos_titia[z], 2 * z - 18, COL_INTRO);
    if (!pulo) 
    {
      	troca_spr_intro();
        oam_meta_spr(SPRX, SPRY, ENERG, spr_nrg);
    }
    for (; z < 22; z++)
        escreve_mensagem(trechos_titia[z], 2 * z - 18, COL_INTRO);
    if (!pulo) 
    {
    	transicao_intro();
    	oam_meta_spr(TTX, SPRY, MOEDA, spr_moeda);
    }
    for (; z < 31; z++)
        escreve_mensagem(trechos_titia[z], 2 * z - 42, COL_INTRO);
    if (!pulo) espera(150);
}

// História inicial do jogo
void historinha()
{
    byte z;
    byte caim_x = 100, caim_y = 210; // Coordenadas do personagem principal
    char pad;
    limpa_tela(NAMETABLE_C);
    limpa_tela(NAMETABLE_A);
    scroll(0,0);
    ppu_off();
    escrita_centralizada("BROOKLIN", 13);
    escrita_centralizada("1984", 15);
    ppu_on_all();
    espera(30);
    for (z = 0; z < 150 && !pulo; z++)
    {
        pad = pad_poll(0);
        if (pad & BOTAO_PULO) pulo = true;
        ppu_wait_nmi();
    }
    limpa_tela(NAMETABLE_A);
    ppu_off();
    oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador_parado);
    desenha_tia();
    ppu_on_all();
    while (caim_x > TTX - 4 && !pulo)
    {      
        caim_x--; caim_y -= 2;
        oam_clear();
        oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador(caim_x % 2));
        desenha_tia();
        ppu_wait_nmi();
        // Pula introdução
        pad = pad_poll(0);
        if (pad & BOTAO_PULO) pulo = true;
        espera(4);
    }
    oam_meta_spr(caim_x, caim_y, CAIM, spr_jogador_parado);
    desenha_tia();
    if (!pulo) conversa();
}

// INÍCIO DOS NÍVEIS ESPECIAIS
void conversa_tigre()
{
    byte z;
    pulo = false;
    for (z = 0; z < 3; z++) escreve_mensagem(trechos_tigre[i], 2 * z + 6, 2);
}

void conversa_edson()
{
    byte z;
    pulo = false;
    for (z = 0; z < 5; z++) escreve_mensagem(trechos_edson[z], 2 * z + 6, 2);
}

void conversa_devil()
{
    byte z;
    oam_meta_spr(CX, CY + 10, CAIM, spr_jogador_parado);
    pulo = false;
    for (z = 0; z < 10; z++) escreve_mensagem(trechos_devil[z], 2 * z + 6, 2);
}

// Cria um novo adversário na tela
void novo_adversario(byte a)
{
    if (res >= RESERVA) 
    {
        byte resto = a % 3;
        bool linha = a < 3;
        advs[a].ativo = true;
        advs[a].energia = 100;
        advs[a].x = linha ? real(98 + 24 * resto) : real(64 + 60 * resto);
        advs[a].y = real(YMIN + (linha ? 40 : 70 + 15 * arena));
        banco--;
    }
}

// Inicializações
void posiciona_jogador()
{
    x = real(CX);
    y = real(CY);
}

void inicializa_jogador()
{
    posiciona_jogador();
    dinheiro = 0;
    energia = 99;
    luvas = 0;
    vidas = 3;
    bonus = BGOL | BARB | BDIN;
    poup = 0;
}

void posiciona_vilao()
{
    v.x = real(VLX);
    v.y = real(VLY);
}

void inicializa_vilao()
{
    v.ativo = true;
    v.energia = 100;
    posiciona_vilao();
    switch (nivel)
    {
      	case 17: v.nome = TIGRE; break;
      	case 34: v.nome = EDISON; break;
      	case 51: v.nome = DIABITO;
    }
}

void inicializa_agentes()
{
    ata = 0;
    deb = DEBOUNCE;
    dec = DELAY_CHUTE;
    if (nivel_comum) inicializa_adv(advs);
    else inicializa_vilao();
    inicializa_bol(bolas);
    inicializa_car(cards);
}

// Posicionamento inicial dos adversários
void posiciona_advs()
{
    byte a;
    for (a = 0; a < N_ADVS; a++) novo_adversario(a);
}

// Funções referentes ao jogador
void leva_bolada()
{
    if (luvas > 0) luvas--;
    else energia = MAX(0, energia - 19);
    if (gol_disponivel) dbo[0] = 0;
    sfx_play(SFX_BOLADA, 0);
}

void sofre_falta()
{
    if (rec > RECUPERACAO)
    {
        switch (nivel)
        {
            case 34: energia = 0; break;
            default:
            	if (bonus & TEM_CARTAO)
                {
                    if (cVermelho) bonus &= ~CARD_VERM;
                    else bonus &= ~TEM_CARTAO;
                }
                else energia = MAX(0, energia - 50);
                rec = 0;
        }
	posiciona_jogador();
        dbo[1] = 0; dbo[2] = 0;
        sfx_play(SFX_FALTA, 0);
    }
}

void toma_energetico()
{
    energia = MIN(energia + 51, 99);
    bonus &= ~BNRG;
    sfx_play(SFX_BONUS, 0);
}

void escala_goleiro()
{
    luvas = nivel < 51 ? 5 : 2;
    bonus &= ~BGOL;
    sfx_play(SFX_BONUS, 0);
}

void compra_arbitro()
{
    if (temCartao) bonus |= CARD_VERM;
    else bonus |= TEM_CARTAO & ~CARD_VERM;
    bonus &= ~BARB;
    sfx_play(SFX_JUIZ, 0);
}

void ganha_dinheiro()
{
    dinheiro++;
    if (dinheiro > 99)
    {
        vidas++;
        dinheiro = 0;
        sfx_play(SFX_VIDA, 0);
    }
    else sfx_play(SFX_BONUS, 0);
    bonus &= ~BDIN;    
}

byte ativa_cartao()
{
    byte c;
    for (c = 0; c < N_CARDS; c++)
    {
        if (!card_ativo(cards[c])) 
        {
            cards[c].info = CARD_ATIVO | (cVermelho ? CARD_VERM : 0);
            return c;
        }
    }
    return 0;
}

void desativa_cartao(byte c)
{ cards[c].info &= ~CARD_ATIVO; }

void atira()
{
    byte c = ativa_cartao();
    if (jogador_mov && jogador_dir == 8)
    {
        cards[c].y = y;
        if (pos(x) > 126)
        {
            cards[c].x = x - real(10);
            cards[c].info |= 0x10;
        }
        else cards[c].x = x + real(10);
    }
    else
    {
        cards[c].x = x;
        cards[c].y = y - real(16);
        cards[c].info |= (!jogador_mov || jogador_dir > 16) ? 24 : (jogador_dir < 8 ? 28 : 20);
    }
    sfx_play(SFX_ATIRA, 0);
    deb = 0;
}

bool pegou_taca()
{
    byte xj = pos(x), yj = pos(y);
    return xj >= XTACA - 9 && xj <= XTACA + 5 && yj >= y_taca - 21 && yj <= y_taca + 13 && temTaca;
}

// Controle do deslocamento do jogador na tela
void controla_jogador()
{
    move = movimento(pad);
    if (jogador_mov)
    {
        int dx = COS(jogador_dir);
        int dy = SEN(jogador_dir);
        if (nao_bate_parede(arena, x + dx, y, true)) x += dx;
        if (nao_bate_parede(arena, x, y + dy, true)) y += dy;
    }
    // Atira cartão (se tiver esse poder)
    if (pad & PAD_A && temCartao && deb > DEBOUNCE) atira();
    if (pad & PAD_START)
    {
      ppu_off();
      escrita_centralizada("PAUSADO", 2);
      if (nivel_comum) music_pause(*trilha_fifa);
      //nivel_comum ? *trilha : */ // FALTA TRILHA
      ppu_on_all();
      est_jogo = PAUSA;
    }
}

// Funções referentes às ações dos rivais
byte ativa_bola()
{
    byte b;
    for (b = 0; b < N_BOLAS; b++)
    {
        if (!bolas[b].ativo)
        {
            bolas[b].ativo = true;
            return b;
        }
    }
    return 0;
}

void chuta(byte a)
{
    if (pode_chutar(nivel))
    {
        byte b = ativa_bola();
        bolas[b].x = advs[a].x;
        bolas[b].y = advs[a].y + real(8);
        bolas[b].dir = direcao2(x, bolas[b].x, y, bolas[b].y);
        sfx_play(SFX_CHUTE, 0);
    }
}

void vilao_chuta()
{
    byte b = ativa_bola();
    bolas[b].x = v.x - real(4);
    bolas[b].y = v.y + real(16);
    bolas[b].dir = direcao2(x, bolas[b].x, y, bolas[b].y);
    sfx_play(SFX_CHUTE, 0);
}

// Efeitos do tiro sobre o adversário
void leva_cartao(byte a, byte c)
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
    desativa_cartao(c);
    sfx_play(SFX_ACERTO, 0);
}

void leva_cartao_vilao(byte c)
{
    v.energia -= vermelho(cards[c]) ? 2 : 1;
    if (v.energia <= 0)
    {
        v.ativo = false;
        bonus |= TEM_TACA;
    }
    desativa_cartao(c);
    sfx_play(SFX_ACERTO, 0);
}

// Alternância de paleta conforme o nível
void troca_paleta(byte cor)
{
    byte p;
    for (p = 0; p < 4; p++) pal_col(24 + p, PAL_EXTRA[(cor << 2) | p]);
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

// Função para adversário correr atrás do jogador
void corre_atras(byte a)
{
    if (advs[a].ativo)
    {
        byte da = direcao2(x, advs[a].x, y, advs[a].y);
        int dx = COS(da) << 1, dy = SEN(da) << 1; 
        if (nao_bate_parede(arena, advs[a].x + dx, advs[a].y, true)) advs[a].x += dx;
        if (nao_bate_parede(arena, advs[a].x, advs[a].y + dy, true)) advs[a].y += dy;
    }
}

// Atualização do placar a cada quadro
void atualiza_placar()
{
    placar(vidas, PLV, 1);
    placar(dinheiro, PLD, 2);
    placar(energia, PLE, 2);
    placar(luvas, PLL, 1);
    if (temCartao) cor_cartao(cVermelho);
}

// Atualização dos sprites a cada quadro
void atualiza_sprites()
{
    byte a, b, m, n, da;
    int dx, dy, xa, ya, xc, yc, xj = pos(x), yj = pos(y);
    oam_meta_spr(xj, yj, CAIM, pad & 0xF0 ? spr_jogador(lado) : spr_jogador_parado); // Jogador
    if (temTaca) oam_meta_spr(XTACA, y_taca, TACA, spr_liberta); // Taça
    // Adversários
    if (nivel_comum)
    {
        for (a = 0; a < N_ADVS; a++)
        {
            if (advs[a].ativo)
            {
                xa = (int) pos(advs[a].x);
                ya = (int) pos(advs[a].y);
                if (vabs(xj - xa) < 8 && vabs(yj - ya) < 16) sofre_falta();
                oam_meta_spr(pos(advs[a].x), pos(advs[a].y), ADV + 8 * a, spr_adv());
            }
            else if (banco > 0) novo_adversario(a);
        }
        if (corre)
        {
            corre = false;
            if (nivel < 17 || nivel > 34) corre_atras(4);
            if (nivel > 17)
            {
                corre_atras(5);
                corre_atras(3);
            }
            for (a = 0; a < 3; a++) advs[a].x += real(lado ? 5 : -5);
        }
    }
    else if (v.ativo)
    {
        xa = (int) pos(v.x);
        ya = (int) pos(v.y);
        if (xa > xj - 4 && xa < xj + 12 && ya > yj - 24 && ya < yj + 16) 
        {
            sofre_falta();
            if (nivel == 51) posiciona_vilao();
        }
        switch (nivel)
        {
            case 17:
            	if (corre)
                {
                    da = rand() & 0x1F;                
                    dx = COS(da) << 2;
                    dy = SEN(da) << 2;
                    m = pos(v.x + dx);
                    n = pos(v.y + dy);
                    if (m >= 58 && m <= 198) v.x += dx;
                    if (n >= 50 && n <= 100) v.y += dy;
                }
                break;
            case 34:
                v.x = real(MAX(50, MIN(200, 255 - xj)));
                v.y = (xa == 50 || xa == 200) ? real(MAX(50, ya - 2)) : real(MIN(120, ya + 2));
                break;
            default:
                if (ya > 144 && (xj < 80 || xj > 176)) v.y = real(144);
                else if (xj < 80)
                {
                    v.x = real(60);
                    v.y = real(MAX(50, ya - 1));
                }
                else if (xj > 176)
                {
                    v.x = real(196);
                    v.y = real(MAX(50, ya - 1));
                }
                else if (xj >= 100 && xj <= 156)
                {
                    da = direcao2(x, v.x, y, v.y);
                    v.x += COS(da) << 1;
                    v.y += SEN(da) << 1; 
                }
                else v.x += real(lado ? 7 : -7);
        }
        oam_meta_spr(pos(v.x), pos(v.y), EXTRA, v.nome == TIGRE ? spr_tigre(lado) : (v.nome == EDISON ? spr_edson(lado) : spr_diabito(lado)));
    }
    // Cartões
    for (a = 0; a < N_CARDS; a++)
    {
    	if (card_ativo(cards[a]))
        {
            dx = COS(card_dir(cards[a])) << 2;
            dy = SEN(card_dir(cards[a])) << 2;
            xc = (int) pos(cards[a].x);
            yc = (int) pos(cards[a].y);
            if (nivel_comum)
            {
                for (b = 0; b < N_ADVS; b++)
                {
                    if (advs[b].ativo)
                    {
                        xa = (int) pos(advs[b].x);
                        ya = (int) pos(advs[b].y);
                        if (vabs(xa - xc) < 6 && yc > ya - 14 && yc < ya + 7)
                            leva_cartao(b, a);
                    }	
                }
            }
            else if (v.ativo)
            {
                if (xa > xc - 4 && xa < xc + 8 && ya > yc - 16 && ya < yc + 14)
                    leva_cartao_vilao(a);
            }
            if (nao_bate_parede(arena, cards[a].x + dx, cards[a].y + dy, false))
            {
                cards[a].x += dx;
                cards[a].y += dy;
            }
            else desativa_cartao(a);
            oam_spr(pos(cards[a].x), pos(cards[a].y), CARD, vermelho(cards[a]) ? 0 : 3, CARTAO + 4 * a);
        }
    }
    // Disparos inimigos
    for (a = 0; a < N_BOLAS; a++)
    {
        if (bolas[a].ativo)
	{
	    dx = COS(bolas[a].dir) << veloc;
	    dy = SEN(bolas[a].dir) << veloc;
            if (vabs(xj - pos(bolas[a].x)) < 8 && pos(bolas[a].y) > yj - 16 && pos(bolas[a].y) < yj + 8)
            {
                leva_bolada();
                bolas[a].ativo = false;
            }
	    else if (nao_bate_parede(arena, bolas[a].x + dx, bolas[a].y + dy, false))
	    {
		bolas[a].x += dx;
                bolas[a].y += dy;
	    }
	    else bolas[a].ativo = false;
	    oam_spr(pos(bolas[a].x), pos(bolas[a].y), BOLA, 0, TIRO + 4 * a);
	}
    }    
    // Bônus a coletar
    if (bonus & BGOL)
    {
        if (vabs(xj - xb[0]) < 8 && vabs(yj - yb[0]) < 16)
        {
            escala_goleiro();
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
            compra_arbitro();
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
            toma_energetico();
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
            ganha_dinheiro();
            bonus &= ~BDIN;
            dbo[3] = 0; tbo[3] = 0;
        }
        else tbo[3]++;
      	oam_meta_spr(xb[3], yb[3], MOEDA, spr_moeda);
    }
    else dbo[3]++;
    // Prepara novo chute
    if (dec > DELAY_CHUTE)
    {
        if (nivel_comum)
        {
            if (advs[ata].ativo) chuta(ata);
            else ata = (ata >= N_ADVS) ? 0 : ata + 1;
        }
        else if (v.ativo && !(rand() & (nivel == 34 ? 0x01 : 0x03)))
            vilao_chuta();
        dec = 0;
    }
}

// Controle do surgimento e sumiço de bônus na arena de jogo
void atualiza_bonus()
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

// ESTADOS DE INÍCIO DO JOGO
// Início da aplicação
void inicio()
{
    setup_graphics();
    sfx_play(SFX_INTRO, 0);
    apresentacao();
    selecao(completo);    
    estado = MENU;
}

// Introdução do jogo
void introducao()
{
    setup_graphics();
    historinha();
    limpa_tela(NAMETABLE_A);
    nivel = completo ? 0 : demo[j = 0];
    inicializa_jogador();
    estado = JOGO;
    est_jogo = ENTRADA;
}

// Controle do menu inicial
void menu()
{
    pad = pad_poll(0);
    if (pad & PAD_DOWN && !completo) selecao(completo = true);
    if (pad & PAD_UP && completo) selecao(completo = false);
    if (pad & PAD_A)
    {
        srand(nesclock());
        estado = INTRO;
    }
}

// Espera START para voltar ao início da aplicação
void retorna()
{
    pad = pad_poll(0);
    while (!(pad & PAD_START))
    {
      pad = pad_poll(0);
      ppu_wait_nmi();
    }
    limpa_tela(NAMETABLE_A);
    limpa_tela(NAMETABLE_C);
    estado = INICIO;
}

// ESTADOS DE FIM DE JOGO
void fim_vitoria()
{
    ppu_off();
    setup_graphics();
    vram_adr(NAMETABLE_A);
    vram_unrle(completo ? tela_fim_completo : tela_fim_demo);
    ppu_on_all();
    retorna();
}

void fim_derrota()
{
    game_over();
    sfx_play(nivel & 0x01 ? SFX_FIM1 : SFX_FIM0, 0);
    retorna();
}

// FUNÇÕES DE OPERAÇÃO DO JOGO
// Início de nível
void inicio_nivel()
{
    entrada_nivel(nivel);
    pal_adv();
    deb = DEBOUNCE;
    dec = DELAY_CHUTE;
    rec = RECUPERACAO;
    for (k = 0; k < 4; k++) tbo[k] = 0;
    inicializa_agentes();
    // Vilões se apresentam
    if (!nivel_comum)
    {
        oam_meta_spr(CX, CY, CAIM, spr_jogador_parado);                
        switch (nivel)
        {
          case 17:
              oam_meta_spr(VLX, VLYI, EXTRA, spr_tigre_parado);
              conversa_tigre();
              break;
          case 34: 
              oam_meta_spr(VLX, VLYI, EXTRA, spr_edson_parado);
              conversa_edson();
              break;
          default: 
              oam_meta_spr(VLX, VLYI, EXTRA, spr_diabito_parado);
                conversa_devil();
        }
        if (!pulo) espera(120);
        bonus &= ~TEM_TACA;
        y_taca = YTACA + 16;
        //famitone_init(trilha_fifa);
        //music_play(0);
    }
    else // Configurações dos níveis normais
    {
        res = RESERVA;
        banco = 11 + nivel % DIVISOR;                                                                        
        posiciona_advs();
        bonus |= TEM_TACA;   // Taça (fim de nível)
        y_taca = YTACA;
        //famitone_init(trilha_fifa);	// TEMPORÁRIO
        //music_play(0);
        oam_meta_spr(pos(x), pos(y), CAIM, spr_jogador_parado);
    }
    est_jogo = LOOP;
    ppu_off();
    posiciona_jogador();
    arena = carrega_arena(nivel);
    scroll(0, 0);
    ppu_on_all(); 
}

// Estado de pausa
void pausa()
{
    pad = pad_poll(0);
    if (pad & PAD_START)
    {
        ppu_off();
        escrita_centralizada("       ", 2);
        ppu_on_all();
        espera(15);
        //music_pause(0);
        est_jogo = LOOP;
    }
    else espera(100);
}

// Ações em caso de perder vida
void perdeu_vida()
{
    //music_stop();
    vidas--;
    energia = 99;
    sfx_play(SFX_MORREU, 0);
    espera(90);
    if (vidas <= 0) estado = DERROTA;
    else est_jogo = ENTRADA;
}

// Avanço de nível
void avanca_nivel()
{
    //music_stop();
    nivel = (completo ? nivel + 1 : demo[++j]);
    if (nivel > NIVEIS) estado = VITORIA;
    else est_jogo = ENTRADA;
}

// Loop principal do jogo
void loop_jogo()
{
    pad = pad_poll(0);
    // Comando de animação
    i++;
    if (i > 4)
    {
        i = 0;
        lado = !lado;
        corre = true;
    }
    controla_jogador();    	
    if (pegou_taca())
    {	// Chegou ao objetivo
        if (nivel_comum)
        {
            ppu_off();
            //music_stop();
            oam_clear();
            escrita_centralizada(" PARABENS!", 8);
            escrita_centralizada(" VOCE PASSOU DE FASE!", 10);
            ppu_on_all();
        }
        sfx_play(SFX_PASSOU, 0);
        espera(400);
        est_jogo = AVANCA;
    }
    else if (energia <= 0) est_jogo = PERDE; // Acabou energia
    // Atualização do quadro
    oam_clear();
    atualiza_placar();
    atualiza_sprites();
    atualiza_bonus();
    if (deb <= DEBOUNCE) deb++;
    if (dec <= DELAY_CHUTE) dec++;
    if (res <= RESERVA) res++;
    if (rec <= RECUPERACAO) rec++;
}

// Máquina de estados em ação durante o jogo
void jogo()
{
    switch (est_jogo)
    {
        case ENTRADA: inicio_nivel(); break;        
        case LOOP: loop_jogo(); break;	// Jogando pra valer
        case PAUSA: pausa(); break;	// Pausa        
        case PERDE: perdeu_vida(); break;  // Perde vida      
        case AVANCA: avanca_nivel();	// Avança nível            
        default: break;
    }
    ppu_wait_nmi();
}

void main(void)
{
    setup();
    while (1)
    {	// Loop infinito
        switch (estado)
        {   
            case INICIO: inicio(); break;
            case MENU: menu(); break; // Menu            
            case INTRO: introducao(); break; 	// Início da história         
            case JOGO: jogo(); break;  // Regime normal de jogo          
            case VITORIA: fim_vitoria(); break;	// Vitória            
            case DERROTA: fim_derrota(); 	// Derrota            	
            default: break;
        }        
    }
}
