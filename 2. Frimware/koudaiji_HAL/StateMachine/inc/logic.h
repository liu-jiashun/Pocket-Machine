#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <stdbool.h>
#include <stdint.h>

extern volatile bool manual_claib_finish;
extern uint8_t select_color;
extern uint8_t select_shape;

extern uint8_t ColorStr[][8];
extern uint8_t ShapeStr[][16];
extern uint8_t ColorStrZh[][8];
extern uint8_t ShapeStrZh[][16];

void Logic_Matrix_Key(void);
void Logic_Touch_Key(void);
void Logic_Tripod_Calib(void);
void Logic_System_Check(void);

#endif



