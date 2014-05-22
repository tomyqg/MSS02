/*
 * MenuIO.cpp
 *
 *  Created on: 22 лист. 2013
 *      Author: ANK
 */

#include "IO_7segment.h"
#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "math.h"

IO_7segment::IO_7segment(u8 maxDigit)
{
	DigitMax = maxDigit;
	Digit = new GpioLt[DigitMax];
}
;

/**
 * Init GPIO 7-segment
 */
void IO_7segment::GpioIOConfig()
{

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GpioIO.A.Pin;
	GPIO_Init(GpioIO.A.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.B.Pin;
	GPIO_Init(GpioIO.B.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.C.Pin;
	GPIO_Init(GpioIO.C.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.D.Pin;
	GPIO_Init(GpioIO.D.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.E.Pin;
	GPIO_Init(GpioIO.E.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.F.Pin;
	GPIO_Init(GpioIO.F.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.G.Pin;
	GPIO_Init(GpioIO.G.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.DP.Pin;
	GPIO_Init(GpioIO.DP.Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GpioIO.SG.Pin;
	GPIO_Init(GpioIO.SG.Port, &GPIO_InitStructure);

	for (int i = 0; i < DigitMax; i++)
	{
		GPIO_InitStructure.GPIO_Pin = Digit[i].Pin;
		GPIO_Init(Digit[i].Port, &GPIO_InitStructure);
	}

}

/**
 * Write one digit
 */
void IO_7segment::DigitWrite(u8 value, bool point, u8 CurrDigit)
{
	for (u8 i = 0; i < DigitMax; i++)
	{
		Digit[i].Port->BSRRH = Digit[i].Pin;
	}
	delayMicroseconds(5);
	(value & 0x01) ? GpioIO.A.Port->BSRRL = GpioIO.A.Pin : GpioIO.A.Port->BSRRH = GpioIO.A.Pin;
	(value & 0x02) ? GpioIO.B.Port->BSRRL = GpioIO.B.Pin : GpioIO.B.Port->BSRRH = GpioIO.B.Pin;
	(value & 0x04) ? GpioIO.C.Port->BSRRL = GpioIO.C.Pin : GpioIO.C.Port->BSRRH = GpioIO.C.Pin;
	(value & 0x08) ? GpioIO.D.Port->BSRRL = GpioIO.D.Pin : GpioIO.D.Port->BSRRH = GpioIO.D.Pin;
	(value & 0x10) ? GpioIO.E.Port->BSRRL = GpioIO.E.Pin : GpioIO.E.Port->BSRRH = GpioIO.E.Pin;
	(value & 0x20) ? GpioIO.F.Port->BSRRL = GpioIO.F.Pin : GpioIO.F.Port->BSRRH = GpioIO.F.Pin;
	(value & 0x40) ? GpioIO.G.Port->BSRRL = GpioIO.G.Pin : GpioIO.G.Port->BSRRH = GpioIO.G.Pin;
	(point) ? GpioIO.DP.Port->BSRRL = GpioIO.DP.Pin : GpioIO.DP.Port->BSRRH = GpioIO.DP.Pin;

	Digit[CurrDigit].Port->BSRRL = Digit[CurrDigit].Pin;
	delayMicroseconds(15);
}

/**
 * Write u16 - value
 */
void IO_7segment::write(u16 value)
{
	u8 x[DigitMax];
	u32 factor = 1;

	for (u8 i = 0; i < DigitMax; i++)
	{
		x[i] = (value / factor) % 10;
		factor *= 10;
		DigitWrite(num[x[i]], false, i);
	}

	GpioIO.SG.Port->BSRRH = GpioIO.SG.Pin;

}

/**
 * Write s16 - value
 */
void IO_7segment::write(s16 value)
{
	if (value < 0)
	{
		GpioIO.SG.Port->BSRRL = GpioIO.SG.Pin;
		value *= -1;
	}
	else
	{
		GpioIO.SG.Port->BSRRH = GpioIO.SG.Pin;
	}

	write((u16) value);

}

/**
 * Write  symbol and digit
 */
void IO_7segment::write(u8 letter, u16 value)
{

	u8 x[DigitMax];
	u32 factor = 1;

	for (u8 i = 0; i < DigitMax - 2; i++)
	{
		x[i] = (value / factor) % 10;
		factor *= 10;
		DigitWrite(num[x[i]], false, i);
	}

	DigitWrite(sym_neg, false, DigitMax - 2); //display "-"
	DigitWrite(letter, false, DigitMax - 1);

	GpioIO.SG.Port->BSRRH = GpioIO.SG.Pin;

}

/**
 * Write  float value
 */
void IO_7segment::write(float value)
{

	u8 x1, x10, x100, x1000;
	bool point[4]
	{ false, false, false, false };
	float x;
	s16 icel;
	bool rZero;

	//Индикатор знак минус
	if (value < 0)
	{
		GpioIO.SG.Port->BSRRL = GpioIO.SG.Pin;
		value *= -1;
	}
	else
	{
		GpioIO.SG.Port->BSRRH = GpioIO.SG.Pin;
	}

	icel = (s16) (value);
	if ((value - (float) icel) == 0.0)
	{
		rZero = true;
	}
	else
	{
		rZero = false;
	}

	if (icel >= 1000) //Тысячи
	{
		point[0] = false;
		x = 1;
	}
	if (icel >= 100 && icel < 1000) //Сотни
	{
		if (rZero)
		{
			point[1] = false;
			x = 1;
		}
		else
		{
			point[1] = true;
			x = 10;
		}
	}
	if (icel >= 10 && icel < 100) //Десятки
	{
		if (rZero)
		{
			point[2] = false;
			x = 1;
		}
		else
		{
			point[2] = true;
			x = 100;
		}

	}
	if (icel < 10) //Единицы
	{

		if (rZero)
		{
			point[3] = false;
			x = 1;
		}
		else
		{
			point[3] = true;
			x = 1000;
		}
	}
	value *= x; //change it

	icel = (s16) (value);

	x1000 = icel / 1000;
	x100 = (icel / 100) % 10;
	x10 = (icel / 10) % 10;
	x1 = (icel % 10);

	if (x1000 == 0)
	{
		DigitWrite(num[10], point[3], 3);
	}
	else
	{
		DigitWrite(num[x1000], point[3], 3);
	}

	if (x1000 == 0 && x100==0)
	{
		DigitWrite(num[10], point[3], 3);
	}
	else
	{
		DigitWrite(num[x100], point[2], 2);
	}


	if (x1000 == 0 && x100==0 && x10 == 0)
	{
		DigitWrite(num[10], point[3], 3);
	}
	else
	{
		DigitWrite(num[x10], point[1], 1);
	}

DigitWrite(num[x1], point[0], 0);

}

