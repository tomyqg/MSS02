/*
 * clMin.h
 *
 *  Created on: 29 мая 2014 г.
 *      Author: temmka
 *  Description: 
 */

#ifndef CL_MIN_H_
#define CL_MIN_H_

#include "stm32f4xx_conf.h"
#include "user_conf.h"
/*
 *@autor
 */
class cl_Min
{
public:
	cl_Min();
    void init(MenuItem *tXXX);
	bool DwnToUp(u16 Value, u8 &mCurrPos, u16 ZeroOffset);
	void Calculate();

	u8 mCurPos[2];
	bool DtU[2];

	MenuItem *tMIN;

	u16 ZeroOffset[2];
	u16 SignalOk[2];

	uint16_t ADCavr[2];
	bool dataOk[2];

	AdcX Ch1_Adc;
	AdcX Ch2_Adc;

	FrRms Ch1_FrRms;
	FrRms Ch2_FrRms;



};

#endif /* CLMIN_H_ */
