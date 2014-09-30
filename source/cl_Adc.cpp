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

void AdcX::init(MinComunication *iMinCom, ADC_TypeDef* ADC_N)
    {
    MinCom = iMinCom;
    ADCx = ADC_N;
    MaxAvrCount = 25;
    i = 0;
    flr = new Filter(100);
    }

void AdcX::sample()
    {
//	if (i >= MaxAvrCount)
//	{
//		//Calculate average adc value
//		MinCom->ADCavr = (ADCBuff / MaxAvrCount);

    MinCom->ADCavr = flr->fl(ADCx->DR);
    MinCom->dataOk = true;

    if ((ADCx->DR > (MinCom->ZeroOffset + 60))
	    || (ADCx->DR < (MinCom->ZeroOffset - 60)))
	{
	if (MinCom->SignalOk < 15000)
	    {
	    MinCom->SignalOk += 100;
	    }
	}

//		//Reset data
//		i = 1;
//		ADCBuff = 0;
//
//		//Sampling data
//		ADCBuff += ADCx->DR;

//	}
//	else //Sampling data
//	{
//		ADCBuff += ADCx->DR;
//		i++;
//	}

    }

void AdcX::sendToItem(MenuItem &Adc)
    {

    Adc.pValue = (float) MinCom->ADCavr;

    }
