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
	bool DwnToUp(u16 Value);
	void gpioInit(IO_7segment* SevenSeg, softSpi* spiFlash);











	MenuManager Menu;
	MenuItem RT[2]; //IO item
	MenuItem SYS[8]; //System item
	MenuItem MIN[46]; //MIN01
	MenuItem MSY[25]; //MSY02
	MenuItem MDI[9]; //MDI02
	MenuItem MDO[37]; //MDO02

	modbusSlave mbs_Slave; //object mbSlave
	u16 *mbs_table[BUFF_SIZE]; //modbus table

	__IO uint16_t aADCDualConvertedValue[4]; //
	uint16_t aADCavr[4];
	uint32_t aADCBuff[4];
	u16 adcAvrCnt = 0; // Averaging count
	float rms;

	u8 profibus_out_table[OUTPUT_DATA_SIZE];

	u8 profibus_in_table[INPUT_DATA_SIZE];

};

#endif /* GLOBAL_H_ */
