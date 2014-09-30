
#ifndef CL_ADC_H
#define CL_ADC_H

#include "stm32f4xx_conf.h"
#include "user_conf.h"


class AdcX
{
public:
	AdcX();
	void init(MinComunication *iMinCom, ADC_TypeDef* ADC_N);
	void sample();
	void sendToItem(MenuItem &Adc);


	u16 MaxAvrCount;
	u8 i;
	uint32_t ADCBuff;
	ADC_TypeDef* ADCx;
	MinComunication *MinCom;


};

#endif /* CLADC_H_ */
