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
 *@autor static
 */
class cl_Min
{
public:
	cl_Min();
	void init(MenuItem *tXXX);
	inline bool DwnToUp(u16 Value, u8 &mCurrPos, u16 ZeroOffset, u16 iSignalOk, u8 &pos, u16 &lastVal);
	void Calculate();

	u8 mCurPos[2];
	Averaging AvCos;

	bool SignaType[2]; //0=DC, 1 = DC;

	MenuItem *tMIN;

	u16 ZeroOffset[2];
	u16 SignalOk[2];
	u16 ADCavr[2];
	bool dataOk[2];
	bool DtU[2]; //Zero-crossing
	u8 AC_DC[2];

	AdcX Ch1_Adc;
	AdcX Ch2_Adc;

	FrRms Ch1_FrRms;
	FrRms Ch2_FrRms;

	cl_Alarm Ch1_Alarm;
	cl_Alarm Ch2_Alarm;

	GPIO_TypeDef *inGpioPort;
	uint16_t inGpioPin;
	bool inputValue;
	bool inputReverse;

u16 tLastVal[2];
u8 tpos[2];


	u16 cntCos;
	float cos;
	float tcos;
	float AvrCos;


};

#endif /* CLMIN_H_ */
