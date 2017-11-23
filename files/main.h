
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_can.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_iwdg.h"

#include "CAN.h"
#include "GPIO.h"
#include "rcc.h"
#include "timer.h"


#define sys_clock 24000
#define set_strobe 		GPIO_SetBits(GPIOA, GPIO_Pin_15);		//строб записи в регистр
#define reset_strobe 	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
#define is_button_sgu	 	1<<0
#define is_button_left	 	1<<1
#define is_button_beam		1<<2
#define is_button_inside	1<<3
#define is_button_rear		1<<4
#define is_button_right		1<<5
#define is_button_strobe	1<<6
#define is_button_search	1<<7
#define set_water_14		GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define reset_water_14		GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define set_water_12		GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define reset_water_12		GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define set_water_34		GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define reset_water_34		GPIO_ResetBits(GPIOB, GPIO_Pin_4)
#define set_water_1			GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define reset_water_1		GPIO_ResetBits(GPIOA, GPIO_Pin_10)
#define set_foam_14			GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define reset_foam_14		GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define set_foam_12			GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define reset_foam_12		GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define set_foam_34			GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define reset_foam_34		GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define set_foam_1			GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define reset_foam_1		GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#define sgu_mask			1<<0
#define left_mask			1<<1
#define beam_mask			1<<2
#define inside_mask			1<<3
#define rear_mask			1<<4
#define right_mask			1<<5
#define strobe_mask			1<<6
#define search_mask			1<<7
#define sgu_light_mask		1<<13
#define left_light_mask		1<<12
#define beam_light_mask		1<<9
#define inside_light_mask	1<<0
#define rear_light_mask		1<<5
#define right_light_mask	1<<4
#define strobe_light_mask	1<<2
#define search_light_mask	1<<1
#define left_door_light_mask		1<<14
#define right_door_light_mask		1<<3
#define left_stair_light_mask		1<<11
#define right_stair_light_mask		1<<7
#define left_roll_light_mask		1<<10
#define right_roll_light_mask		1<<6
#define rear_roll_light_mask		1<<8

#define config_address 0x08007800

void flash_erase(void);
void flash_prog_all(void);

extern uint16_t light_mask;	//0 - осв отсеков
						//1 - фара-искатель
						//2 - строб
						//3 - двери справа
						//4 - осв справа
						//5 - зад прожектор
						//6 - отсек справа
						//7 - ступень справа
						//8 - задний отсек
						//9 - маяк
						//10 - отсек слева
						//11 - ступень слева
						//12 - осв слева
						//13 - СГУ
						//14 - двери слева
extern uint16_t light_out;
extern uint8_t is_idle, left, right;
extern uint8_t butt, blink, can_on, pwm_count, can_error, light_buf,
levels,	//0-3 - вода
		//4-7 - пена
buttons, butt_mask;	//0 - СГУ
					//1 - освещение слева
					//2 - маяк
					//3 - освещение отсеков
					//4 - задний прожектор
					//5 - освещение справа
					//6 - строб
					//7 - фара-искатель
extern uint8_t b[16], level[8];

typedef struct {
	uint8_t		roll_rear;
	uint8_t		roll_left;
	uint8_t		roll_right;
	uint8_t		stair_rear;
	uint8_t		stair_left;
	uint8_t		stair_right;
	uint8_t		doors;
	uint8_t		light_comm;
	uint8_t		light_comm_front;
	uint8_t		light_comm_rear;
} communicate;

extern communicate can;

extern CanTxMsg TxMessage;
extern GPIO_InitTypeDef GPIO_InitStructure;
extern NVIC_InitTypeDef NVIC_InitStructure;
extern CAN_InitTypeDef CAN_InitStructure;
extern CAN_FilterInitTypeDef CAN_FilterInitStructure;
extern SPI_InitTypeDef SPI_InitStructure;

#endif
