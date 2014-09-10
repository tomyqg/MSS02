/*
 * global.h
 *
 *  Created on: Apr 10, 2014
 *      Author: TEMMKA
 *  Description: 
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "cl_Min.h"
static const float swVersion = 24;
class global
{
public:
	global();
	void init(void);
	void cycle(void);

	void ptrFtoI(float *value, u16 **buff, u16 number);
	void usrMenuBuild(void);
	void itSampleADC(void);
	void itCalcFreq(void);

	void gpioInit(IO_7segment* SevenSeg, softSpi* spiFlash);
	void initModbusUsart(void);
	void initModbusTimer(void);
	u32 UART_SPEED;
	u8 UART_ADDR;
	u8 mCurPos;

u16 cycle_cnt;

	MenuManager Menu;
	cl_Min oMin;

	void changeVisibleItem(void);
	MenuItem RT[1]; //IO item
	MenuItem SYS[9]; //System item
	MenuItem MIN[41]; //MIN01
	MenuItem MSY[25]; //MSY02
	MenuItem MDI[9]; //MDI02
	MenuItem MDO[37]; //MDO02

	modbusSlave mbs_Slave; //object mbSlave
	u16 *mbs_table[BUFF_SIZE]; //modbus table










	u8 profibus_out_table[OUTPUT_DATA_SIZE];

	u8 profibus_in_table[INPUT_DATA_SIZE];

};

#endif /* GLOBAL_H_ */
