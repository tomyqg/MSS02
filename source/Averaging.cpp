/*
 * Averaging.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: temmka
 *  Description: 
 */

#include <Averaging.h>

Averaging::Averaging()
    {
    maxCount = 1;
    countValue = 0;
    tAvr=0.0f;
    actAvr=0.0f;
    }

void Averaging::setMaxCount(u16 Value)
    {

    if ((Value != maxCount) && (Value > 0))
	{
	countValue = 0;
	maxCount = Value;

	}

    }

float Averaging::avr(float Value)
    {

    if (countValue >= maxCount)
	{
	countValue=0;
	actAvr = tAvr / (float) maxCount;
	tAvr = Value;
	}
    else
	{
	tAvr += Value;
	}
    countValue++;

    return actAvr;
    }
