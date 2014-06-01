
#ifndef CL_ADC_H
#define CL_ADC_H

#include "stm32f4xx_conf.h"
#include "user_conf.h"


class AdcX
{
public:
	AdcX();
	void init(bool *idDataOk, u16 *iSigOk, u16 *iADCavr, u16 *iZeroOffset, ADC_TypeDef* ADC_N);
	void sample(void);
	void sendToItem(MenuItem &Adc);

	bool *dataOk;
	u16 *SigOk;

	u16 *ADCavr;
	u16 *ZeroOffset;
	u16 MaxAvrCount;
	u8 i;
	uint32_t ADCBuff;
	ADC_TypeDef* ADCx;


//MIN[4].pValue = (float) *aADCavr;
};

#endif /* CLADC_H_ */
