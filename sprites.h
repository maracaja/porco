#ifndef SPRITES_H
#define SPRITES_H

extern unsigned char spr_moeda[];
extern unsigned char spr_titia[];
extern unsigned char spr_liberta[];
extern unsigned char spr_nrg[];
extern unsigned char spr_jogador_parado[];
extern unsigned char spr_jogador_correndo1[];
extern unsigned char spr_jogador_correndo2[];
extern unsigned char spr_goleiro[];
extern unsigned char spr_arbitro[];
extern unsigned char spr_edson_parado[];
extern unsigned char spr_edson_correndo1[];
extern unsigned char spr_edson_correndo2[];
extern unsigned char spr_diabito_parado[];
extern unsigned char spr_diabito_correndo1[];
extern unsigned char spr_diabito_correndo2[];
extern unsigned char spr_tigre_parado[];
extern unsigned char spr_tigre_correndo1[];
extern unsigned char spr_tigre_correndo2[];
extern unsigned char spr_adv1_parado[];
extern unsigned char spr_adv1_correndo1[];
extern unsigned char spr_adv1_correndo2[];
extern unsigned char spr_adv2_parado[];
extern unsigned char spr_adv2_correndo1[];
extern unsigned char spr_adv2_correndo2[];
extern unsigned char spr_adv3_parado[];
extern unsigned char spr_adv3_correndo1[];
extern unsigned char spr_adv3_correndo2[];

#define spr_jogador(x) ((x) ? spr_jogador_correndo1 : spr_jogador_correndo2)
#define spr_adv1(x) ((x) ? spr_adv1_correndo1 : spr_adv1_correndo2)
#define spr_adv2(x) ((x) ? spr_adv2_correndo1 : spr_adv2_correndo2)
#define spr_adv3(x) ((x) ? spr_adv3_correndo1 : spr_adv3_correndo2)
#define spr_diabito(x) ((x) ? spr_diabito_correndo1 : spr_diabito_correndo2)
#define spr_edson(x) ((x) ? spr_edson_correndo1 : spr_edson_correndo2)
#define spr_tigre(x) ((x) ? spr_tigre_correndo1 : spr_tigre_correndo2)

#endif