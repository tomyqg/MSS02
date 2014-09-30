/*
 * clMin.h
 *
 *  Created on: 29 мая 2014 г.
 *      Author: temmka
 *  Description: 
 */

#ifndef CL_MIN_H_
#define CL_MIN_H_

#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "Filter.h"
/*
 *@autor static
 */



class cl_Min
{
public:
	cl_Min();
	void init(MenuItem *tXXX);
	bool DwnToUp(MinComunication *MinCom, u8 &pos, u16 &lastVal);
	void Calculate();


	MinComunication MinCom_Ch_1;
	MinComunication MinCom_Ch_2;




	Averaging AvCos;


	MenuItem *tMIN;
	 Filter *fr;
u16 *arrx;






	AdcX Ch1_Adc;
	AdcX Ch2_Adc;

	FrRms Ch1_FrRms;
	FrRms Ch2_FrRms;

	cl_Alarm Ch1_Alarm;
	cl_Alarm Ch2_Alarm;

	GPIO_TypeDef *inGpioPort;
	uint16_t inGpioPin;

	bool inputValue;
	bool inputReverse;

u16 tLastVal[2];
u8 tpos[2];


	u16 cntCos;
	float cos;
	float tcos;
	float AvrCos;
	u8 cosMode; //0=Off, 1=cosinus, 2=sync


};

#endif /* CLMIN_H_ */
