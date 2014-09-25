/*
 * tmr.cpp
 *
 *  Created on: 27 апр. 2014 г.
 *      Author: temmka
 *  Description: 
 */

#include "tmr.h"

tmr::tmr(unsigned char tim_type)
{
	type = tim_type;

	p = 0;
	tout = 0;
	out = 0;
	counter = 0;
    run=0;
}



bool tmr::update(bool in, unsigned long time_value)
{
	run = in;


	switch (type)
	{
	case T_ON:

		if (run) //input is active
		{
			if (!p) //firs cycle detect
			{
				counter = 0;
				p = true;
			}

			if (counter >= time_value) //set output after overflow
			{
				tout = true;

			} else //reset data
			{
				tout = false;
				counter++;
			}
		} else
		{
			p = false;
			tout = false;
		}
		break;

	case T_OFF:

		if (run) //input is active
		{
			p = false;
			tout = true;

		} else //input deactivate
		{
			if (!p) //firs cycle detect deactivate
			{
				counter = 0;
				p = true;
			}

			if (counter >= time_value) //reset output after overflow
			{
				tout = false;
			} else //reset data
			{
				counter++;
			}

		}
		break;
	}
out = tout;
	return out;

}

bool tmr::status(void)
{
	return out;
}
