
#include "MenuIO.h"

Menu_IO::Menu_IO()
{

	DigitCnt = 0;

}
;

void Menu_IO::addDiigit(GPIO_TypeDef* Port, u16 Pin)
{

}

void Menu_IO::GpioConfig()
{

}

void Menu_IO::vPortWrite(u8 value, bool dp, u8 CurrDigit)
{

	(value & 0x01) ? GpioIO.A_Port->BSRRL = GpioIO.A_Pin : GpioIO.A_Port->BSRRH = GpioIO.A_Pin;
	(value & 0x02) ? GpioIO.B_Port->BSRRL = GpioIO.B_Pin : GpioIO.B_Port->BSRRH = GpioIO.B_Pin;
	(value & 0x04) ? GpioIO.C_Port->BSRRL = GpioIO.C_Pin : GpioIO.C_Port->BSRRH = GpioIO.C_Pin;
	(value & 0x08) ? GpioIO.D_Port->BSRRL = GpioIO.D_Pin : GpioIO.D_Port->BSRRH = GpioIO.D_Pin;
	(value & 0x10) ? GpioIO.E_Port->BSRRL = GpioIO.E_Pin : GpioIO.E_Port->BSRRH = GpioIO.E_Pin;
	(value & 0x20) ? GpioIO.F_Port->BSRRL = GpioIO.F_Pin : GpioIO.F_Port->BSRRH = GpioIO.F_Pin;
	(value & 0x40) ? GpioIO.G_Port->BSRRL = GpioIO.G_Pin : GpioIO.G_Port->BSRRH = GpioIO.G_Pin;
	(dp) ? GpioIO.DP_Port->BSRRL = GpioIO.DP_Pin : GpioIO.DP_Port->BSRRH = GpioIO.DP_Pin;

	for (u8 i = 0; i < DigitMax; i++)
	{

		if (i == CurrDigit)
		{
			Digit[i].Port->BSRRL = Digit[i].Pin;
		}
		else
		{
			Digit[i].Port->BSRRH = Digit[i].Pin;
		}

	}
}
