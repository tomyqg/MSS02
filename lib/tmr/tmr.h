/*
 * tmr.h
 *
 *  Created on: 27 апр. 2014 г.
 *      Author: temmka
 *  Description: 
 */

#ifndef TMR_H_
#define TMR_H_

#include "stm32f4xx_conf.h"
#include "user_conf.h"





enum{
	T_ON,
	T_OFF,
};

//SYSTICK_VALUE
//SYSCLK_VALUE

class tmr {
public:


	tmr(unsigned char type);
	bool update(bool in, unsigned long set_delay);
	bool status(void);


private:

	bool run; //status of work
	bool tout; // signal
	bool out; //output signal
	unsigned char type; //0=ton, 1=tof, 2=pulse
	unsigned long counter;//internal counter
	bool p;     //one cycle detection
};



#endif /* TMR_H_ */
