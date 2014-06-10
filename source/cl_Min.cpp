/*
 * clMin.cpp
 *
 *  Created on: 29 мая 2014 г.
 *      Author: temmka
 *  Description: 
 */

#include "cl_Min.h"

cl_Min::cl_Min()
{
}

void cl_Min::init(MenuItem *tXXX)
{
	tMIN = tXXX;
	Ch1_FrRms.init(&dataOk[0], &SignalOk[0], &ADCavr[0], &ZeroOffset[0], &AC_DC[0]);
	Ch2_FrRms.init(&dataOk[1], &SignalOk[1], &ADCavr[1], &ZeroOffset[1], &AC_DC[1]);
	Ch1_Adc.init(&dataOk[0], &SignalOk[0], &ADCavr[0], &ZeroOffset[0], ADC1);
	Ch2_Adc.init(&dataOk[1], &SignalOk[1], &ADCavr[1], &ZeroOffset[1], ADC2);

	Ch1_Alarm.outGpioX = GPIOC;
	Ch1_Alarm.outGpioPin = GPIO_Pin_1;

	Ch2_Alarm.outGpioX = GPIOC;
	Ch2_Alarm.outGpioPin = GPIO_Pin_2;
	Ch1_Alarm.init();
	Ch2_Alarm.init();

}

bool cl_Min::DwnToUp(u16 Value, u8 &mCurrPos, u16 ZeroOffset)
{
	u16 PlusHyst = 15;
	u16 MinusHyst = 15;

	if (Value > ZeroOffset + PlusHyst)
	{
		if (mCurrPos == 0)
		{
			mCurrPos = 1;
			return true;
		}
		mCurrPos = 1;
	}

	if (Value < ZeroOffset - MinusHyst)
		mCurrPos = 0;

	return false;

}

void cl_Min::Calculate()
{

	Ch1_Adc.sample();
	Ch2_Adc.sample();

	if (AC_DC[0])
	{
		Ch1_FrRms.calculateDC();
	}
	else
	{
		Ch1_FrRms.calculateAC(DwnToUp(ADCavr[0], mCurPos[0], ZeroOffset[0]));
	}

	if (AC_DC[1])
	{
		Ch2_FrRms.calculateDC();
	}
	else
	{
		Ch2_FrRms.calculateAC(DwnToUp(ADCavr[1], mCurPos[1], ZeroOffset[1]));
	}

	Ch1_Alarm.calculate(mCurPos[0], ADCavr[0]);

}
