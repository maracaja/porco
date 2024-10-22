const unsigned char spr_moeda[] = {

	-8,-8,0x03,3,
	  0,-8,0x04,3,
	-8,  0,0x04,3 | OAM_FLIP_H | OAM_FLIP_V,
	  0,  0,0x03,3 | OAM_FLIP_H | OAM_FLIP_V,
	128

};

const unsigned char spr_nrg[] = {

	  0,-8,0x05,0,
	  0,  0,0x15,0,
	128

};

const unsigned char spr_liberta[] = {

	-8,  0,0x1c,3,
	  0,  0,0x1d,3,
	-8,-8,0x0c,3,
	  0,-8,0x0d,3,
	-8,-16,0x13,3,
	  0,-16,0x14,3,
	128

};

const unsigned char spr_bola[] = {

	-8,-8,0x07,0,
	  0,-8,0x08,0,
	-8,  0,0x17,0,
	  0,  0,0x18,0,
	128

};

const unsigned char spr_caim_parado[] = {

	-8,-8,0x09,1,
	-8,  0,0x19,1,
	  0,-8,0x09,1 | OAM_FLIP_H,
	  0,  0,0x19,1 | OAM_FLIP_H,
	128

};

const unsigned char spr_caim_correndo[] = {

	-8,-8,0x09,1,
	-8,  0,0x1a,1,
	  0,  0,0x1b,1 | OAM_FLIP_H,
	  0,-8,0x0a,1 | OAM_FLIP_H,
	128

};

const unsigned char* const spr_list[] = {

	spr_moeda,
	spr_nrg,
	spr_liberta,
	spr_bola,
	spr_caim_parado,
	spr_caim_correndo

};

// Animacao do Caim
const unsigned char* const spr_caim[4][4] = {
    {spr_caim_parado, spr_caim_correndo, spr_caim_parado, spr_caim_correndo},
    {spr_caim_correndo,spr_caim_parado, spr_caim_correndo, spr_caim_parado},
    {spr_caim_parado, spr_caim_correndo, spr_caim_parado, spr_caim_correndo},
    {spr_caim_correndo,spr_caim_parado, spr_caim_correndo, spr_caim_parado}
};