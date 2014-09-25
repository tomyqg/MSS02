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
u16 FrRms::AbsValue(u16 iValue, u16 iZeroOffset, bool iDC=0)
{
	if (iValue >= iZeroOffset)
	{
		return iValue - iZeroOffset;
	}

	if (iValue < iZeroOffset)
	{
		if (iDC)
		{
			return (iZeroOffset - iValue) ;
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

void FrRms::init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset)
{
	dataOk = idDataOk;
	SigOk = iSigOk;
	ADCavr = iADCavr;
	ZeroOffset = iZeroOffset;
	factor = 0.0000100600;
	RmsFactor = 0.0;
	AbsAdc=0;


}

void FrRms::calculateAC(bool DtU)
{
	//Amplitude of signal above the minimum
	if (*SigOk > 0)
	{
		//Zero-crossing
		if (DtU)
		{
			//Calculate frequency
			Freq = (1 / ((float) tCntPerPeriod * factor));

			//Averaging of the current frequency
			avrFreq = AvFreq.avr(Freq);

			//Calculate RMS
			Rms = (sqrtf((((float) sumRms) * 2) / ((float) tCntPerPeriod))) * RmsFactor;

			//Averaging of the RMS
			avrRms = AvRms.avr(Rms);

			cntPerPeriod = tCntPerPeriod;
			tCntPerPeriod = 0;
			sumRms = 0;

		}
		else
		{
			// Calc absolute value of the ADC
			AbsAdc = AbsValue(*ADCavr, *ZeroOffset);

			sumRms += (AbsAdc * AbsAdc) / 2;
			tCntPerPeriod++;
		}

		*SigOk = *SigOk - 1;
	}
	else
	{
		avrFreq = 0.0f;
		avrRms = 0.0f;
		Rms = 0.0f;
		Freq = 0.0f;


	}

	*dataOk = false;

}

void FrRms::calculateDC()
{
	//Amplitude of signal above the minimum
	if (*SigOk > 0)
	{
		if (DcCounter >= 2000)
		{
			//Calculate RMS
			Rms = sqrtf((((float) sumRms) * 2) / ((float) tCntPerPeriod));
			Rms *= RmsFactor * 1.414;

			if (*ADCavr < *ZeroOffset)
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
			AbsAdc = AbsValue(*ADCavr, *ZeroOffset,true);
			sumRms += (AbsAdc * AbsAdc) / 2;

			DcCounter++;
		}

		*SigOk = *SigOk-1;; //decrease SigOk
	}
	else
	{
		avrFreq = 0.0;
		avrRms = 0.0;
		Rms = 0.0;
	}

	*dataOk = false;
}


void FrRms::calculate(){


}


void FrRms::sendToItem(MenuItem &iFreq, MenuItem &iRms)
{

	iFreq.pValue = avrFreq;
	iRms.pValue = avrRms;

}

