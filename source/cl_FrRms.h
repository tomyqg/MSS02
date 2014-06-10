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
	void init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset, u8 *iAcDc);
	void calculateAC(bool DtU);
	void calculateDC();
	void sendToItem(MenuItem &Freq, MenuItem &Rms);

	static const u8 FilterOutValue = 25;
	u16 *SigOk;
	u16 *ADCavr;
	u16 *ZeroOffset;

	float RmsFactor;
	bool *dataOk;


	u8 *AcDc; // 0=AC, 1=DC;
	u16 cntFreqMax;
	u16 cntRmsMax;

	u16 DcCounter;

private:

	u16 cntCalc;


	u16 cntFreq;


	u16 cntRms;

	u8  cntEnOutValue; //Counter - filter first data

	u16 cntAvr;
	float avrFr;
	float tAvrFr;
	float factor;

	float tRms;
	float tAvrRms;
	float avrRms;
	u32 rmsSum = 0;
	u16 tmp = 0;


};

#endif /* OFREQUENCY_H_ */
