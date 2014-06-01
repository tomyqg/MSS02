/*
 * oFrequency.cpp
 *
 *  Created on: 28 мая 2014 г.
 *      Author: temmka
 *  Description:
 */

#include "cl_FrRms.h"

FrRms::FrRms()
{
}

void FrRms::init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr,u16 *iZeroOffset)
{
	dataOk = idDataOk;
	SigOk = iSigOk;
	ADCavr = iADCavr;
	ZeroOffset = iZeroOffset;
	factor = 0.00001001;
	cntFreq = 0;
	cntAvr = 0;

}

void FrRms::calculate(bool DtU)
{
	if (*SigOk > 0)
	{

		if (DtU)
		{

			//Усреднение текущей частоты
			if (cntFreq >= cntFreqMax) //
			{
				avrFr = tAvrFr / cntFreqMax;
				tAvrFr = 0.0;
				cntFreq = 0;
			}

			//Расчет частоты
			tAvrFr = tAvrFr + (1 / ((float) cntCalc * factor));
			cntFreq++;

			//Расчет рмс
			rms = sqrtf((float) rmsSum);
			rms = rms / (float) cntCalc;

			cntCalc = 0;
			rmsSum = 0;

		}
		else
		{
			cntCalc++;

			if (*ADCavr >= *ZeroOffset)
			{
				tmp = *ADCavr - *ZeroOffset;
			}

			if (*ADCavr < *ZeroOffset)
			{
				tmp = *ZeroOffset - *ADCavr;
			}

			rmsSum += tmp * tmp;

		}

		*SigOk = *SigOk - 1;
	}
	else
	{
		avrFr = 0.0;
		rms = 0.0;
	}
	*dataOk = false;

}

void FrRms::sendToItem(MenuItem &Freq, MenuItem &Rms)
{

	Freq.pValue = avrFr;
	Rms.pValue = rms;

}

