/*
 * Averaging.h
 *
 *  Created on: Sep 7, 2014
 *      Author: temmka
 *  Description: 
 */

#ifndef AVERAGING_H_
#define AVERAGING_H_

#include "stm32f4xx_conf.h"
#include <vector>

class Averaging
{
public:
	Averaging();
	void setMaxCount(u16 Value);
	float avr(float Value);


	u16 maxCount;
	u16 countValue;
	float tAvr;
	float actAvr;


};

#endif /* AVERAGING_H_ */
