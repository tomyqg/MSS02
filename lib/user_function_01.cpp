/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#include "user_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/




/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

bool filterOut(u16 &fcnt, u16 max)
{

	if (fcnt >= max)
	{
		fcnt = 0;
		return true;
	}
	else
	{

		fcnt++;
		return false;
	}

}

void initProfibusTimer(void)
{
	TIM_TimeBaseInitTypeDef TimeBaseInit_Structure;
	NVIC_InitTypeDef NVIC_Init_Structure;

	RCC_APB1PeriphClockCmd(PROFIBUS_TIMER_RCC, ENABLE);




//1 = 2us;
//10 = 11 us
//100 = 101 us


	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = 34;//103
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(PROFIBUS_TIMER, &TimeBaseInit_Structure);
	TIM_Cmd(PROFIBUS_TIMER, ENABLE);
	TIM_ARRPreloadConfig(PROFIBUS_TIMER, ENABLE);

	TIM_ITConfig(PROFIBUS_TIMER, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = PROFIBUS_TIMER_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);

}

void initProfibusUsart(void)
{
	NVIC_InitTypeDef NVIC_Init_Structure;
		USART_InitTypeDef USART_Init_Structure;
		GPIO_InitTypeDef GPIO_Init_Structure;

		RCC_AHB1PeriphClockCmd(MODBUS_USART_TX_PORT_AF, ENABLE);
		RCC_AHB1PeriphClockCmd(MODBUS_USART_RX_PORT_AF, ENABLE);
		RCC_APB2PeriphClockCmd(MODBUS_USART_RCC, ENABLE);

		//  Connect PXx to USARTx_Tx
		GPIO_PinAFConfig(MODBUS_USART_TX_PORT, MODBUS_USART_TX_PINSOURCE,
		MODBUS_USART_GPIO_AF);

		//  Connect PXx to USARTx_Rx
		GPIO_PinAFConfig(MODBUS_USART_RX_PORT, MODBUS_USART_RX_PINSOURCE,
		MODBUS_USART_GPIO_AF);

		GPIO_Init_Structure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_100MHz;

		//Configure USART Tx as alternate function
		GPIO_Init_Structure.GPIO_Pin = MODBUS_USART_TX_PIN;
		GPIO_Init(MODBUS_USART_TX_PORT, &GPIO_Init_Structure);

		//Configure USART Rx as alternate function
		GPIO_Init_Structure.GPIO_Pin = MODBUS_USART_RX_PIN;
		GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_Init_Structure);

	USART_Init_Structure.USART_BaudRate = PROFIBUS_USART_Baudrate;
	USART_Init_Structure.USART_WordLength = USART_WordLength_9b;
	USART_Init_Structure.USART_StopBits = USART_StopBits_1;
	USART_Init_Structure.USART_Parity = USART_Parity_Even;
	USART_Init_Structure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_Init_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(PROFIBUS_USART, &USART_Init_Structure);
	USART_Cmd(PROFIBUS_USART, ENABLE);
	USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = MODBUS_USART_IRQN;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);
	NVIC_EnableIRQ(MODBUS_USART_IRQN);
}


void ptrFtoI(float *value, u16 **buff, u16 number)
{
buff[number] = (u16*)value;
buff[number+1] = buff[number]+1;
}





void send_USART_dma(const uint8_t *buff, uint8_t size)
{

	DMA_DeInit(DMA2_Stream6);

	DMA_InitTypeDef DMA_InitStructure;
	/* Configure DMA Initialization Structure */
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(USART6->DR));
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	/* Configure TX DMA */
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) buff;
	DMA_Init(DMA2_Stream6, &DMA_InitStructure);

	DMA_Cmd(DMA2_Stream6, ENABLE);
	USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);

}



void profibusDataExchange(void)
{

}



uint8_t readSlaveAddress(void)
{
	uint8_t addr = 0;
	addr |= (GPIOA->IDR & GPIO_Pin_9) ? 0 : 1;
	addr |= ((GPIOA->IDR & GPIO_Pin_10) ? 0 : 1) << 1;
	addr |= ((GPIOA->IDR & GPIO_Pin_11) ? 0 : 1) << 2;
	addr |= ((GPIOA->IDR & GPIO_Pin_12) ? 0 : 1) << 3;
	addr |= ((GPIOA->IDR & GPIO_Pin_15) ? 0 : 1) << 4;
	addr |= ((GPIOC->IDR & GPIO_Pin_10) ? 0 : 1) << 5;
	addr |= ((GPIOC->IDR & GPIO_Pin_11) ? 0 : 1) << 6;
	addr |= ((GPIOC->IDR & GPIO_Pin_12) ? 0 : 1) << 7;

	return addr;
}



uint16_t tPow2(uint16_t n)
{
	uint16_t i, result;

	result = 1;

	for (i = 1; i <= n; i++)
	{
		result = result * 2;
	}

	return result;

}

void regBitToPin(uint16_t reg, uint8_t bitNumber, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if (reg & (1 << bitNumber))
	{
		GPIOx->BSRRL = GPIO_Pin;
	}
	else
	{
		GPIOx->BSRRH = GPIO_Pin;
	}

}

void PinToregBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t &reg, uint8_t bitNumber)
{

	if ((GPIOx->IDR & GPIO_Pin) != (uint32_t) Bit_RESET)
	{
		reg |= (1 << bitNumber);
	}
	else
	{
		reg &= ~(1 << bitNumber);
	}
}

void invPinToregBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t &reg, uint8_t bitNumber)
{
	if ((GPIOx->IDR & GPIO_Pin) != (uint32_t) Bit_RESET)
	{
		reg &= ~(1 << bitNumber);
	}
	else
	{
		reg |= (1 << bitNumber);
	}
}

uint8_t highByte(uint16_t Value)
{
	return (Value >> 8) & 0xFF;
}

uint8_t lowByte(uint16_t Value)
{
	return Value & 0xFF;
}

uint16_t _crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;
}

int bitRead(uint8_t b, int bitPos)
{
	int x = b & (1 << bitPos);
	return x == 0 ? 0 : 1;
}

uint16_t word(uint8_t high, uint8_t low)
{
	uint16_t ret_val = low;
	ret_val += (high << 8);
	return ret_val;
}

void delayMicroseconds(uint32_t time)
{
	time *= 17;
	while (time > 0)
	{

		time--;
	}

}

/* delay-on function with RTC */
bool ton(bool input, u16 delayTime)
{
	static u32 cmpVal = 0; //setpoint value
	static bool p = 0;     //one cycle var
	bool out = 0;          //output signal(returned)

	if (input == true) //input signal on
	{
		if (p == false) //first cycle
		{
			cmpVal = RTC->TR + delayTime; //set on-value
			p = true;

		} //first cycle

		else //other cycle
		{
			if (RTC->TR >= cmpVal)
			{
				out = true;
			}
			else
			{
				out = false;
			}

		} //other cycle

	} //input signal on

	else //input signal off
	{
		out = false;
		cmpVal = 0;
		p = false;

	} //input signal off

	return out;
}

/* delay-off function with RTC */
bool tof(bool input, u16 delayTime, bool &p,u32 &cmpVal )
{

	bool out = 0;          //output signal(returned)

	if (input == true) //input signal on
	{
		out = true;
		cmpVal = 0;
		p = false;
	} //input signal on

	else //input signal off
	{
		if (p == false) //first cycle
		{
			cmpVal = RTC->TR + delayTime; //set off-value
			p = true;

		} //first cycle

		else //other cycle
		{
			if (RTC->TR >= cmpVal)
			{
				out = false;

			}
			else
			{
				out = true;
			}

		} //other cycle

	} //input signal off

	return out;
}

bool tpulse(void)
{
	return RTC->TR & 0x00000001;
}

void initRTC(void)
{

	RTC_InitTypeDef RTC_InitStructure;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* LSI used as RTC source clock */
	/* The RTC Clock may varies due to LSI frequency dispersion. */
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Calendar Configuration with LSI supposed at 32KHz */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv = 0xFF; /* (32KHz / 128) - 1 = 0xFF*/
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

}

float ItoF(u16 **buff, u16 number)
{
	float *pf;
	pf = (float*)buff[number];

return *pf;
}
