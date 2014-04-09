/*
 * softSpi.cpp
 *
 *  Created on: 18.10.2013
 *      Author: AS
 */

#include "user_conf.h"

softSpi::softSpi(){
	csPort = GPIOA;
	clkPort = GPIOA;
	siPort = GPIOA;
	soPort = GPIOA;

	csPin = 0;
	clkPin = 0;
	siPin = 0;
	soPin = 0;

}


softSpi::softSpi(GpioSpi pinStruct) {
	softSpi::init(pinStruct);
}




void softSpi::pause(void) {
	u16 tPauseValue = pauseValue;
	while (tPauseValue > 0)
		tPauseValue--;
}

void softSpi::pause(u16 count) {
	while (count > 0)
		count--;
}

void softSpi::init(GpioSpi pinStruct) {

	GPIO_InitTypeDef GPIO_InitStructure;

	pauseValue = 2;

	csPort  = pinStruct.csPORT;
	clkPort = pinStruct.clkPORT;
	soPort  = pinStruct.soPORT;
	siPort  = pinStruct.siPORT;

	csPin  = pinStruct.csPIN;
	clkPin = pinStruct.clkPIN;
	soPin  = pinStruct.soPIN;
	siPin  = pinStruct.siPIN;

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_InitStructure.GPIO_Pin = csPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(csPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = clkPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(clkPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = siPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(siPort, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = soPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(soPort, &GPIO_InitStructure);

	SclkSet();
	CsDeactive();
	pause(pauseValue*2);

}

void softSpi::CsActive(void) {
	csPort->BSRRH = csPin;
}

void softSpi::CsDeactive(void) {
	csPort->BSRRL = csPin;
}

void softSpi::SclkSet(void) {
	clkPort->BSRRL = clkPin;
}

void softSpi::SclkReset(void) {
	clkPort->BSRRH = clkPin;
}

void softSpi::WriteMosi(bool state) {
	SclkReset();
	pause();
	(state == 0) ? siPort->BSRRH = siPin : siPort->BSRRL = siPin;
	pause();
	SclkSet();
	pause(pauseValue * 2);
}

bool softSpi::ReadMiso(void) {

	bool retval = false;

	SclkReset();
	pause();

	retval = (soPort->IDR & soPin) ? true : false;
	pause();
	SclkSet();
	pause(pauseValue * 2);

	return retval;
}

void softSpi::Write(u8 value){
	    WriteMosi(value & 0x80);
		WriteMosi(value & 0x40);
		WriteMosi(value & 0x20);
		WriteMosi(value & 0x10);
		WriteMosi(value & 0x08);
		WriteMosi(value & 0x04);
		WriteMosi(value & 0x02);
		WriteMosi(value & 0x01);

}

u8 softSpi::Read(void){
	u8 rcvByte = 0;
	rcvByte |= 0x80 * ReadMiso();
	rcvByte |= 0x40 * ReadMiso();
	rcvByte |= 0x20 * ReadMiso();
	rcvByte |= 0x10 * ReadMiso();
	rcvByte |= 0x08 * ReadMiso();
	rcvByte |= 0x04 * ReadMiso();
	rcvByte |= 0x02 * ReadMiso();
	rcvByte |= 0x01 * ReadMiso();
	return rcvByte;
}


void softSpi::Write(u8 *value,u8 size){

	for(u8 i=0; i < size; i++){

		Write(value[i]);
	}


}

void softSpi::Read(u8 *value, u8 size){

	for(u8 i=0;i<size;i++){

		value[i]=Read();
	}

}
