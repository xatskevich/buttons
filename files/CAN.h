
#ifndef __CAN_H
#define __CAN_H

void Init_CAN(void);
void roll_inputs(uint8_t msg, uint8_t* roll, uint8_t* stair, uint8_t shift);

#endif
