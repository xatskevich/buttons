
#include "main.h"
//#include "CAN.h"

void CEC_CAN_IRQHandler (void)
{
	uint8_t water, foam, tmp;
	static uint8_t door_buf,	//0 - ПЛ
								//1 - ЗЛ
								//2 - ПП
								//3 - ЗП
								//7 - задняя роллета
								//8 - задняя ступень
	roll_buf, stair_buf;	//0 - Л1
							//1 - Л2
							//2 - Л3
							//3 - Л4
							//4 - П1
							//5 - П2
							//6 - П3
							//7 - П4

	if (CAN_GetITStatus(CAN,CAN_IT_FMP0))
	{
		CAN_ClearITPendingBit(CAN, CAN_IT_FMP0);

	}
	CanRxMsg msg_buf;
	CAN_Receive(CAN, CAN_FIFO0, &msg_buf);
	can_on = 1;

	switch(msg_buf.FMI){
	case 0:		//0x018
		water = msg_buf.Data[0];
		foam = msg_buf.Data[1];
		levels = 0;
		if(water > 90){
			levels |= 0xF;	//полный бак
		} else if(water > 65){
			levels |= 7;
		} else if(water > 40){
			levels |= 3;
		} else if(water >12){
			levels |= 1;
		} else {
			if(blink) levels |= 1;
		}

		if(foam > 90){
			levels |= 0xF0;
		} else if(foam > 65){
			levels |= 0x70;
		} else if(foam > 40){
			levels |= 0x30;
		} else if(foam > 12){
			levels |= 0x10;
		} else {
			if(blink) levels |= 0x10;
		}
		break;
	case 1:		//0x028
		tmp = 0;
		if ((msg_buf.Data[2] & 0x3) == 1) tmp |= 1; //освещение слева
		if ((msg_buf.Data[2] & 0xC) == 4) tmp |= 2; //освещение справа
		if (butt_mask & left_mask) {
			if ((tmp & 1) ^ (light_buf & 1)) { //если освещение слева переключили с пульта
				if (tmp & 1) { //включено
					buttons |= left_mask;
					light_mask |= left_light_mask;
				} else { //выключено
					buttons &= ~(left_mask);
					light_mask &= ~(left_light_mask);
				}
			}
		}
		if (butt_mask & right_mask) {
			if ((tmp & 2) ^ (light_buf & 2)) { //если освещение справа переключили с пульта
				if (tmp & 2) { //включено
					buttons |= right_mask;
					light_mask |= right_light_mask;
				} else { //выключено
					buttons &= ~(right_mask);
					light_mask &= ~(right_light_mask);
				}
			}
		}
		light_buf = tmp;
		break;
	case 2:		//0x040
		if((msg_buf.Data[0] & 0x3) == 1) door_buf |= 1;		//ЛП дверь открыта
		if((msg_buf.Data[0] & 0xC) == 4) door_buf |= 2;		//ЛЗ дверь открыта
		if((msg_buf.Data[0] & 0x3) == 0) door_buf &= ~1;	//ЛП дверь закрыта
		if((msg_buf.Data[0] & 0xC) == 0) door_buf &= ~2;	//ЛЗ дверь закрыта
		if(door_buf & 3) light_mask |= left_door_light_mask;
		else light_mask &= ~(left_door_light_mask);

		if((msg_buf.Data[0] & 0x30) == 0x10) door_buf |= 4;		//ПП дверь открыта
		if((msg_buf.Data[0] & 0xC0) == 0x40) door_buf |= 8;		//ПЗ дверь открыта
		if((msg_buf.Data[0] & 0x30) == 0) door_buf &= ~4;		//ПП дверь закрыта
		if((msg_buf.Data[0] & 0xC0) == 0) door_buf &= ~8;		//ПЗ дверь закрыта
		if(door_buf & 0xC) light_mask |= right_door_light_mask;
		else light_mask &= ~(right_door_light_mask);
		break;
	case 3:		//0x0AC
		butt_mask = msg_buf.Data[0];		//прием конфигурации пульта
		//стирание flash
		flash_erase();
		//запись
		flash_prog_all();
		break;
	case 4: 	//0x050
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 0);
		break;
	case 5:		//0x051
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 1);
		break;
	case 6:		//0x052
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 2);
		break;
	case 7:		//0x053
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 3);
		break;
	case 8:		//0x060
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 4);
		break;
	case 9:		//0x061
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 5);
		break;
	case 10:	//0x062
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 6);
		break;
	case 11:	//0x063
		roll_inputs(msg_buf.Data[0], &roll_buf, &stair_buf, 7);
		break;
	case 12:	//0x056
		if((msg_buf.Data[0] & 0x3) == 1) door_buf |= 0x40;	//задняя роллета открыта
		if((msg_buf.Data[0] & 0xC) == 4) door_buf |= 0x80;	//задняя ступень открыта
		if((msg_buf.Data[0] & 0x3) == 0) door_buf &= ~0x40;	//задняя роллета закрыта
		if((msg_buf.Data[0] & 0xC) == 0) door_buf &= ~0x80;	//задняя ступень закрыта
		if(door_buf & 0xC0) light_mask |= rear_roll_light_mask;
		else light_mask &= ~(rear_roll_light_mask);
		break;
	}


}

void roll_inputs(uint8_t msg, uint8_t* roll, uint8_t* stair, uint8_t shift){
	if((msg & 0x3) == 1) *roll |= 1<<shift;		//роллета открыта
	if((msg & 0xC) == 4) *stair |= 1<<shift;		//ступень открыта
	if((msg & 0x3) == 0) *roll &= ~(1<<shift);		//роллета открыта
 	if((msg & 0xC) == 0) *stair &= ~(1<<shift);	//ступень закрыта
	if(*roll & 0xF) light_mask |= left_roll_light_mask;		//индикатор левой роллеты
	else light_mask &= ~(left_roll_light_mask);
	if(*roll & 0xF0) light_mask |= right_roll_light_mask;	//индикатор правой роллеты
	else light_mask &= ~(right_roll_light_mask);
	if(*stair & 0xF) light_mask |= left_stair_light_mask;	//индикатор левой ступени
	else light_mask &= ~(left_stair_light_mask);
	if(*stair & 0xF0) light_mask |= right_stair_light_mask;	//индикатор правой ступени
	else light_mask &= ~(right_stair_light_mask);
}

void Init_CAN(){
	//volatile int i;

	/* CAN register init */
	CAN_DeInit(CAN);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 250 kBps (CAN clocked at 24 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 6;		//24M / 16 / 6 = 250k
	CAN_Init(CAN, &CAN_InitStructure);
	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x018<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x028<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 2;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x040<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 3;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0AC<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 4;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x050<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 5;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x051 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 6;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x052 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 7;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x053 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 8;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x060 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 9;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x061 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 10;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x062 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 11;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x063 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 12;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x056 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	//вектор
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
