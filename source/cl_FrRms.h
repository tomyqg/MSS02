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
#include <Averaging.h>
/*
 *@autor
 */
class FrRms
{
public:
	FrRms();
	void init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset);
	void calculateAC(bool DtU);
	void calculateDC();
	void sendToItem(MenuItem &Freq, MenuItem &Rms);
	u16 AbsValue(u16 iValue, u16 iZeroOffset, bool iDC);


	u16 *SigOk;
	u16 *ADCavr;
	u16 *ZeroOffset;

	float RmsFactor;
	bool *dataOk;


	u16 AbsAdc;

	u16 AvValueRms;
	u16 AvValueFreq;

	u16 DcCounter;



	u16 cntPerPeriod; //cycles during one period

	Averaging AvFreq;
	Averaging AvRms;

private:

	u16 tCntPerPeriod; //cycles during one period


	float factor;

	float Freq;
	float avrFreq;

	float Rms;
	float avrRms;

	u32 sumRms;



};

#endif /* OFREQUENCY_H_ */
