#ifndef USER_FUNCTION_01_H
#define USER_FUNCTION_01_H

#include "stm32f4xx_conf.h"
#include "IO_7segment.h"
#include "softSpi.h"






#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

enum
{
	OUT_VALUE = 0,
	PARAMETR  = 1
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
bool filterOut(u16 &fcnt,u16 max);

void initProfibusTimer(void);
void initProfibusUsart(void);

void gpioInit(IO_7segment * SevenSeg, softSpi * spiFlash);
void usrMenuBuild(void);
void profibusDataExchange(void);
void send_USART_dma(const uint8_t *buff, uint8_t size);
float ItoF(u16 **buff, u16 number);

void initRTC(void);

/*read 8 input pin to byte*/
uint8_t readSlaveAddress(void);

void pinConfig(void);
void writeBitInReg(uint16_t reg, uint8_t bitNumber, uint8_t value);
uint8_t readBitInReg(uint16_t reg, uint8_t bitNumber);
uint16_t tPow2(uint16_t n);
void regBitToPin(uint16_t reg, uint8_t bitNumber, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void PinToregBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t &reg, uint8_t bitNumber);
void invPinToregBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t &reg, uint8_t bitNumber);

uint8_t highByte(uint16_t Value);
uint8_t lowByte(uint16_t Value);
uint16_t _crc16_update(uint16_t crc, uint8_t a);
int bitRead(uint8_t b, int bitPos);






uint16_t word(uint8_t high, uint8_t low);
void delayMicroseconds(uint32_t time);

bool ton(bool input, u16 delayTime);
bool tof(bool input, u16 delayTime);

bool tpulse(void);


void alToRegBit (uint16_t *reg, uint8_t bitNumber);




#ifdef __cplusplus
}
#endif

#endif /* FUNC_H */

