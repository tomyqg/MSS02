/*
 * clAlarm.cpp
 *
 *  Created on: 09 θών 2014 γ.
 *      Author: temmka
 *  Description: 
 */

#include <clAlarm.h>

const float sqrt2 = 0.7071f;

cl_Alarm::cl_Alarm()
{
	// TODO Auto-generated constructor stub

}

void cl_Alarm::init()
{

	SelectMode = 0; // 0 = None, 1=Alarm, 2=ZeroLine, 3=OverFull

	invOut = 0;
	setOut = 0;
	AcDc = 0; // 0=AC, 1 = DC

	out = 0;

	cntAvrage = 0;
	factor = 1.0;

	minValue = 0.0;
	maxValue = 4000;
	avrAl = 10;

}

void cl_Alarm::calculate(u8 Pos, u16 Adc)
{

	switch (SelectMode)
	{
	case 1: //Hard fault

		if ((inGpioX->IDR & inGpioPin) != (uint32_t) Bit_RESET)
		{
			if (!setOut)
				tOut = 0;
		}
		else
		{
			tOut = 1;
		}

		break;

	case 2: //Zero Line
		tOut = Pos;
		break;

	case 3: //OverFull

		if (AcDc)
		{
			dcVal = ((float) Adc);

			if ((dcVal > maxValue) || (dcVal < minValue))
			{
				if (cntAvrage >= ((u16) avrAl))
				{
					tOut = 1;
				}
				else
				{
					++cntAvrage;
				}
			}
			else
			{
				if (!setOut)
				{
					tOut = 0;
					cntAvrage = 0;
				}

			}

		}
		else
		{
			acVal = (float) Adc * sqrt2;

			if ((acVal > maxValue) || (acVal < minValue))
			{
				if (cntAvrage >= (u16) avrAl)
				{
					tOut = 1;
				}
				else
				{
					cntAvrage++;
				}
			}
			else
			{
				if (!setOut)
				{
					tOut = 0;
					cntAvrage = 0;
				}
//
			}

		}

		break;

	default:

		break;
	}

	if (invOut)
	{
		tOut = !tOut;
	}

	if (SelectMode == 2)
	{
		out = tOut;
	}
	else
	{
		out = tof(tOut, 5);
	}




	if (out)
	{
		ledGpioX->BSRRL = ledGpioPin;
		outGpioX->BSRRL = outGpioPin;
	}
	else
	{
		ledGpioX->BSRRH = ledGpioPin;
		outGpioX->BSRRH = outGpioPin;
	}

}
