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
	maxCount = 100;
	countValue = 0;
	arr = 0;
	u16 Valid=0;
}

void Averaging::setMaxCount(u16 Value)
{
	maxCount = Value;
	arr = new float[Value]; // выделить память под стек
}

float Averaging::avr(float Value)
{
	float tAvr = 0.f;
	if (countValue >= maxCount - 1)
	{

		for (u16 i = maxCount - 1; i > 0; i--)
		{
			tAvr += arr[i];
			arr[i] = arr[i - 1];

		}
		arr[0] = Value;

		if (Valid > maxCount)
		{
			return tAvr / (maxCount - 1);\

		}
		else
		{
			Valid++;
			return 0.0f;
		}

	}
	else
	{
		arr[countValue] = maxCount;
		countValue++;
		return 0.0f;
	}
}
