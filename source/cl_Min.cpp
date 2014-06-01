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
	Ch1_FrRms.init(&dataOk[0], &SignalOk[0], &ADCavr[0],&ZeroOffset[0]);
	Ch2_FrRms.init(&dataOk[1], &SignalOk[1], &ADCavr[1],&ZeroOffset[1]);
	Ch1_Adc.init(&dataOk[0], &SignalOk[0], &ADCavr[0],&ZeroOffset[0], ADC1);
	Ch2_Adc.init(&dataOk[1], &SignalOk[1], &ADCavr[1],&ZeroOffset[1], ADC2);


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



	DtU[0] = DwnToUp(ADCavr[0], mCurPos[0], ZeroOffset[0]);
	DtU[1] = DwnToUp(ADCavr[1], mCurPos[1], ZeroOffset[1]);



	Ch1_FrRms.calculate(DtU[0]);
	Ch2_FrRms.calculate(DtU[1]);


}
