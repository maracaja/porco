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

unsigned char spr_caim_parado[] = {
    0,-8,0x12,1,
    0, 0,0x13,1,
    128
};

unsigned char spr_caim_correndo1[] = {
    0,-8,0x14,1,
    0, 0,0x15,1,
    128
};

unsigned char spr_caim_correndo2[] = {
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