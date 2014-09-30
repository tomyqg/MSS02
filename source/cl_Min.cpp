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

	Ch1_Adc.init(&MinCom_Ch_1, ADC1);
	Ch1_FrRms.init(&MinCom_Ch_1);

	Ch2_Adc.init(&MinCom_Ch_2, ADC2);
	Ch2_FrRms.init(&MinCom_Ch_2);

	Ch1_Alarm.ledGpioX = GPIOC;
	Ch1_Alarm.ledGpioPin = GPIO_Pin_1;

	Ch1_Alarm.outGpioX = GPIOA;
	Ch1_Alarm.outGpioPin = GPIO_Pin_3;

	Ch1_Alarm.inGpioX = GPIOB;
	Ch1_Alarm.inGpioPin = GPIO_Pin_14;

	Ch1_Alarm.init(&MinCom_Ch_1);

	Ch2_Alarm.ledGpioX = GPIOC;
	Ch2_Alarm.ledGpioPin = GPIO_Pin_2;

	Ch2_Alarm.outGpioX = GPIOC;
	Ch2_Alarm.outGpioPin = GPIO_Pin_12;

	Ch2_Alarm.inGpioX = GPIOB;
	Ch2_Alarm.inGpioPin = GPIO_Pin_15;

	inGpioPort = GPIOD;
	inGpioPin = GPIO_Pin_2;

	Ch2_Alarm.init(&MinCom_Ch_2);

	cosMode=0;

}

//TODO not end

bool cl_Min::DwnToUp(MinComunication *MinCom, u8 &pos, u16 &lastVal)
{

	if (MinCom->ADCavr > lastVal + 20)
		pos = 1;
	if (MinCom->ADCavr < lastVal - 20)
		pos = 0;

	lastVal = MinCom->ADCavr;

	if (MinCom->ADCavr > (MinCom->ZeroOffset - 400) && pos == 1)
	{
		if (MinCom->mCurPos == 0)
		{
			MinCom->mCurPos = 1;
			return true;
		}
		MinCom->mCurPos = 1;
	}

	if (((MinCom->ADCavr < MinCom->ZeroOffset + 400) && (pos == 0)) || !MinCom->SignalOk)
		MinCom->mCurPos = 0;

	return false;

}

void cl_Min::Calculate()
{

	Ch1_Adc.sample();
	MinCom_Ch_1.DtU = DwnToUp(&MinCom_Ch_1, tpos[0], tLastVal[0]);
	Ch1_FrRms.calculate();
	Ch1_Alarm.calculate();

	Ch2_Adc.sample();
	MinCom_Ch_2.DtU = DwnToUp(&MinCom_Ch_2, tpos[1], tLastVal[1]);
	Ch2_FrRms.calculate();
	Ch2_Alarm.calculate();

	switch (cosMode)
	{
	case 0:
		AvrCos = 0.0f;
		break;
	case 1:
		//if 1 and 2 chanal is AC
		if (!MinCom_Ch_1.AC_DC && !MinCom_Ch_2.AC_DC && MinCom_Ch_1.SignalOk > 0 && MinCom_Ch_2.SignalOk > 0)
		{
			//Calculate COS
			if (MinCom_Ch_1.DtU)
			{
				cntCos = 1;
			}
			if (MinCom_Ch_2.DtU)
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
		break;
	case 2:
		AvrCos = 0.0f;
		break;

	default:
		AvrCos = 0.0f;
		break;

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
