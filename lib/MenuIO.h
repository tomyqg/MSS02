#ifndef MENUIO_H
#define MENUIO_H

#include "stm32f4xx_conf.h"
//#include "user_conf.h"




typedef struct{

	GPIO_TypeDef* Port;
	u16           Pin;

}GpioLt;

typedef struct{

	GPIO_TypeDef* A_Port;
	GPIO_TypeDef* B_Port;
	GPIO_TypeDef* C_Port;
	GPIO_TypeDef* D_Port;
	GPIO_TypeDef* E_Port;
	GPIO_TypeDef* F_Port;
	GPIO_TypeDef* G_Port;
	GPIO_TypeDef* DP_Port;

	u16           A_Pin;
	u16           B_Pin;
	u16           C_Pin;
	u16           D_Pin;
	u16           E_Pin;
	u16           F_Pin;
	u16           G_Pin;
	u16           DP_Pin;


}GpioSS_IO;



class Menu_IO {
public:

	static const u8 DigitMax = 4; //Количество сегментов
	u8 DigitCnt; //Количество используемых сегментов
	GpioSS_IO GpioIO;

	Menu_IO();
	GpioLt Digit[DigitMax];

    void addDiigit(GPIO_TypeDef* Port, u16 Pin);

    void  vPortWrite(u8 value, bool digit, u8 CurrDigit);




    void GpioConfig();







};

#endif /* MENUIO_H_ */
