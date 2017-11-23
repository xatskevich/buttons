
#include "main.h"

void Init_SPI(){

	SPI_InitTypeDef SPI_InitStructure;

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; //полный дуплекс
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b; // передаем по 8 бит
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // Полярность и
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // фаза тактового сигнала
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // Управлять состоянием сигнала NSS программно
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // Предделитель SCK
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Первым отправляется старший бит
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // Режим - мастер
	SPI_Init(SPI1, &SPI_InitStructure); //Настраиваем SPI1
	SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1....

	// Поскольку сигнал NSS контролируется программно, установим его в единицу
	// Если сбросить его в ноль, то наш SPI модуль подумает, что
	// у нас мультимастерная топология и его лишили полномочий мастера.
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}


void Init_GPIO(){

	GPIO_StructInit(&GPIO_InitStructure);

	//цифровые входы
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//цифровые выходы
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//CAN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//подключение портов
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);

	//SPI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//подключение портов
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);

}
