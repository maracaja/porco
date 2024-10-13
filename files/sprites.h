const unsigned char spr_bloco[] = {

	-8,-8,0x01,2,
	  0,-8,0x01,2 | OAM_FLIP_H,
	-8,  0,0x01,2 | OAM_FLIP_V,
	  0,  0,0x01,2 | OAM_FLIP_H | OAM_FLIP_V,
	128

};

const unsigned char spr_moeda[] = {

	-8,-8,0x02,3,
	  0,-8,0x03,3,
	  0,  0,0x02,3 | OAM_FLIP_H | OAM_FLIP_V,
	-8,  0,0x03,3 | OAM_FLIP_H | OAM_FLIP_V,
	128

};

const unsigned char spr_nrg[] = {

	-8,  0,0x04,0,
	-8,-8,0x05,0,
	128

};

const unsigned char spr_bola[] = {

	-8,-8,0x07,0,
	  0,-8,0x08,0,
	  0,  0,0x09,0,
	-8,  0,0x0a,0,
	128

};

const unsigned char spr_caim_parado[] = {

	-8,-8,0x0c,1,
	  0,-8,0x0c,1 | OAM_FLIP_H,
	-8,  0,0x0d,1,
	  0,  0,0x0d,1 | OAM_FLIP_H,
	128

};

const unsigned char spr_caim_correndo[] = {

	-8,-8,0x0c,1,
	-8,  0,0x0f,1,
	  0,  0,0x0e,1 | OAM_FLIP_H,
	  0,-8,0x10,1 | OAM_FLIP_H,
	128

};

const unsigned char spr_titia[] = {
	-8,-8,0x61,2,
	-8,  0,0x62,2,
	  0,-8,0x61,2 | OAM_FLIP_H,
	  0,  0,0x62,2 | OAM_FLIP_H,
	-8,  8,0x63,2,
	  0,  8,0x63,2 | OAM_FLIP_H,
	128
};



const unsigned char spr_liberta[] = {

	  0,-16,0x66,3,
	-8,-8,0x64,3,
	  0,-8,0x67,3,
	-8,  0,0x68,3,
	  0,  0,0x69,3,
	-8,-16,0x6a,3,
	128

};

const unsigned char* const sprites_list[] = {

	spr_bloco,
	spr_moeda,
	spr_nrg,
	spr_bola,
	spr_caim_parado,
	spr_caim_correndo,
	spr_titia,
	spr_liberta

};
