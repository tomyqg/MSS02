/*
 * clAlarm.h
 *
 *  Created on: 09 θώνÿ 2014 γ.
 *      Author: temmka
 *  Description: 
 */

#ifndef CLALARM_H_
#define CLALARM_H_


#include "stm32f4xx_conf.h"
#include "user_conf.h"
/*
 *@autor
 */
class cl_Alarm
{
public:
	cl_Alarm();
	void init();
	void calculate(u8 Pos, u16 Adc);
    GPIO_TypeDef *outGpioX;
    uint16_t outGpioPin;
    GPIO_TypeDef *inGpioX;
    uint16_t inGpioPin;
	u8 SelectMode; // 0 = None, 1=Alarm, 2=ZeroLine, 3=OverFull

	//static const float sqrt2;

	u8 invOut;
	u8 setOut;
	u8 AcDc; // 0=AC, 1 = DC
	u8 tOut = 0;
	float out;

	u16 cntAvrage;
	float factor;
	float dcVal; //Calculated value
	float acVal; //Calculated value

	float minValue;
	float maxValue;
	float avrAl;

};

#endif /* CLALARM_H_ */
