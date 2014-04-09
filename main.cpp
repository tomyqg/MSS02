
#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include <string.h>

__IO uint16_t aADCDualConvertedValue[4];//
uint16_t aADCavr[4];
uint32_t aADCBuff[4];

MenuManager Menu;
MenuItem RT[2]; //IO item
MenuItem SYS[8]; //System item
MenuItem MIN[46]; //MIN01
MenuItem MSY[25]; //MSY02
MenuItem MDI[9]; //MDI02
MenuItem MDO[37]; //MDO02



u8 profibus_out_table[OUTPUT_DATA_SIZE];


u8 profibus_in_table[INPUT_DATA_SIZE];


u16 *mbs_table[BUFF_SIZE]; //modbus table









modbusSlave mbs_Slave; //object mbSlave
__IO uint16_t x;
int main(void)
{
	SystemInit();

	initRTC();










	Menu.config();
	gpioInit(Menu.m_pMenuLCD, Menu.Flash.spiNum);
	usrMenuBuild();

	Menu.readFlash();
	Menu.selectGroup(SYS[1].getValue());
	Menu.selectRoot();
//
//	RTC_TimeTypeDef tr;
//
//	u16 cnt1;
//
//	float time;






    initModbusTimer();
	initModbusUsart();
    mbs_Slave.configureAddress(10);

//    initProfibusTimer();
//	initProfibusUsart();
//	initProfibus();







	NVIC_InitTypeDef NVIC_Init_Structure;
	TIM_TimeBaseInitTypeDef TimeBaseInit_Structure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = 49; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TimeBaseInit_Structure);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ARRPreloadConfig(TIM2, ENABLE);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);







	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period =4 ; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TimeBaseInit_Structure);
	TIM_Cmd(TIM4, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);




	//SysTick_Config(SystemCoreClock / 10000);



	while (1)
	{

		profibusDataExchange();

			//MIN[4].setValue((float) x);


	//	RTC_GetTime( RTC_Format_BIN, &tr);

		//time = (float) tr.RTC_Seconds;
		//time /= 100;
		//time += (float) tr.RTC_Minutes;

		//MIN[1].setValue(time);

		Menu.setDefaultValue(SYS[6].getValue());
	    Menu.selectGroup(SYS[1].getValue());
		Menu.Up(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1));
		Menu.Down(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));
		Menu.Select(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
	    Menu.TimerReset(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) || GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) || GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));
		Menu.Display();

	}

}
//main

