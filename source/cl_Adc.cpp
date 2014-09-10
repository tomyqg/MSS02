/*
 * clAdc.cpp
 *
 *  Created on: 29 мая 2014 г.
 *      Author: temmka
 *  Description: 
 */

#include "cl_Adc.h"

AdcX::AdcX()
{

}

void AdcX::init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset, ADC_TypeDef* ADC_N)
{
	dataOk = idDataOk;
	SigOk = iSigOk;
	ADCavr = iADCavr;
	ZeroOffset = iZeroOffset;
	ADCx = ADC_N;
	MaxAvrCount = 25;
	i = 0;
}

void AdcX::sample()
{
	if (i >= MaxAvrCount)
	{
		//Calculate average adc value
		*ADCavr = (ADCBuff / MaxAvrCount);
		*dataOk = true;

		if (ADCx->DR > *ZeroOffset + 33 || ADCx->DR < *ZeroOffset - 33)
		{
			if (*SigOk < 15000)
			{
				*SigOk += 100;
			}
		}

		//Reset data
		i = 1;
		ADCBuff = 0;

		//Sampling data
		ADCBuff += ADCx->DR;

	}
	else //Sampling data
	{
		ADCBuff += ADCx->DR;
		i++;
	}

}

void AdcX::sendToItem(MenuItem &Adc)
{

	Adc.pValue = (float) *ADCavr;


}
