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

void FrRms::init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset, u8 *iAcDc)
{
	dataOk = idDataOk;
	SigOk = iSigOk;
	ADCavr = iADCavr;
	ZeroOffset = iZeroOffset;
	factor = 0.00001001;
	cntFreq = 0;
	cntAvr = 0;
	cntFreq = 0;
	RmsFactor = 0.0;
	AcDc = iAcDc;
}

void FrRms::calculateAC(bool DtU)
{
	if (*SigOk > 0)
	{

		if (DtU)
		{

			//Расчет частоты
			tAvrFr = tAvrFr + (1 / ((float) cntCalc * factor));
			cntFreq++;

			//Усреднение текущей частоты
			if (cntFreq >= cntFreqMax) //
			{
				if (cntEnOutValue > FilterOutValue)
					avrFr = tAvrFr / cntFreqMax;

				tAvrFr = 0.0;
				cntFreq = 0;
			}

			//Расчет рмс
			tRms = sqrtf((((float) rmsSum) *2) / ((float) cntCalc));
			tRms *= RmsFactor;
			cntRms++;
			tAvrRms += tRms;

			if (cntRms >= cntRmsMax) //
			{
				if (cntEnOutValue > FilterOutValue)
					avrRms = tAvrRms / cntRmsMax;

				tAvrRms = 0.0;
				cntRms = 0;
			}

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

			rmsSum += (tmp * tmp)/2;

		}

		*SigOk = *SigOk - 1;
	}
	else
	{
		avrFr = 0.0;
		avrRms = 0.0;
		tRms = 0.0;
		cntEnOutValue = 0;
	}
	*dataOk = false;

	if (cntEnOutValue <= FilterOutValue)
		cntEnOutValue++;

}

void FrRms::calculateDC()
{
	if (*SigOk > 0)
	{

		if (DcCounter > 1999)
		{

			//Расчет рмс
			tRms = sqrtf(((float) rmsSum) / ((float) cntCalc));
			tRms *= RmsFactor * 1.414;
			cntRms++;
			tAvrRms += tRms;

			if (cntRms >= cntRmsMax) //
			{
				if (cntEnOutValue > FilterOutValue)
					avrRms = tAvrRms / cntRmsMax;

				tAvrRms = 0.0;
				cntRms = 0;
			}
			tAvrFr = 0.0;
			cntFreq = 0;
			cntCalc = 0;
			rmsSum = 0;
			DcCounter = 1;

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
			DcCounter++;

		}

		*SigOk = *SigOk - 1;
	}
	else
	{
		avrFr = 0.0;
		avrRms = 0.0;
		tRms = 0.0;
		cntEnOutValue = 0;
	}
	*dataOk = false;

	if (cntEnOutValue <= FilterOutValue)
		cntEnOutValue++;

}

void FrRms::sendToItem(MenuItem &iFreq, MenuItem &iRms)
{

	iFreq.pValue = avrFr;
	iRms.pValue = avrRms;

}

