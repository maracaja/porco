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

#define spr_jogador(x) ((x) == 0 ? spr_jogador_correndo1 : spr_jogador_correndo2)
#define spr_diabito(x) ((x) == 0 ? spr_diabito_correndo1 : spr_diabito_correndo2)
#define spr_edson(x) ((x) == 0 ? spr_edson_correndo1 : spr_edson_correndo2)
#define spr_tigre(x) ((x) == 0 ? spr_tigre_correndo1 : spr_tigre_correndo2)

#endif