/*
 * tmr.cpp
 *
 *  Created on: 27 апр. 2014 г.
 *      Author: temmka
 *  Description: 
 */

#include "tmr.h"

tmr::tmr(u8 tim_type, u32 set_delay)
{
	type = tim_type;
	cmpVal = set_delay;

	//SYSCLK_VALUE / SYSTICK_VALUE

}

void tmr::start(void)
{
	run = true;
}

void tmr::stop(void)
{
	run = false;
}

void tmr::reset(void)
{
	counter = 0;
}

void tmr::update(void)
{
	switch (type)
	{
	case T_ON:

		if (in) //input is active
		{
			if (!p) //firs cycle detect
			{
				counter = 0;
				p = true;
			}

			if (counter > cmpVal) //set output after overflow
			{
				out = true;

			}
			else //reset data
			{
				out = false;
				counter++;
			}
		}
		else
		{
			p = false;
			out = false;
		}
		break;

	case T_OFF:

		if (in) //input is active
		{
			p = false;
			out = true;

		}
		else //input deactivate
		{
			if (!p) //firs cycle detect deactivate
			{
				counter = 0;
				p = true;
			}

			if (counter > cmpVal) //reset output after overflow
			{
				out = false;
			}
			else //reset data
			{
				counter++;
			}

		}
		break;
	}



}



bool tmr::status(void)
{
	return out;
}
