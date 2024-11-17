#ifndef CONSTANTES_H
#define CONSTANTES_H

#define NIVEIS 51
#define DIVISOR 17  // 51 níveis -> Divide o jogo em 3 grandes blocos
#define DEBOUNCE 60
#define ZERO 0x30	// Sprite '0'
#define CARD 0x01	// Sprite cartão
#define BOLA 0x0A	// Sprite bola
#define LUVA 0x62	// Sprite luva
// Sprites adversário parado
#define JADV1 0x1A
#define JADV2 0x1D
// Endereços OAM
#define CAIM 0x00
#define ENERG 0x08
#define JUIZ 0x10
#define GOLEIRO 0x18
#define ADV 0x20
#define PL_VIDA 0x4C
#define PL_NRG 0x50
#define PL_DIN 0x58
#define PL_LUVA 0x60
#define PL_CARD 0x74
#define CARTAO 0x78
#define TIRO 0x90
#define MOEDA 0xC0
#define TACA 0xD0
#define EXTRA 0xE8
// Posição da apresentação do jogo
#define TTX 24
#define TTY 32
#define SPRX TTX - 4
#define SPRY TTY + 60
// Placar
#define PL_LIN 7
#define PLV 6
#define PLD 10
#define PLE 20
#define PLL 23
#define PLC 240
// Quantidades máximas simultâneas de sprites
#define N_ADVS  11
#define N_BOLAS 12
#define N_CARDS 6
// Posições iniciais do jogador
#define CX 124
#define CY 200
// Limites máximos de deslocamento na tela
#define XMIN 15
#define XMAX 233 
#define YMIN 30
#define YMAX 224
// Posições da taça
#define XTACA 128
#define YTACA 44
// Identificação dos bônus
#define BGOL 0x01
#define BARB 0x02
#define BNRG 0x04
#define BDIN 0x08
#define TEM_CARTAO 0x10
#define CARD_VERM 0x20
#define CARD_ATIVO 0x40
#define TEM_TACA 0x40

#endif