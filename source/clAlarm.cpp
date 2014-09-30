/*
 * clAlarm.cpp
 *
 *  Created on: 09 θών 2014 γ.
 *      Author: temmka
 *  Description: 
 */

#include "clAlarm.h"

const float sqrt2 = 0.7071f;

cl_Alarm::cl_Alarm()
{
	// TODO Auto-generated constructor stub

}

void cl_Alarm::init(MinComunication *iMinCom)
{
	MinCom = iMinCom;
	SelectMode = 0; // 0 = None, 1=Alarm, 2=ZeroLine, 3=OverFull, 4=Protocol

	invOut = 0;
	setOut = 0;

	out = 0;
	tmpOut = 0;

	cntAvrage = 0;

	minValue = 0.0;
	maxValue = 4000;
	avrAl = 10;

	cmpVal = 0; //setpoint value
	p = 1;     //one cycle var

}

void cl_Alarm::calculate()
{

	switch (SelectMode)
	{
	case 1: //Hard fault

		if ((inGpioX->IDR & inGpioPin) != (uint32_t) Bit_RESET)
		{
			if (!setOut)
				tmpOut = 0;
		}
		else
		{
			tmpOut = 1;
		}

		break;

	case 2: //Zero Line
		if (MinCom->SignalOk)
		{
			tmpOut = MinCom->mCurPos;
		}
		else
		{
			tmpOut = 0;
		}
		break;

	case 3: //OverFull

		if (cntUpdOver >= 100)
		{
			if (MinCom->AC_DC)
			{
				s16 tAbsAdc;
				tAbsAdc = (s16) MinCom->AbsAdc;
				if (MinCom->ADCavr < MinCom->ZeroOffset)
					tAbsAdc = tAbsAdc * (-1);
				if (MinCom->SignalOk > 0)
				{
					dcVal = (float) tAbsAdc * MinCom->RmsFactor; //TODO very load proc if usr sqrt
				}
				else
				{
					dcVal = 0.0f;
				}
				if ((dcVal > maxValue) || (dcVal < minValue))
				{
					if (cntAvrage >= ((u16) avrAl))
					{
						tmpOut = true;
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
						tmpOut = 0;
						cntAvrage = 0;
					}

				}

			}
			else
			{
				acVal = (float) MinCom->AbsAdc * MinCom->RmsFactor / 1.41f; // /17.15f;

				if ((acVal > maxValue) || (MinCom->Rms > maxValue) || (MinCom->Rms < minValue))
				{
					if (cntAvrage >= (u16) avrAl)
					{
						tmpOut = true;
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
						tmpOut = 0;
						cntAvrage = 0;
					}

				}

			}
			cntUpdOver = 0;

		}
		cntUpdOver++;
		break;

	case 4: //Soft output

		break;

	default:

		break;
	}

	if (invOut)
	{
		tmpOut = !tmpOut;
	}

	if (SelectMode == 2)
	{
		out = tmpOut;
	}
	else
	{
		out = tof(tmpOut, 5, p, cmpVal);
		//out = tmpOut;
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
