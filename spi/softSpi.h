/*
 * softSpi.h
 *
 *  Created on: 18.10.2013
 *      Author: AS
 */

#ifndef SOFTSPI_H
#define SOFTSPI_H

#include "stm32f4xx_conf.h"


typedef struct {
	GPIO_TypeDef*  csPORT;
	uint16_t       csPIN;

	GPIO_TypeDef*  clkPORT;
	uint16_t       clkPIN;

	GPIO_TypeDef*  soPORT;
	uint16_t       soPIN;

	GPIO_TypeDef*  siPORT;
	uint16_t       siPIN;
}GpioSpi;





class softSpi {
	public:
	u16 pauseValue;

	softSpi();
	softSpi(GpioSpi pinStruct);


	void init(GpioSpi pinStruct);

	void Write(u8 value);
	u8 Read(void);
	void Write(u8 *value,u8 size);
	void Read(u8 *value, u8 size);

	void CsActive(void);
	void CsDeactive(void);

    friend class eeprom;
    friend class mcp23s17;


	GPIO_TypeDef* csPort;
	uint16_t       csPin;

	private:

	GPIO_TypeDef* clkPort;
	uint16_t       clkPin;

	GPIO_TypeDef* soPort;
	uint16_t       soPin;

	GPIO_TypeDef* siPort;
	uint16_t       siPin;

	void pause(void); //use internal value
	void pause(u16 count);//use external value

	void SclkSet(void);
	void SclkReset(void);

	void WriteMosi(bool state);
	bool ReadMiso(void);

};

#endif /* SOFTSPI_H_ */
