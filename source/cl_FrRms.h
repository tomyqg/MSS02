/*
 * oFrequency.h
 *
 *  Created on: 28 мая 2014 г.
 *      Author: temmka
 *  Description: 
 */

#ifndef CL_FRRMS_H_
#define CL_FRRMS_H_

#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "math.h"
/*
 *@autor
 */
class FrRms
{
public:
	FrRms();
	void init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset);
	void calculate(bool DtU);
	void sendToItem(MenuItem &Freq, MenuItem &Rms);

	u16 *SigOk;
	u16 *ADCavr;
	u16 *ZeroOffset;
	u16 cntFreqMax;
	bool *dataOk;
private:

	u16 cntCalc;
	u16 cntFreq;

	u16 cntAvr;
	float avrFr;
	float tAvrFr;
	float factor;


	float rms;
	u32 rmsSum = 0;
	u16 tmp = 0;


};

#endif /* OFREQUENCY_H_ */
