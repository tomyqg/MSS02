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
	Ch1_FrRms.init(&dataOk[0], &SignalOk[0], &ADCavr[0], &ZeroOffset[0]);
	Ch2_FrRms.init(&dataOk[1], &SignalOk[1], &ADCavr[1], &ZeroOffset[1]);
	Ch1_Adc.init(&dataOk[0], &SignalOk[0], &ADCavr[0], &ZeroOffset[0], ADC1);
	Ch2_Adc.init(&dataOk[1], &SignalOk[1], &ADCavr[1], &ZeroOffset[1], ADC2);

	Ch1_Alarm.outGpioX = GPIOC;
	Ch1_Alarm.outGpioPin = GPIO_Pin_1;

	Ch1_Alarm.inGpioX = GPIOB;
	Ch1_Alarm.inGpioPin = GPIO_Pin_14;

	Ch1_Alarm.init();

	Ch2_Alarm.outGpioX = GPIOC;
	Ch2_Alarm.outGpioPin = GPIO_Pin_2;

	Ch2_Alarm.inGpioX = GPIOB;
	Ch2_Alarm.inGpioPin = GPIO_Pin_15;

	inGpioPort = GPIOD;
	inGpioPin = GPIO_Pin_2;

	Ch2_Alarm.init();

}

bool cl_Min::DwnToUp(u16 Value, u8 &mCurrPos, u16 ZeroOffset, u16 iSignalOk)
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

	if ((Value < ZeroOffset - MinusHyst) || !iSignalOk)
		mCurrPos = 0;

	return false;

}

void cl_Min::Calculate()
{

	Ch1_Adc.sample();
	DtU[0] = DwnToUp(ADCavr[0], mCurPos[0], ZeroOffset[0], SignalOk[0]);

	//Select type of current channel 1
	if (AC_DC[0])
	{
		Ch1_FrRms.calculateDC();
	}
	else
	{
		Ch1_FrRms.calculateAC(DtU[0]);
	}

	Ch1_Alarm.calculate(mCurPos[0], Ch1_FrRms.AbsAdc);

	Ch2_Adc.sample();
	DtU[1] = DwnToUp(ADCavr[1], mCurPos[1], ZeroOffset[1], SignalOk[1]);

	//Select type of current channel 2
	if (AC_DC[1])
	{
		Ch2_FrRms.calculateDC();
	}
	else
	{
		Ch2_FrRms.calculateAC(DtU[1]);
	}

	Ch2_Alarm.calculate(mCurPos[1], Ch2_FrRms.AbsAdc);

	//if 1 and 2 chanal is AC
	if (!AC_DC[0] && !AC_DC[1])
	{
		//Calculate COS
		if (DtU[0])
		{
			cntCos = 1;
		}
		if (DtU[1])
		{

			if (cntCos > Ch1_FrRms.cntPerPeriod)
				cntCos = Ch1_FrRms.cntPerPeriod - cntCos;
			if (Ch1_FrRms.cntPerPeriod)
			{
				if (cntCos >= (Ch1_FrRms.cntPerPeriod / 2))
				{
					tcos = (float) cntCos / ((float) Ch1_FrRms.cntPerPeriod / 2.0f);
					tcos = tcos - 1;
					tcos *= -1.0f;

				}
				else
				{
					tcos = (float) cntCos / ((float) Ch1_FrRms.cntPerPeriod / 2.0f);
					tcos = (1.0f - tcos);
				}

				if (tcos <= 1.0f && tcos >= -0.999f)
					cos = tcos;

				cntCos++;
			}
			AvrCos = AvCos.avr(cos);
		}

		cntCos++;
	}
	else
	{
		AvrCos = 0.0f;
	}

	if ((inGpioPort->IDR & inGpioPin) != 0)
	{
		inputValue = (uint8_t) Bit_SET;
	}
	else
	{
		inputValue = (uint8_t) Bit_RESET;
	}

	if (!inputReverse)
		inputValue = !inputValue;

}
