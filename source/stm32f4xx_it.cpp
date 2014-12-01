/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_conf.h"
#include "user_conf.h"
#include "math.h"
#include "global.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern global prg;
// UART variables
u8 uart_buffer[UART_BUFFER_SIZE]; // uart temporary buffer
u16 uart_byte_cnt;                // uart receive byte counter

//Profibus slave variables
extern u8 profibus_status;

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

}

void TIM4_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{

		prg.itSampleADC();
		prg.itCalcFreq();

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}

}

void TIM2_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}

void TIM5_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{

		prg.Menu.Display();

		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
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
				prg.mbs_Slave.update(prg.mbs_table, BUFF_SIZE);
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

		TIM_ClearITPendingBit(PROFIBUS_TIMER, TIM_IT_Update);
	}

}

/**
 * Modbus/Profibus UART
 */
void USART6_IRQHandler(void)
{

	u8 len;

	if (USART_GetITStatus(PROFIBUS_USART, USART_IT_IDLE) != RESET)
	{


		uart_byte_cnt = UART_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA2_Stream1);

		NVIC_DisableIRQ(MODBUS_USART_IRQN);
		USART_ClearITPendingBit(PROFIBUS_USART, USART_SR_IDLE);


		DMA_Cmd(DMA2_Stream1, DISABLE);
		DMA_SetCurrDataCounter(DMA2_Stream1, UART_BUFFER_SIZE);//UART_BUFFER_SIZE);
		DMA_Cmd(DMA2_Stream1, ENABLE);
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);

		prg.mbs_Slave.update(prg.mbs_table, BUFF_SIZE);

	}

}

void DMA2_Stream1_IRQHandler(void)
{
	u16 x;
	/* Test on DMA Stream Transfer Complete interrupt */
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
	{

		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);

		// NVIC_EnableIRQ (MODBUS_USART_IRQN);

	}

	/* Test on DMA Stream Half Transfer interrupt */
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_HTIF1))
	{

		/* Clear DMA Stream Half Transfer interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_HTIF1);

		NVIC_EnableIRQ(MODBUS_USART_IRQN);

	}

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
