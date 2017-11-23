
#include "main.h"

uint16_t light_mask;	//0 - осв отсеков
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
uint16_t light_out;
uint8_t is_idle, left, right;
uint8_t butt, blink, can_on, pwm_count, can_error, light_buf,
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
uint8_t b[16], level[8];

communicate can;

CanTxMsg TxMessage;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
CAN_InitTypeDef CAN_InitStructure;
CAN_FilterInitTypeDef CAN_FilterInitStructure;
SPI_InitTypeDef SPI_InitStructure;

int main(void)
{
	uint32_t temp;


	butt_mask = 0;
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
	//загрузка данных из flash
	temp=(*(__IO uint32_t*)config_address);
	if(temp!=0xffffffff) butt_mask=temp;

	butt_mask = 0xFF;
	//butt_mask = 0x3F;	//без стробов и ФИ

	Init_IWDG();			//сторожевой

	Init_RCC();			//тактирование блоков
	Init_GPIO();		//инициализация портов
	Init_SPI();
	Init_CAN();
	Init_Timer();

	if(butt_mask & inside_mask){ 	//включаем освещение отсеков
		buttons |= inside_mask;
		light_mask |= inside_light_mask;
	}

    while(1)
    {


    }
}

//стирание flash
void flash_erase(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = config_address;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);//while (!(FLASH->SR & FLASH_SR_EOP));
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
}

//запись данных во flash
void flash_prog_all(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint16_t*)config_address = butt_mask;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;

	FLASH->CR &= ~(FLASH_CR_PG);
}
