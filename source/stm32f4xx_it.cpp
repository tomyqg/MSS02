/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

// UART variables
u8 uart_buffer[UART_BUFFER_SIZE]; // uart temporary buffer
u16 uart_byte_cnt;                // uart receive byte counter

//Modbus Slave Variables
extern modbusSlave mbs_Slave;    //modbus object
extern u16 *mbs_table[BUFF_SIZE]; //modbus IO table

//Profibus slave variables
extern u8 profibus_status;

//ADC variables
extern __IO uint16_t aADCDualConvertedValue[4];
extern uint32_t aADCBuff[4];
extern uint16_t aADCavr[4];

u16 adcAvrCnt = 0; // Averaging count
float rms;



extern MenuItem MIN[46]; //MIN01

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
	static u16 arr[1000];
	static int i = 0;
	aADCDualConvertedValue[0] = ADC_GetConversionValue(ADC1);
	aADCDualConvertedValue[1] = ADC_GetConversionValue(ADC2);
	arr[i] = aADCDualConvertedValue[0];
	i++;

	if (i >= 1000)
	{

		i = 0;
	}

}

void TIM4_IRQHandler(void)
{
	static u8 i = 0;
	u16 avrCnt = 10; // (u16)MIN[13].getValue();

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		aADCDualConvertedValue[0] = ADC_GetConversionValue(ADC1);
		aADCDualConvertedValue[1] = ADC_GetConversionValue(ADC2);

		if (i > avrCnt)
		{

			//Calculate average adc value
			aADCavr[0] = aADCBuff[0] / avrCnt;
			aADCavr[1] = aADCBuff[1] / avrCnt;
MIN[4].pValue=(float)aADCavr[0];
MIN[5].pValue=(float)aADCavr[1];


			//reset data
			i = 0;
			aADCBuff[0] = 0;
			aADCBuff[1] = 0;

			//sampling data
			aADCBuff[0] += aADCDualConvertedValue[0];
			aADCBuff[1] += aADCDualConvertedValue[1];

			i++;

		}
		else
		{
			//sampling data
			aADCBuff[0] += aADCDualConvertedValue[0];
			aADCBuff[1] += aADCDualConvertedValue[1];

			i++;
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}

}

bool DwnToUp(u16 Value)
{

	u16 ZeroOffset = (u16) MIN[15].getValue();
	static u16 PlusHyst = 10;
	static u16 MinusHyst = 10;
	static u8 mCurrPos = 0; // 0 = In Minus phase, 1 = In Plus phase

	if (Value > ZeroOffset + PlusHyst)
	{
		if (mCurrPos == 0)
		{
			mCurrPos = 1;
			return true;
		}
		mCurrPos = 1;

	}

	if (Value < ZeroOffset - MinusHyst)
	{
		mCurrPos = 0;
	}

	return false;

}

void TIM2_IRQHandler(void)
{

	static u32 CntValue = 0;
	static u32 rmsSum = 0;
	static u16 tmp = 0;

	u16 ZeroOffset = (u16) MIN[15].getValue();

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{

		if (DwnToUp(aADCavr[0]))
		{
			MIN[6].setValue(1 / ((float) CntValue * 0.00005));

			adcAvrCnt = CntValue;

			rms = sqrtf((float)rmsSum);
			rms = rms/CntValue;
			CntValue = 1;
			rmsSum = 0;

		}
		else
		{
			CntValue++;

			if (aADCavr[0] > ZeroOffset ){
				tmp = aADCavr[0] - ZeroOffset;
			}

			if (aADCavr[0] < ZeroOffset ){
				tmp =  ZeroOffset - aADCavr[0];
			}




			rmsSum += tmp*tmp;

		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}

/**
 * Modbus/Profibus timer
 */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(PROFIBUS_TIMER, TIM_IT_Update) != RESET)
	{
		//Select modbus or profibus used
		if (USE_MODBUS)
		{
			if (uart_byte_cnt > 0)
			{
				mbs_Slave.update(mbs_table, BUFF_SIZE);
				uart_byte_cnt = 0;

			}
			MODBUS_TIMER->CNT = 0;
		}
		else
		{
			//Disable interrupt
			NVIC_DisableIRQ(PROFIBUS_TIMER_IRQn);

			switch (profibus_status)
			{
			case PROFIBUS_WAIT_SYN: // TSYN expired

				profibus_status = PROFIBUS_WAIT_DATA;

				PROFIBUS_TIMER->ARR = TIMEOUT_MAX_SDR_TIME;
				PROFIBUS_TIMER->CNT = 0;
				uart_byte_cnt = 0;

				break;

			case PROFIBUS_WAIT_DATA: //TSDR expired but no data because

				break;

			case PROFIBUS_GET_DATA: //TSDR expired and data as

				profibus_status = PROFIBUS_WAIT_SYN;
				PROFIBUS_TIMER->ARR = TIMEOUT_MAX_SYN_TIME;
				PROFIBUS_TIMER->CNT = 0;
				profibus_RX();

				break;

			case PROFIBUS_SEND_DATA: //TX timeout expired, go back to receive

				profibus_status = PROFIBUS_WAIT_SYN;
				PROFIBUS_TIMER->ARR = TIMEOUT_MAX_SYN_TIME;
				PROFIBUS_TIMER->CNT = 0;

				break;

			default:
				break;

			}

			NVIC_EnableIRQ(PROFIBUS_TIMER_IRQn);

		}

#ifdef USE_COMUNICATION_LED
		COMUNICATION_LED_PORT->BSRRH = COMUNICATION_LED_PIN;
#endif

		TIM_ClearITPendingBit(PROFIBUS_TIMER, TIM_IT_Update);
	}

}

/**
 * Modbus/Profibus UART
 */
void USART6_IRQHandler(void)
{
	if (USART_GetITStatus(PROFIBUS_USART, USART_IT_RXNE) != RESET)
	{

		if (USE_MODBUS)
		{
			uart_buffer[uart_byte_cnt] = (uint8_t) MODBUS_USART->DR;
			uart_byte_cnt++;
			MODBUS_TIMER->CNT = 0;
		}
		else
		{

			// Profibus timer reset
			PROFIBUS_TIMER->CNT = 0;

			// Save first byte in buffer
			uart_buffer[uart_byte_cnt] = USART_ReceiveData(PROFIBUS_USART);

			// Only read if TSYN expired
			if (profibus_status == PROFIBUS_WAIT_DATA)
			{

				// TSYN expired, read data
				profibus_status = PROFIBUS_GET_DATA;
			}

			// Reading allowed?
			if (profibus_status == PROFIBUS_GET_DATA)
			{
				uart_byte_cnt++;

				//Read no more than fits into buffer
				if (uart_byte_cnt == MAX_BUFFER_SIZE)
					uart_byte_cnt--;
			}

			// Profibus timer reset
			PROFIBUS_TIMER->CNT = 0;
		}

		USART_ClearITPendingBit(PROFIBUS_USART, USART_SR_RXNE);
	}
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
