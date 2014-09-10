/*
 * global.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: TEMMKA
 *  Description: 
 */

#include <global.h>
#include "math.h"
global::global()
{
	// TODO Auto-generated constructor stub

}

void global::init(void)
{

	initRTC();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Menu.config();
	Menu.PasswordSp = &SYS[7];
	Menu.PasswordPv = &SYS[0];
	gpioInit(Menu.m_pMenuLCD, Menu.Flash.spiNum);
	usrMenuBuild();

	Menu.readFlash();

	SYS[0].pValue = 0.0f;
	Menu.selectGroup(SYS[1].getValue());
	Menu.selectRoot();
	mbs_table[301] = (u16*) (0x1FFF7A10);
	mbs_table[302] = (u16*) (0x1FFF7A12);
	mbs_table[303] = (u16*) (0x1FFF7A14);
	mbs_table[304] = (u16*) (0x1FFF7A16);
	mbs_table[305] = (u16*) (0x1FFF7A18);
	mbs_table[306] = (u16*) (0x1FFF7A1A);



	oMin.Ch1_FrRms.AvValueFreq = (u16) (MIN[11].pValue);
	oMin.Ch2_FrRms.AvValueFreq = (u16) (MIN[12].pValue);
	oMin.Ch1_FrRms.AvValueRms = (u16) MIN[20].getValue();

	oMin.init(MIN);

	switch ((u32) SYS[4].getValue())
	{
	case 0:
		UART_SPEED = 300;
		break;
	case 1:
		UART_SPEED = 600;
		break;
	case 2:
		UART_SPEED = 1200;
		break;
	case 3:
		UART_SPEED = 2400;
		break;
	case 4:
		UART_SPEED = 4800;
		break;
	case 5:
		UART_SPEED = 9600;
		break;
	case 6:
		UART_SPEED = 14400;
		break;
	case 7:
		UART_SPEED = 19200;
		break;
	case 8:
		UART_SPEED = 38400;
		break;
	case 9:
		UART_SPEED = 56000;
		break;
	case 10:
		UART_SPEED = 57600;
		break;
	case 11:
		UART_SPEED = 115200;
		break;

	default:
		UART_SPEED = 19200;
		break;
	}

	initModbusTimer();
	initModbusUsart();
	mbs_Slave.configureAddress((u8) SYS[3].getValue());

	//  initProfibusTimer();
	//	initProfibusUsart();
	//	initProfibus();

	NVIC_InitTypeDef NVIC_Init_Structure;
	TIM_TimeBaseInitTypeDef TimeBaseInit_Structure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//
//	TimeBaseInit_Structure.TIM_Prescaler = 83;
//	TimeBaseInit_Structure.TIM_Period = 49; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
//	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM2, &TimeBaseInit_Structure);
//	TIM_Cmd(TIM2, ENABLE);
//	TIM_ARRPreloadConfig(TIM2, ENABLE);
//
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//
//	NVIC_Init_Structure.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 5;
//	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_Init_Structure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = 9;
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TimeBaseInit_Structure);
	TIM_Cmd(TIM4, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = 1000000; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV4;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TimeBaseInit_Structure);
	TIM_Cmd(TIM5, ENABLE);
	TIM_ARRPreloadConfig(TIM5, ENABLE);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);



}

void global::initModbusUsart(void)
{

	NVIC_InitTypeDef NVIC_Init_Structure;
	USART_InitTypeDef USART_Init_Structure;
	GPIO_InitTypeDef GPIO_Init_Structure;

	RCC_AHB1PeriphClockCmd(MODBUS_USART_TX_PORT_AF, ENABLE);
	RCC_AHB1PeriphClockCmd(MODBUS_USART_RX_PORT_AF, ENABLE);
	RCC_APB2PeriphClockCmd(MODBUS_USART_RCC, ENABLE);

	//  Connect PXx to USARTx_Tx
	GPIO_PinAFConfig(MODBUS_USART_TX_PORT, MODBUS_USART_TX_PINSOURCE,
	MODBUS_USART_GPIO_AF);

	//  Connect PXx to USARTx_Rx
	GPIO_PinAFConfig(MODBUS_USART_RX_PORT, MODBUS_USART_RX_PINSOURCE,
	MODBUS_USART_GPIO_AF);

	GPIO_Init_Structure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_100MHz;

	//Configure USART Tx as alternate function
	GPIO_Init_Structure.GPIO_Pin = MODBUS_USART_TX_PIN;
	GPIO_Init(MODBUS_USART_TX_PORT, &GPIO_Init_Structure);

	//Configure USART Rx as alternate function
	GPIO_Init_Structure.GPIO_Pin = MODBUS_USART_RX_PIN;
	GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_Init_Structure);

	USART_Init_Structure.USART_BaudRate = UART_SPEED;
	USART_Init_Structure.USART_WordLength = MODBUS_USART_WORDLENGTH;
	USART_Init_Structure.USART_StopBits = MODBUS_USART_STOPBITS;
	USART_Init_Structure.USART_Parity = MODBUS_USART_PARITY;
	USART_Init_Structure.USART_HardwareFlowControl = MODBUS_USART_HWFWCTRL;
	USART_Init_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MODBUS_USART, &USART_Init_Structure);
	USART_Cmd(MODBUS_USART, ENABLE);
	USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = MODBUS_USART_IRQN;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);
	NVIC_EnableIRQ(MODBUS_USART_IRQN);
}

void global::initModbusTimer(void)
{
	uint16_t bytePerSecond; //  serial speed (bit/s)  / 8
	float timeForByte; // 1 byte transmit/recive time
	uint16_t overtime; // overtime
	uint16_t TimPeriod;

	bytePerSecond = UART_SPEED / 8;

	timeForByte = (1.0 / (float) bytePerSecond) * 1000000.0; // = N mks  (*1000000 - correction for timer )

	overtime = 300; // added to timeForByte

	TimPeriod = (uint16_t) timeForByte + overtime;

	NVIC_InitTypeDef NVIC_Init_Structure;
	TIM_TimeBaseInitTypeDef TimeBaseInit_Structure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = TimPeriod; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MODBUS_TIMER, &TimeBaseInit_Structure);
	TIM_Cmd(MODBUS_TIMER, ENABLE);
	TIM_ARRPreloadConfig(MODBUS_TIMER, ENABLE);

	TIM_ITConfig(MODBUS_TIMER, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = MODBUS_TIMER_IRQN;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);

}

void global::cycle(void)
{

	while (1)
	{

		profibusDataExchange();
		changeVisibleItem();

		Menu.Display();
		Menu.setDefaultValue(SYS[6].getValue());
		Menu.selectGroup(SYS[1].getValue());
		Menu.systemRestart(SYS[8]);
		Menu.Up(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1));
		Menu.Down(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));
		Menu.Select(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
		Menu.TimerReset(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));




		if (cycle_cnt > 50)
		{
			SYS[5].setValue(swVersion);

			MIN[8].pValue = oMin.AvrCos;

			cycle_cnt = 0;
			Menu.changeItem(MIN[13], MIN[4]);
			Menu.changeItem(MIN[14], MIN[5]);



			oMin.ZeroOffset[0] = (u16) (MIN[13].pValue);
			oMin.ZeroOffset[1] = (u16) (MIN[14].pValue);

			oMin.Ch1_FrRms.RmsFactor = (MIN[15].pValue) / 1000.0f;
			oMin.Ch2_FrRms.RmsFactor = (MIN[16].pValue) / 1000.0f;

			oMin.Ch1_Alarm.factor = oMin.Ch1_FrRms.RmsFactor;
			oMin.Ch2_Alarm.factor = oMin.Ch2_FrRms.RmsFactor;

			oMin.Ch1_Adc.MaxAvrCount = (u16) MIN[17].pValue; //Max count average
			oMin.Ch2_Adc.MaxAvrCount = (u16) MIN[18].pValue; //Max count average

			oMin.Ch1_FrRms.AvValueRms = (u16) MIN[20].getValue();


			oMin.AC_DC[0] = (u8) MIN[9].getValue();
			oMin.AC_DC[1] = (u8) MIN[10].getValue();

			oMin.Ch1_Alarm.AcDc =  (u8)MIN[9].getValue();
			oMin.Ch1_Alarm.SelectMode = (u8) MIN[21].getValue();
			oMin.Ch1_Alarm.invOut = (u8) MIN[22].getValue();
			oMin.Ch1_Alarm.minValue = MIN[23].getValue();
			oMin.Ch1_Alarm.maxValue = MIN[24].getValue();
			oMin.Ch1_Alarm.avrAl = MIN[25].getValue();
			oMin.Ch1_Alarm.setOut = (u8) MIN[26].getValue();

			if (oMin.Ch1_Alarm.SelectMode == 4.0f)
			{
				oMin.Ch1_Alarm.tOut = (u8) MIN[27].getValue();
			}
			else
			{
				MIN[27].setValue((float) oMin.Ch1_Alarm.tOut);
			}

			oMin.Ch2_Alarm.AcDc = (u8) MIN[10].getValue();
			oMin.Ch2_Alarm.SelectMode = (u8) MIN[28].getValue();
			oMin.Ch2_Alarm.invOut = (u8) MIN[29].getValue();
			oMin.Ch2_Alarm.minValue = MIN[30].getValue();
			oMin.Ch2_Alarm.maxValue = MIN[31].getValue();
			oMin.Ch2_Alarm.avrAl = MIN[32].getValue();
			oMin.Ch2_Alarm.setOut = (u8) MIN[33].getValue();

			if (oMin.Ch2_Alarm.SelectMode == 4.0f)
			{
				oMin.Ch2_Alarm.tOut = (u8) MIN[34].getValue();
			}
			else
			{
				MIN[34].setValue((float) oMin.Ch2_Alarm.tOut);
			}

			oMin.Ch1_Adc.sendToItem(MIN[4]);
			oMin.Ch2_Adc.sendToItem(MIN[5]);

			oMin.Ch1_FrRms.sendToItem(MIN[6], MIN[2]);
			oMin.Ch2_FrRms.sendToItem(MIN[7], MIN[3]);
		}

		cycle_cnt++;


		if (mbs_Slave.act > 0)
		{
			Menu.Flash.writeFloat(mbs_Slave.mbToFlashData.mbAddr * 2, mbs_Slave.mbToFlashData.mbValue);
			mbs_Slave.act = 0;
		}

		Menu.Display();
	}

}

void global::changeVisibleItem(void)
{

	static float mem[5];

	switch ((int) SYS[1].getValue())
	{
	case 1.0:
		if (MIN[1].getValue() != mem[1]) // CREATE comment
		{
			mem[1] = MIN[1].getValue();
			Menu.selectItemNum(1);
			Menu.selectRoot();
		}
	case 2.0:
		if (MSY[1].getValue() != mem[2]) // CREATE comment
		{
			mem[2] = MSY[1].getValue();
			Menu.selectItemNum(2);
		}
	case 3.0:
		if (MDI[1].getValue() != mem[3]) // CREATE comment
		{
			mem[3] = MDI[1].getValue();
			Menu.selectItemNum(3);
		}
	case 4.0:
		if (MDO[1].getValue() != mem[4]) // CREATE comment
		{
			mem[4] = MDO[1].getValue();
			Menu.selectItemNum(4);
		}

		break;
	default:
		Menu.selectItemNum(1);
		break;
	}

}

void global::ptrFtoI(float *value, u16 **buff, u16 number)
{
	buff[number] = (u16*) value;
	buff[number + 1] = buff[number] + 1;
}

void global::usrMenuBuild(void)
{

	u16 mbsCnt = 0;		// counter of modbus data record

	u16 adr = 0;

	RT[0].config(sym_r, 0, 0, 9999, 1, 0, adr, OUT_VALUE); //Отображаемое значение на экране

	Menu.addRoot(0, &RT[0]);
	ptrFtoI(&RT[0].pValue, mbs_table, mbsCnt);

	SYS[0].config(sym_C, 1, 0, 9999, 1, 0, adr += 4, PARAMETR); //Ввод пароля для входа в меню
	SYS[1].config(sym_C, 2, 1, 4, 1, 1, adr += 4, PARAMETR); //Тип модуля
	SYS[2].config(sym_C, 3, 1, 2, 1, 1, adr += 4, PARAMETR); //Тип протокола
	SYS[3].config(sym_C, 4, 0, 255, 1, 1, adr += 4, PARAMETR); //Адрес
	SYS[4].config(sym_C, 5, 0, 11, 1, 5, adr += 4, PARAMETR); //Скорость передачи данных
	SYS[5].config(sym_C, 6, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Версия прошивки
	SYS[6].config(sym_C, 7, 0, 1, 1, 0, adr += 4, PARAMETR); //Вернуть к заводским настройкам
	SYS[7].config(sym_C, 8, 0, 9999, 1, 0, adr += 4, PARAMETR); //Пароль для доступа
	SYS[8].config(sym_C, 9, 0, 1, 1, 0, adr += 4, PARAMETR); //Сброс контроллера

	for (u8 i = 0; i < 8; i++)
	{
		Menu.addItem(0, &SYS[i]);

		ptrFtoI(&SYS[i].pValue, mbs_table, mbsCnt += 2);

	}
	Menu.addLast(0, &SYS[8]);
	ptrFtoI(&SYS[8].pValue, mbs_table, mbsCnt += 2);

	MIN[1].config(sym_n, 1, 1, 8, 1, 9, adr += 4, PARAMETR); //Отображаемый сигнал на экране
	MIN[2].config(sym_n, 2, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Текущее значение RMS 1
	MIN[3].config(sym_n, 3, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Текущее значение RMS 2
	MIN[4].config(sym_n, 4, 0, 4096, 0, 0, adr += 4, OUT_VALUE); //Значение текущее АЦП 1
	MIN[5].config(sym_n, 5, 0, 4096, 0, 0, adr += 4, OUT_VALUE); //Значение текущее АЦП 2
	MIN[6].config(sym_n, 6, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Значение текущее частоты 1
	MIN[7].config(sym_n, 7, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Значение текущее частоты 2
	MIN[8].config(sym_n, 8, 0, 9999, 0, 0, adr += 4, OUT_VALUE); //Значение текущее косинуса
	MIN[9].config(sym_n, 9, 0, 1, 1, 0, adr += 4, PARAMETR); //Тип сигнала переменка/постоянка 1
	MIN[10].config(sym_n, 10, 0, 1, 1, 0, adr += 4, PARAMETR); //Тип сигнала переменка/постоянка 2
	MIN[11].config(sym_n, 11, 1, 99, 1, 25, adr += 4, PARAMETR); //Количество усреднений данных 1
	MIN[12].config(sym_n, 12, 1, 99, 1, 25, adr += 4, PARAMETR); //Количество усреднений данных 2
	MIN[13].config(sym_n, 13, 0, 0, 2048, 2048, adr += 4, PARAMETR); //Задание нуля 1
	MIN[14].config(sym_n, 14, 0, 0, 2048, 2048, adr += 4, PARAMETR); //Задание нуля 2
	MIN[15].config(sym_n, 15, 0, 9999, 1, 1, adr += 4, PARAMETR); //Коэффициент канала 1
	MIN[16].config(sym_n, 16, 0, 9999, 1, 1, adr += 4, PARAMETR); //Коэффициент канала 2
	MIN[17].config(sym_n, 17, 5, 50, 1, 25, adr += 4, PARAMETR); //Усреднение АЦП канал 1
	MIN[18].config(sym_n, 18, 5, 50, 1, 25, adr += 4, PARAMETR); //Усреднение АЦП канал 2
	MIN[19].config(sym_n, 19, 0, 1, 1, 0, adr += 4, PARAMETR); //Реверс знак косинуса
	MIN[20].config(sym_n, 20, 1, 9999, 1, 500, adr += 4, PARAMETR); //Усреднение значения косинуса
	MIN[21].config(sym_n, 21, 1, 4, 1, 1, adr += 4, PARAMETR); //Выход аварии 1 назначение
	MIN[22].config(sym_n, 22, 0, 1, 1, 0, adr += 4, PARAMETR); //Инвертирование выхода 1
	MIN[23].config(sym_n, 23, -9999, 9999, 1, 0, adr += 4, PARAMETR); //Значение уставки минимум 1
	MIN[24].config(sym_n, 24, -9999, 9999, 1, 1, adr += 4, PARAMETR); //Значение уставки максимум 1
	MIN[25].config(sym_n, 25, 0, 9999, 1, 50, adr += 4, PARAMETR); //Усреднение аварии 1
	MIN[26].config(sym_n, 26, 0, 1, 1, 0, adr += 4, PARAMETR); //Удержание аварии 1
	MIN[27].config(sym_n, 27, 0, 1, 1, 0, adr += 4, PARAMETR); //Значение выход 1
	MIN[28].config(sym_n, 28, 1, 4, 1, 1, adr += 4, PARAMETR); //Выход аварии 2 назначение
	MIN[29].config(sym_n, 29, 0, 1, 1, 0, adr += 4, PARAMETR); //Инвертирование выхода 2
	MIN[30].config(sym_n, 30, -9999, 9999, 1, 0, adr += 4, PARAMETR); //Значение уставки минимум 2
	MIN[31].config(sym_n, 31, -9999, 9999, 1, 1, adr += 4, PARAMETR); //Значение уставки максимум 2
	MIN[32].config(sym_n, 32, 0, 99, 1, 5, adr += 4, PARAMETR); //Усреднение аварии 2
	MIN[33].config(sym_n, 33, 0, 1, 1, 0, adr += 4, PARAMETR); //Удержание аварии 2
	MIN[34].config(sym_n, 34, 0, 1, 1, 0, adr += 4, PARAMETR); //Значение выход 2
	MIN[35].config(sym_n, 35, 1, 2, 1, 1, adr += 4, PARAMETR); //Дискретный вход 1 настройка
	MIN[36].config(sym_n, 36, 0, 1, 1, 0, adr += 4, PARAMETR); //Дискретный вход 1 значение
	MIN[37].config(sym_n, 37, 1, 2, 1, 1, adr += 4, PARAMETR); //Дискретный вход 2 настройка
	MIN[38].config(sym_n, 38, 0, 1, 1, 0, adr += 4, PARAMETR); //Дискретный вход 2 значение
	MIN[39].config(sym_n, 39, 0, 0, 0, 0, adr += 4, PARAMETR); //Резерв
	MIN[40].config(sym_n, 40, 0, 0, 0, 0, adr += 4, PARAMETR); //Резерв

	Menu.addRoot(1, &MIN[1]);
	ptrFtoI(&MIN[1].pValue, mbs_table, mbsCnt += 2);
	for (u8 i = 2; i < 41; i++)
	{
		Menu.addItem(1, &MIN[i]);
		ptrFtoI(&MIN[i].pValue, mbs_table, mbsCnt += 2);
	}

	MSY[1].config(sym_u, 1, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[2].config(sym_u, 2, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MSY[3].config(sym_u, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[4].config(sym_u, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[5].config(sym_u, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[6].config(sym_u, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[7].config(sym_u, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[8].config(sym_u, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[9].config(sym_u, 9, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[10].config(sym_u, 10, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[11].config(sym_u, 11, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[12].config(sym_u, 12, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[13].config(sym_u, 13, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[14].config(sym_u, 14, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[15].config(sym_u, 15, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[16].config(sym_u, 16, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[17].config(sym_u, 17, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[18].config(sym_u, 18, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[19].config(sym_u, 19, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[20].config(sym_u, 20, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[21].config(sym_u, 21, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[22].config(sym_u, 22, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[23].config(sym_u, 23, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[24].config(sym_u, 24, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(2, &MSY[1]);
	ptrFtoI(&MSY[1].pValue, mbs_table, mbsCnt += 2);
	for (u8 i = 2; i < 25; i++)
	{
		Menu.addItem(2, &MSY[i]);
		ptrFtoI(&MSY[i].pValue, mbs_table, mbsCnt += 2);
	}

	MDI[1].config(sym_i, 1, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[2].config(sym_i, 2, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[3].config(sym_i, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[4].config(sym_i, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[5].config(sym_i, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[6].config(sym_i, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[7].config(sym_i, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[8].config(sym_i, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(3, &MDI[1]);
	ptrFtoI(&MDI[1].pValue, mbs_table, mbsCnt += 2);
	for (u8 i = 2; i < 9; i++)
	{
		Menu.addItem(3, &MDI[i]);
		ptrFtoI(&MDI[i].pValue, mbs_table, mbsCnt += 2);
	}

	MDO[1].config(sym_o, 1, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[2].config(sym_o, 2, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[3].config(sym_o, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[4].config(sym_o, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[5].config(sym_o, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[6].config(sym_o, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[7].config(sym_o, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[8].config(sym_o, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[9].config(sym_o, 9, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[10].config(sym_o, 10, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[11].config(sym_o, 11, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[12].config(sym_o, 12, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[13].config(sym_o, 13, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[14].config(sym_o, 14, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[15].config(sym_o, 15, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[16].config(sym_o, 16, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[17].config(sym_o, 17, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[18].config(sym_o, 18, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[19].config(sym_o, 19, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[20].config(sym_o, 20, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[21].config(sym_o, 21, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[22].config(sym_o, 22, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[23].config(sym_o, 23, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[24].config(sym_o, 24, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[25].config(sym_o, 25, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[26].config(sym_o, 26, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[27].config(sym_o, 27, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[28].config(sym_o, 28, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[29].config(sym_o, 29, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[30].config(sym_o, 30, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[31].config(sym_o, 31, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[32].config(sym_o, 32, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[33].config(sym_o, 33, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[34].config(sym_o, 34, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[35].config(sym_o, 35, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[36].config(sym_o, 36, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(4, &MDO[1]);
	ptrFtoI(&MDO[1].pValue, mbs_table, mbsCnt += 2);
	for (u8 i = 2; i < 37; i++)
	{
		Menu.addItem(4, &MDO[i]);
		ptrFtoI(&MDO[i].pValue, mbs_table, mbsCnt += 2);
	}

}

void global::itSampleADC(void)
{
	oMin.Calculate();
}

void global::itCalcFreq(void)
{

}

void global::gpioInit(IO_7segment* SevenSeg, softSpi* spiFlash)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GpioSpi spiPinStruct;

	ADC_InitTypeDef ADC_InitStructure;    // ADC
	ADC_CommonInitTypeDef ADC_CommonInitStruct; // ADC

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // GPIO_A
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // GPIO_B
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // GPIO_C
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // GPIO_D
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // GPIO_E
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); // GPIO_F
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); // GPIO_G

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	spiPinStruct.csPORT = GPIOB;
	spiPinStruct.csPIN = GPIO_Pin_5;

	spiPinStruct.clkPORT = GPIOB;
	spiPinStruct.clkPIN = GPIO_Pin_4;

	spiPinStruct.siPORT = GPIOC;
	spiPinStruct.siPIN = GPIO_Pin_10;

	spiPinStruct.soPORT = GPIOC;
	spiPinStruct.soPIN = GPIO_Pin_11;

	spiFlash->init(spiPinStruct);

	SevenSeg->GpioIO.A.Port = GPIOC; //
	SevenSeg->GpioIO.B.Port = GPIOC; //
	SevenSeg->GpioIO.C.Port = GPIOC; //
	SevenSeg->GpioIO.D.Port = GPIOC; //
	SevenSeg->GpioIO.E.Port = GPIOC; //
	SevenSeg->GpioIO.F.Port = GPIOB; //
	SevenSeg->GpioIO.G.Port = GPIOB; //

	SevenSeg->GpioIO.DP.Port = GPIOC; //
	SevenSeg->GpioIO.SG.Port = GPIOB; //

	SevenSeg->GpioIO.A.Pin = GPIO_Pin_13; //
	SevenSeg->GpioIO.B.Pin = GPIO_Pin_14; //
	SevenSeg->GpioIO.C.Pin = GPIO_Pin_15; //
	SevenSeg->GpioIO.D.Pin = GPIO_Pin_4; //
	SevenSeg->GpioIO.E.Pin = GPIO_Pin_5; //
	SevenSeg->GpioIO.F.Pin = GPIO_Pin_0; //
	SevenSeg->GpioIO.G.Pin = GPIO_Pin_1; //
	SevenSeg->GpioIO.DP.Pin = GPIO_Pin_8; //
	SevenSeg->GpioIO.SG.Pin = GPIO_Pin_12; //

	SevenSeg->Digit[0].Port = GPIOB; //
	SevenSeg->Digit[1].Port = GPIOB; //
	SevenSeg->Digit[2].Port = GPIOB; //
	SevenSeg->Digit[3].Port = GPIOB; //

	SevenSeg->Digit[3].Pin = GPIO_Pin_2; //
	SevenSeg->Digit[2].Pin = GPIO_Pin_10; //
	SevenSeg->Digit[1].Pin = GPIO_Pin_11; //
	SevenSeg->Digit[0].Pin = GPIO_Pin_13; //

	SevenSeg->GpioIOConfig();

#ifdef USE_COMUNICATION_LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = COMUNICATION_LED_PIN;
	GPIO_Init(COMUNICATION_LED_PORT, &GPIO_InitStructure);
#endif

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_CommonInitStruct.ADC_Mode = ADC_DualMode_RegSimult;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);

	//ADC1
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);

	//ADC2
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_3Cycles);

	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_SoftwareStartConv(ADC1);

}

