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
	maxCount = 0;
	countValue = 0;
	//arr = 0;
	//vArr.resize(10);

}

void Averaging::setMaxCount(u16 Value)
{

	if ((Value != maxCount) && (Value > 0))
	{
		countValue = maxCount;
		maxCount = Value;
		vArr.resize(maxCount);
	}
	else
	{

	}

	//arr = new float[Value]; // выделить память под стек
}

float Averaging::avr(float Value)
{
	float tAvr = 0.f;
	if (maxCount)
	{
		if (countValue >= maxCount - 1)
		{

			for (u16 i = maxCount - 1; i > 0; i--)
			{
				tAvr += vArr[i];
				vArr[i] = vArr[i - 1];

			}
			vArr[0] = Value;

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
			vArr[countValue] = maxCount;
			countValue++;
			return 0.0f;
		}
	}
	return 0.0f;
}
