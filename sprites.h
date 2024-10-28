#ifndef SPRITES_H
#define SPRITES_H

const unsigned char spr_moeda[] = {
    - 8,- 8,0x04,3,
    0,- 8,0x05,3,
    - 8,  0,0x05,3|OAM_FLIP_H|OAM_FLIP_V,
    0,  0,0x04,3|OAM_FLIP_H|OAM_FLIP_V,
    128
};

const unsigned char spr_titia[] = {
    - 8,-16,0x0c,2,
    - 8,- 8,0x0d,2,
    - 8,  0,0x0e,2,
    0,  0,0x0e,2|OAM_FLIP_H,
    0,- 8,0x0d,2|OAM_FLIP_H,
    0,-16,0x0c,2|OAM_FLIP_H,
    128
};

const unsigned char spr_bola[] = {
    -8, -8, 0x1c, 0,
    0, -8, 0x1d, 0,
    -8, 0, 0x1e, 0,
    0, 0, 0x1f, 0,
    128
};

const unsigned char spr_liberta[] = {
    - 8,  0,0x18,3,
    0,  0,0x19,3,
    - 8,- 8,0x16,3,
    0,- 8,0x17,3,
    - 8,-16,0x14,3,
    0,-16,0x15,3,
    128
};

const unsigned char spr_nrg[] = {
    0,- 8,0x80,0,
    0,  0,0x81,0,
    128
};

const unsigned char spr_caim_parado[] = {
    - 8,- 8,0x84,1,
    - 8,  0,0x85,1,
    0,  0,0x85,1|OAM_FLIP_H,
    0,- 8,0x84,1|OAM_FLIP_H,
    128
};

const unsigned char spr_caim_correndo1[] = {
    - 8,- 8,0x84,1,
    - 8,  0,0x86,1,
    0,- 8,0x87,1|OAM_FLIP_H,
    0,  0,0x88,1|OAM_FLIP_H,
    128
};

const unsigned char spr_caim_correndo2[] = {
    0,- 8,0x84,1|OAM_FLIP_H,
    0,  0,0x86,1|OAM_FLIP_H,
    - 8,- 8,0x87,1,
    - 8,  0,0x88,1,
    128
};

#define spr_caim(x) ((x) == 0 ? spr_caim_correndo1 : spr_caim_correndo2)

#endif