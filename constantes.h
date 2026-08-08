#ifndef CONSTANTES_H
#define CONSTANTES_H

#define NIVEIS 51
#define DIVISOR 17
#define DEBOUNCE 15
#define DELAY_CHUTE 15
#define DELAY_BONUS 600
#define RECUPERACAO 60
#define RESERVA 130
// Sprites
#define ZERO 0x30
#define CARD 0x01
#define BOLA 0x0A
#define LUVA 0x62
// Endereços OAM
#define CAIM 0x00
#define ENERG 0x08
#define JUIZ 0x10
#define GOLEIRO 0x18
#define ADV 0x20
#define PL_VIDA 0x50
#define PL_NRG 0x54
#define PL_DIN 0x5C
#define PL_LUVA 0x64
#define PL_CARD 0x70
#define CARTAO 0x74
#define TIRO 0x90
#define MOEDA 0xC0
#define TACA 0xD0
#define EXTRA 0xE8
// Posição das apresentações no jogo
#define TTX 24
#define TTY 32
#define VLX 128
#define VLYI 24
#define VLY 50
#define SPRX TTX - 4
#define SPRY TTY + 60
// Placar
#define PL_LIN 7
#define PLV 6
#define PLD 10
#define PLE 20
#define PLL 27
#define PLC 240
// Quantidades máximas simultâneas de sprites
#define N_ADVS  6
#define N_BOLAS 12
#define N_CARDS 7
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
// Localizações dos efeitos de áudio
#define SFX_CHUTE 0
#define SFX_ATIRA 1
#define SFX_JUIZ 2
#define SFX_FALTA 3
#define SFX_ACERTO 4
#define SFX_BONUS 5
#define SFX_BOLADA 6
#define SFX_MORREU 7
#define SFX_VIDA 8
#define SFX_PASSOU 9
#define SFX_FIM0 10
#define SFX_FIM1 11
#define SFX_INTRO 12
// Utilidades para introdução
#define ESPACO " "
#define COL_INTRO 5
#define BOTAO_PULO PAD_A
// Identificação das trilhas sonoras
#define TRILHA_HINO 2
#define TRILHA_COMUM 1
#define TRILHA_VILAO 0

#endif
