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
	T_PULSE
};

//SYSTICK_VALUE
//SYSCLK_VALUE

class tmr {
public:
	bool in;

	tmr(u8 type, u32 delayTime);
	void start(void);
	void stop(void);
	void reset(void);
	void update(void);
	bool status(void);


private:

	bool run; //status of work
	bool out; //output signal
	u8 type; //0=ton, 1=tof, 2=pulse
	u32 cmpVal; //setpoint value
	u32 counter;//internak counter



	bool p;     //one cycle detection
};



#endif /* TMR_H_ */
