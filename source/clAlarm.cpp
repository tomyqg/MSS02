/*
 * clAlarm.cpp
 *
 *  Created on: 09 θώνÿ 2014 γ.
 *      Author: temmka
 *  Description: 
 */

#include <clAlarm.h>

cl_Alarm::cl_Alarm()
{
	// TODO Auto-generated constructor stub

}

void cl_Alarm::init()
{

	SelectMode = 0; // 0 = None, 1=Alarm, 2=ZeroLine, 3=OverFull

	invOut = 0;
	setOut = 0;
	AcDc = 0;
	; // 0=AC, 1 = DC

	out = 0;

	cntAvr = 0;
	factor = 1.0;

	Value = 0.0;
	minValue = 0.0;
	maxValue = 4000;
	avrAl = 10;

}

void cl_Alarm::calculate(u8 Pos, u16 Adc)
{

	if (AcDc)
	{
		Value = ((float) Adc * factor);
	}
	else
	{
		Value = ((float) Adc * factor) / 0.7071;
	}

	switch (SelectMode)
	{
	case 1: //Hard fault

		if ((inGpioX->IDR & inGpioPin) != (uint32_t) Bit_RESET)
		{
			tOut = 1;
		}
		else
		{
			if (!setOut)
				tOut = 0;
		}

		break;

	case 2: //Zero Line
		tOut = Pos;
		break;

	case 3: //OverFull

		if (Value > maxValue || Value < minValue)
		{
			if (cntAvr >= (u16) avrAl)
			{
				tOut = 1;
			}
			else
			{
				cntAvr++;
			}

		}
		else
		{
			if (!setOut)
			{
				tOut = 0;
				cntAvr = 0;
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

	if (tOut != 0)
	{
		outGpioX->BSRRL = outGpioPin;
	}
	else
	{
		outGpioX->BSRRH = outGpioPin;
	}

}
