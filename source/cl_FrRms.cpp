/*
 * oFrequency.cpp
 *
 *  Created on: 28 мая 2014 г.
 *      Author: temmka
 *  Description:
 */

#include "cl_FrRms.h"
#include <Averaging.h>

// Calc absolute value of the ADC
u16 FrRms::AbsValue(u16 iValue, u16 iZeroOffset, bool iDC = 0)
{
	if (iValue >= iZeroOffset)
	{
		return iValue - iZeroOffset;
	}

	if (iValue < iZeroOffset)
	{
		if (iDC)
		{
			return (iZeroOffset - iValue);
		}
		else
		{
			return iZeroOffset - iValue;
		}
	}
	return 0;
}

FrRms::FrRms()
{

}

void FrRms::init(MinComunication *iMinCom)
{
	MinCom = iMinCom;
	factor = 0.00002032;
	//factor = 0.00001025;


}

void FrRms::calculateAC()
{
	//Amplitude of signal above the minimum
	if (MinCom->SignalOk > 0)
	{
		//Zero-crossing
		if (MinCom->DtU)
		{
			//Calculate frequency
			Freq = (1 / ((float) tCntPerPeriod * factor));

			//Averaging of the current frequency
			avrFreq = AvFreq.avr(Freq);
			//avrFreq = Freq;
			//Calculate RMS
			Rms = (sqrtf((((float) sumRms) * 2) / ((float) tCntPerPeriod))) * MinCom->RmsFactor;

			//Averaging of the RMS
			avrRms = AvRms.avr(Rms);

			cntPerPeriod = tCntPerPeriod;
			tCntPerPeriod = 0;
			sumRms = 0;

		}
		else
		{
			// Calc absolute value of the ADC
			MinCom->AbsAdc = AbsValue(MinCom->ADCavr, MinCom->ZeroOffset);

			sumRms += (MinCom->AbsAdc * MinCom->AbsAdc) / 2;
			tCntPerPeriod++;
		}

		MinCom->SignalOk = MinCom->SignalOk - 1;
	}
	else
	{
		avrFreq = 0.0f;
		avrRms = 0.0f;
		Rms = 0.0f;
		Freq = 0.0f;

	}
	MinCom->Rms = avrRms;
	MinCom->dataOk = false;

}

void FrRms::calculateDC()
{
	//Amplitude of signal above the minimum
	if (MinCom->SignalOk > 0)
	{
		if (DcCounter >= 2000)
		{
			//Calculate RMS
			Rms = sqrtf((((float) sumRms) * 2) / ((float) tCntPerPeriod));
			Rms *= MinCom->RmsFactor;

			if (MinCom->ADCavr < MinCom->ZeroOffset)
				Rms *= -1.0f;

			avrRms = AvRms.avr(Rms);

			avrFreq = 0.0f;
			tCntPerPeriod = 0;
			sumRms = 0;
			DcCounter = 1;

		}
		else
		{
			tCntPerPeriod++;

			// Calc absolute value of the ADC
			MinCom->AbsAdc = AbsValue(MinCom->ADCavr, MinCom->ZeroOffset, true);
			sumRms += (MinCom->AbsAdc * MinCom->AbsAdc) / 2;

			DcCounter++;
		}

		MinCom->SignalOk = MinCom->SignalOk - 1;
		; //decrease SigOk
	}
	else
	{
		avrFreq = 0.0;
		avrRms = 0.0;
		Rms = 0.0;
	}
	MinCom->Rms = avrRms;
	MinCom->dataOk = false;
}

void FrRms::calculate()
{
	if (MinCom->AC_DC)
	{
		calculateDC();
	}
	else
	{
		calculateAC();
	}

}

void FrRms::sendToItem(MenuItem &iFreq, MenuItem &iRms)
{

	iFreq.pValue = avrFreq;
	iRms.pValue = avrRms;

}

