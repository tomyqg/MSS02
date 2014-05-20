#ifndef IO_7SEGMENT_H
#define IO_7SEGMENT_H

#include "stm32f4xx_conf.h"

/**
 * Pinout 7-segment display:
 * bit_0 = A - segment
 * bit_1 = B - segment
 * bit_2 = C - segment
 * bit_3 = D - segment
 * bit_4 = E - segment
 * bit_5 = F - segment
 * bit_6 = G - segment
 *
 */


/**
 * Numeric code of 7-segment display
 */
static const u8 num[10]
{       0x3F, //0
		0x06, //1
		0x5B, //2
		0x4F, //3
		0x66, //4
		0x6D, //5
		0x7D, //6
		0x07, //7
		0x7F, //8
		0x6F, //9
};

/**
 * Symbl code of 7-segment display
 */
enum sym{
sym_A = 0x77,//!< sym_A
sym_b = 0x7C,//!< sym_b
sym_C = 0x39,//!< sym_C
sym_c = 0x58,//!< sym_c
sym_d = 0x5E,//!< sym_d
sym_E = 0x79,//!< sym_E
sym_F = 0x71,//!< sym_F
sym_H = 0x76,//!< sym_H
sym_i = 0x10,//!< sym_i
sym_J = 0x1E,//!< sym_J
sym_h = 0x74,//!< sym_h
sym_L = 0x38,//!< sym_L
sym_l = 0x38,//!< sym_l
sym_n = 0x54,//!< sym_n
sym_o = 0x5C,//!< sym_o
sym_P = 0x73,//!< sym_P
sym_r = 0x50,//!< sym_r
sym_t = 0x78,//!< sym_t
sym_U = 0x3E,//!< sym_U
sym_u = 0x1C,//!< sym_u
sym_Y = 0x6E,//!< sym_Y
sym_PP = 0x37,//Ï
sym_GG = 0x31, //Ã
sym_neg = 0x40, //-

};


/**
 * Gpio lite struct
 */
typedef struct
{
	GPIO_TypeDef* Port;
	u16 Pin;
} GpioLt;


/**
 * Gpio 7-segment display
 */
typedef struct
{
	GpioLt A;
	GpioLt B;
	GpioLt C;
	GpioLt D;
	GpioLt E;
	GpioLt F;
	GpioLt G;
	GpioLt DP;
	GpioLt SG;

} GpioSS_IO;




class IO_7segment
{
public:

	u8 DigitMax; //Max digit in 7-segment display
	GpioSS_IO GpioIO; //Gpio 7-segment display
	GpioLt *Digit; //Gpio power pin of digit


	IO_7segment(u8 maxDigit);

	void GpioIOConfig();//Init GPIO



	void DigitWrite(u8 value, bool point, u8 CurrDigit);
	void write(u16 value);
	void write(s16 value);
	void write(u8 letter, u16 value);
	void write(float value);
};

#endif /* MENUIO_H_ */
