#ifndef SPRITES_H
#define SPRITES_H

extern const unsigned char spr_moeda[];
extern const unsigned char spr_titia[];
extern const unsigned char spr_liberta[];
extern const unsigned char spr_nrg[];
extern const unsigned char spr_jogador_parado[];
extern const unsigned char spr_jogador_correndo1[];
extern const unsigned char spr_jogador_correndo2[];
extern const unsigned char spr_goleiro[];
extern const unsigned char spr_arbitro[];
extern const unsigned char spr_edson_parado[];
extern const unsigned char spr_edson_correndo1[];
extern const unsigned char spr_edson_correndo2[];
extern const unsigned char spr_diabito_parado[];
extern const unsigned char spr_diabito_correndo1[];
extern const unsigned char spr_diabito_correndo2[];
extern const unsigned char spr_tigre_parado[];
extern const unsigned char spr_tigre_correndo1[];
extern const unsigned char spr_tigre_correndo2[];
extern const unsigned char spr_adv1_parado[];
extern const unsigned char spr_adv1_correndo1[];
extern const unsigned char spr_adv1_correndo2[];
extern const unsigned char spr_adv2_parado[];
extern const unsigned char spr_adv2_correndo1[];
extern const unsigned char spr_adv2_correndo2[];
extern const unsigned char spr_adv3_parado[];
extern const unsigned char spr_adv3_correndo1[];
extern const unsigned char spr_adv3_correndo2[];

#define spr_jogador(x) ((x) ? spr_jogador_correndo1 : spr_jogador_correndo2)
#define spr_adv1(x) ((x) ? spr_adv1_correndo1 : spr_adv1_correndo2)
#define spr_adv2(x) ((x) ? spr_adv2_correndo1 : spr_adv2_correndo2)
#define spr_adv3(x) ((x) ? spr_adv3_correndo1 : spr_adv3_correndo2)
#define spr_diabito(x) ((x) ? spr_diabito_correndo1 : spr_diabito_correndo2)
#define spr_edson(x) ((x) ? spr_edson_correndo1 : spr_edson_correndo2)
#define spr_tigre(x) ((x) ? spr_tigre_correndo1 : spr_tigre_correndo2)

#endif