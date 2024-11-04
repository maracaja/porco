#include "neslib.h"
#include "sprites.h"

unsigned char spr_moeda[] = {
    -8,-8,0x02,3,
    0,0,0x02,3 | OAM_FLIP_H | OAM_FLIP_V,
    -8,0,0x03,3,
    0,-8,0x03,3 | OAM_FLIP_H | OAM_FLIP_V,
    128
};

unsigned char spr_titia[] = {
    -8,-16,0x0c,2,
    -8,-8,0x0d,2,
    -8,0,0x0e,2,
    0, 0,0x0e,2|OAM_FLIP_H,
    0,-8,0x0d,2|OAM_FLIP_H,
    0,-16,0x0c,2|OAM_FLIP_H,
    128
};

unsigned char spr_liberta[] = {
    -8,-16,0x04,3,
    0,-16,0x06,3,
    -8, 0,0x08,3,
    0, 0,0x09,3,
    -8,-8,0x05,3,
    0,-8,0x07,3,
    128
};

unsigned char spr_nrg[] = {
      0,-8,0x10,0,
      0, 0,0x11,0,
    128
};

unsigned char spr_jogador_parado[] = {
    0,-8,0x12,1,
    0, 0,0x13,1,
    128
};

unsigned char spr_jogador_correndo1[] = {
    0,-8,0x14,1,
    0, 0,0x15,1,
    128
};

unsigned char spr_jogador_correndo2[] = {
    0,-8,0x14,1 | OAM_FLIP_H,
    0, 0,0x15,1 | OAM_FLIP_H,
    128
};

unsigned char spr_goleiro[] = {
    0,-8,0x16,3,
    0, 0,0x17,3,
    128
};

unsigned char spr_arbitro[] = {
    0,-8,0x18,0,
    0, 0,0x19,0,
    128
};

unsigned char spr_edson_parado[] = {
   -8,-8,0x22,2,
	 0,-8,0x22,2 | OAM_FLIP_H,
	 0,  0,0x23,2 | OAM_FLIP_H,
   -8,  0,0x23,2,
   -8,  8,0x24,2,
	 0,  8,0x24,2 | OAM_FLIP_H,
   128
};

unsigned char spr_edson_correndo1[] = {
   -8,-8,0x22,2,
	 0,-8,0x22,2 | OAM_FLIP_H,
   -8,  0,0x27,2,
   -8,  8,0x26,2,
	 0,  0,0x25,2 | OAM_FLIP_H,
	 0,  8,0x24,2 | OAM_FLIP_H,
   128
};

unsigned char spr_edson_correndo2[] = {
	 0,-8,0x22,2 | OAM_FLIP_H,
   -8,-8,0x22,2,
	 0,  0,0x27,2 | OAM_FLIP_H,
	 0,  8,0x26,2 | OAM_FLIP_H,
   -8,  0,0x25,2,
   -8,  8,0x24,2,
   128
};

unsigned char spr_diabito_parado[] = {
   -8,-8,0x66,2,
	 0,-8,0x66,2 | OAM_FLIP_H,
   -8,  0,0x67,2,
	 0,  0,0x67,2 | OAM_FLIP_H,
	 0,  8,0x68,2 | OAM_FLIP_H,
   -8,  8,0x68,2,
   128
};

unsigned char spr_diabito_correndo1[] = {
   -8,-8,0x66,2,
	 0,-8,0x66,2 | OAM_FLIP_H,
   -8,  0,0x69,2,
	 0,  0,0x67,2 | OAM_FLIP_H,
	 0,  8,0x6a,2 | OAM_FLIP_H,
   -8,  8,0x68,2,
   128
};

unsigned char spr_diabito_correndo2[] = {
	 0,-8,0x66,2 | OAM_FLIP_H,
   -8,-8,0x66,2,
	 0,  0,0x69,2 | OAM_FLIP_H,
   -8,  0,0x67,2,
   -8,  8,0x6a,2,
	 0,  8,0x68,2 | OAM_FLIP_H,
   128
};

unsigned char spr_tigre_parado[] = {
   -8,-8,0x5b,2,
	 0,-8,0x5b,2 | OAM_FLIP_H,
   -8,  0,0x5c,2,
	 0,  0,0x5c,2 | OAM_FLIP_H,
	 0,  8,0x5d,2 | OAM_FLIP_H,
   -8,  8,0x5d,2,
   128
};

unsigned char spr_tigre_correndo1[] = {
   -8,-8,0x5b,2,
	 0,-8,0x5b,2 | OAM_FLIP_H,
   -8,  0,0x5c,2,
	 0,  0,0x5f,2 | OAM_FLIP_H,
	 0,  8,0x5d,2 | OAM_FLIP_H,
   -8,  8,0x5e,2,
   128
};

unsigned char spr_tigre_correndo2[] = {
	 0,-8,0x5b,2 | OAM_FLIP_H,
   -8,-8,0x5b,2,
	 0,  0,0x5c,2 | OAM_FLIP_H,
   -8,  0,0x5f,2,
   -8,  8,0x5d,2,
	 0,  8,0x5e,2 | OAM_FLIP_H,
   128
};