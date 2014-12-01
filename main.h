#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx_conf.h"
//#include "user_conf.h"

const float SOFT_VERSION = 11;


const uint32_t  SYSTICK_VALUE = 168000;
const uint32_t  SYSCLK_VALUE = 168000000;
///////////////////////////////////////////////////////////////////////////////////////////////////
// MODBUS USART
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MODBUS_USART              USART6
#define MODBUS_USART_BAUDRATE    (uint32_t)921600

#define MODBUS_USART_WORDLENGTH   USART_WordLength_8b
#define MODBUS_USART_STOPBITS     USART_StopBits_1
#define MODBUS_USART_PARITY       USART_Parity_No
#define MODBUS_USART_HWFWCTRL     USART_HardwareFlowControl_None

#define MODBUS_USART_TX_PORT      GPIOC
#define MODBUS_USART_TX_PORT_AF   RCC_AHB1Periph_GPIOC
#define MODBUS_USART_TX_PIN       GPIO_Pin_6
#define MODBUS_USART_TX_PINSOURCE GPIO_PinSource6

#define MODBUS_USART_RX_PORT      GPIOC
#define MODBUS_USART_RX_PORT_AF   RCC_AHB1Periph_GPIOC
#define MODBUS_USART_RX_PIN       GPIO_Pin_7
#define MODBUS_USART_RX_PINSOURCE GPIO_PinSource7

#define MODBUS_USART_IRQN	       USART6_IRQn
#define MODBUS_USART_IRQHANDLER    USART6_IRQHandler
#define MODBUS_USART_GPIO_AF       GPIO_AF_USART6
#define MODBUS_USART_RCC           RCC_APB2Periph_USART6
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// MODBUS TIMER
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MODBUS_TIMER        	   TIM3
#define MODBUS_TIMER_IRQN	  	   TIM3_IRQn
#define MODBUS_TIMER_IRQHANDLER	   TIM3_IRQHandler
#define MODBUS_TIMER_RCC           RCC_APB1Periph_TIM3
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Profibus UART constant
///////////////////////////////////////////////////////////////////////////////////////////////////
#define PROFIBUS_USART        		USART6
#define PROFIBUS_USART_Baudrate     (uint32_t)500000

#define PROFIBUS_USART_TX_Port      GPIOC
#define PROFIBUS_USART_TX_Port_AF   RCC_AHB1Periph_GPIOC
#define PROFIBUS_USART_TX_Pin       GPIO_Pin_6
#define PROFIBUS_USART_TX_PinSource GPIO_PinSource6

#define PROFIBUS_USART_RX_Port      GPIOC
#define PROFIBUS_USART_RX_Port_AF   RCC_AHB1Periph_GPIOC
#define PROFIBUS_USART_RX_Pin       GPIO_Pin_7
#define PROFIBUS_USART_RX_PinSource GPIO_PinSource7

#define PROFIBUS_USART_IRQn	  		USART6_IRQn
#define PROFIBUS_USART_IRQHandler	USART6_IRQHandler
#define PROFIBUS_USART_GPIO_AF      GPIO_AF_USART6
#define PROFIBUS_USART_RCC          RCC_APB2Periph_USART6

///////////////////////////////////////////////////////////////////////////////////////////////////
// Profibus timer constant
///////////////////////////////////////////////////////////////////////////////////////////////////
#define PROFIBUS_TIMER        		TIM3
#define PROFIBUS_TIMER_IRQn	  		TIM3_IRQn
#define PROFIBUS_TIMER_IRQHandler	TIM3_IRQHandler
#define PROFIBUS_TIMER_RCC          RCC_APB1Periph_TIM3
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Profibus data exchange constant
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_BUFFER_SIZE       45

#define INPUT_DATA_SIZE       5     // Number of bytes coming from the master
#define OUTPUT_DATA_SIZE      5     // Number of bytes that go to the master
#define MODULE_CNT            10    // Number of modules (input and output modules) with modular station

#define USER_PARA_SIZE        0     // Number of bytes for manufacturer-specific parameter data
#define EXT_DIAG_DATA_SIZE    0     // Number of bytes for advanced diagnostics
#define VENDOR_DATA_SIZE      0     // Number of manufacturer-specific module data
///////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct {
	u16 ADCavr;     // <=Ch1_Adc
	u8 mCurPos;     // <=Min
	u16 ZeroOffset; // <=Cycle
	u8 AC_DC;      //0=AC, 1=DC
	bool DtU;       // <=Min
	u16 SignalOk;   //<=Ch1_Adc
	bool dataOk;    //<=Ch1_Adc
	float RmsFactor;
	u16 AbsAdc;     // <=FrRms.AbsAdc
float Rms;



}MinComunication;

















///////////////////////////////////////////////////////////////////////////////////////////////////
// USER PARAMETR
///////////////////////////////////////////////////////////////////////////////////////////////////

#define BUFF_SIZE 100 //Modbus table size
#define UART_BUFFER_SIZE 50
#define USE_COMUNICATION_LED
#define COMUNICATION_LED_PORT      GPIOC
#define COMUNICATION_LED_PIN       GPIO_Pin_0


///////////////////////////////////////////////////////////////////////////////////////////////////
//MENU PARAMETR
///////////////////////////////////////////////////////////////////////////////////////////////////
const u32 MENU_RESET_TIME =300; // reset selected menu time
const u8  USED_BOARDS = 4; // used board counter //TODO create variable


#define USE_MODBUS 1






#endif /* MAIN_H */
