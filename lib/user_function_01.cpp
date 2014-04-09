/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#include "user_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern __IO uint16_t aADCDualConvertedValue[4];
extern u16 *mbs_table[BUFF_SIZE]; //modbus IO table

extern MenuManager Menu;

extern MenuItem RT[2]; //IO item
extern MenuItem SYS[8]; //System item
extern MenuItem MIN[46]; //MIN01
extern MenuItem MSY[25]; //MSY02
extern MenuItem MDI[9]; //MDI02
extern MenuItem MDO[37]; //MDO02

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

void initModbusUsart(void)
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

	USART_Init_Structure.USART_BaudRate = MODBUS_USART_BAUDRATE;
	USART_Init_Structure.USART_WordLength = MODBUS_USART_WORDLENGTH;
	USART_Init_Structure.USART_StopBits = MODBUS_USART_STOPBITS;
	USART_Init_Structure.USART_Parity = MODBUS_USART_PARITY;
	USART_Init_Structure.USART_HardwareFlowControl = MODBUS_USART_HWFWCTRL;
	USART_Init_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MODBUS_USART, &USART_Init_Structure);
	USART_Cmd(MODBUS_USART, ENABLE);
	USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = MODBUS_USART_IRQN;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);
	NVIC_EnableIRQ(MODBUS_USART_IRQN);
}

void initModbusTimer(void)
{
	uint16_t bytePerSecond; //  serial speed (bit/s)  / 8
	float timeForByte; // 1 byte transmit/recive time
	uint16_t overtime; // overtime
	uint16_t TimPeriod;

	bytePerSecond = MODBUS_USART_BAUDRATE / 8;

	timeForByte = (1.0 / (float) bytePerSecond) * 1000000.0; // = N mks  (*1000000 - correction for timer )

	overtime = 300; // added to timeForByte

	TimPeriod = (uint16_t) timeForByte + overtime;

	NVIC_InitTypeDef NVIC_Init_Structure;
	TIM_TimeBaseInitTypeDef TimeBaseInit_Structure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TimeBaseInit_Structure.TIM_Prescaler = 83;
	TimeBaseInit_Structure.TIM_Period = TimPeriod; // value -1; 1 = 2 mks 999 = 1000mks = 1ms
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MODBUS_TIMER, &TimeBaseInit_Structure);
	TIM_Cmd(MODBUS_TIMER, ENABLE);
	TIM_ARRPreloadConfig(MODBUS_TIMER, ENABLE);

	TIM_ITConfig(MODBUS_TIMER, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = MODBUS_TIMER_IRQN;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Init_Structure);

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
	TimeBaseInit_Structure.TIM_Period = 103;
	TimeBaseInit_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit_Structure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(PROFIBUS_TIMER, &TimeBaseInit_Structure);
	TIM_Cmd(PROFIBUS_TIMER, ENABLE);
	TIM_ARRPreloadConfig(PROFIBUS_TIMER, ENABLE);

	TIM_ITConfig(PROFIBUS_TIMER, TIM_IT_Update, ENABLE);

	NVIC_Init_Structure.NVIC_IRQChannel = PROFIBUS_TIMER_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
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
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
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

void usrMenuBuild(void)
{

	u16 mbsCnt=0;// counter of modbus data record

	u16 adr = 4;

	RT[0].config(sym_r, 0, 0, 10, 1, 0, 0, OUT_VALUE);
	RT[1].config(sym_r, 1, 1, 10, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(0, &RT[0]); ptrFtoI(&RT[0].pValue, mbs_table, mbsCnt);
	Menu.addItem(0, &RT[1]); ptrFtoI(&RT[1].pValue, mbs_table, mbsCnt+=2);

	SYS[0].config(sym_P, 0, 0, 9999, 1, 0, adr += 4, PARAMETR);
	SYS[1].config(sym_C, 1, 1, USED_BOARDS, 1, 1, adr += 4, PARAMETR);
	SYS[2].config(sym_C, 2, 1, 2, 1, 1, adr += 4, PARAMETR);
	SYS[3].config(sym_C, 3, 0, 255, 1, 1, adr += 4, PARAMETR);
	SYS[4].config(sym_C, 4, 0, 9999, 0.01, 0.01, adr += 4, PARAMETR);
	SYS[5].config(sym_C, 5, 0, 1000, 1, 1, 0, OUT_VALUE);
	SYS[6].config(sym_C, 6, 0, 1, 1, 0, adr += 4, PARAMETR);
	SYS[7].config(sym_C, 7, 0, 9999, 1, 0, adr += 4, PARAMETR);

	for (u8 i = 0; i < 7; i++)
	{
		Menu.addItem(0, &SYS[i]);

		ptrFtoI(&SYS[i].pValue, mbs_table, mbsCnt+=2);

	}
	Menu.addLast(0, &SYS[7]);  ptrFtoI(&SYS[7].pValue, mbs_table, mbsCnt+=2);

	MIN[1].config(sym_n, 1, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[2].config(sym_n, 2, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[3].config(sym_n, 3, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[4].config(sym_n, 4, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[5].config(sym_n, 5, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[6].config(sym_n, 6, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[7].config(sym_n, 7, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MIN[8].config(sym_n, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[9].config(sym_n, 9, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[10].config(sym_n, 10, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[11].config(sym_n, 11, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[12].config(sym_n, 12, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[13].config(sym_n, 13, 0, 2000, 1, 20, adr += 4, PARAMETR);
	MIN[14].config(sym_n, 14, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[15].config(sym_n, 15, 0, 4095, 1, 1860, adr += 4, PARAMETR);
	MIN[16].config(sym_n, 16, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[17].config(sym_n, 17, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[18].config(sym_n, 18, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[19].config(sym_n, 19, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[20].config(sym_n, 20, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[21].config(sym_n, 21, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[22].config(sym_n, 22, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[23].config(sym_n, 23, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[24].config(sym_n, 24, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[25].config(sym_n, 25, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[26].config(sym_n, 26, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[27].config(sym_n, 27, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[28].config(sym_n, 28, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[29].config(sym_n, 29, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[30].config(sym_n, 30, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[31].config(sym_n, 31, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[32].config(sym_n, 32, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[33].config(sym_n, 33, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[34].config(sym_n, 34, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[35].config(sym_n, 35, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[36].config(sym_n, 36, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[37].config(sym_n, 37, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[38].config(sym_n, 38, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[39].config(sym_n, 39, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[40].config(sym_n, 40, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[41].config(sym_n, 41, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[42].config(sym_n, 42, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[43].config(sym_n, 43, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[44].config(sym_n, 44, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MIN[45].config(sym_n, 45, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(1, &MIN[1]); ptrFtoI(&MIN[1].pValue, mbs_table, mbsCnt+=2);
	for (u8 i = 2; i < 46; i++)
	{
		Menu.addItem(1, &MIN[i]);
		ptrFtoI(&MIN[i].pValue, mbs_table, mbsCnt+=2);
	}

	MSY[1].config(sym_u, 1, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MSY[2].config(sym_u, 2, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MSY[3].config(sym_u, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[4].config(sym_u, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[5].config(sym_u, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[6].config(sym_u, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[7].config(sym_u, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[8].config(sym_u, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[9].config(sym_u, 9, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[10].config(sym_u, 10, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[11].config(sym_u, 11, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[12].config(sym_u, 12, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[13].config(sym_u, 13, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[14].config(sym_u, 14, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[15].config(sym_u, 15, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[16].config(sym_u, 16, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[17].config(sym_u, 17, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[18].config(sym_u, 18, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[19].config(sym_u, 19, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[20].config(sym_u, 20, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[21].config(sym_u, 21, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[22].config(sym_u, 22, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[23].config(sym_u, 23, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MSY[24].config(sym_u, 24, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(2, &MSY[1]);  ptrFtoI(&MSY[1].pValue, mbs_table, mbsCnt+=2);
	for (u8 i = 2; i < 25; i++)
	{
		Menu.addItem(2, &MSY[i]);
		ptrFtoI(&MSY[i].pValue, mbs_table, mbsCnt+=2);
	}

	MDI[1].config(sym_i, 1, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MDI[2].config(sym_i, 2, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[3].config(sym_i, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[4].config(sym_i, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[5].config(sym_i, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[6].config(sym_i, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[7].config(sym_i, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDI[8].config(sym_i, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(3, &MDI[1]); ptrFtoI(&MDI[1].pValue, mbs_table, mbsCnt+=2);
	for (u8 i = 2; i < 9; i++)
	{
		Menu.addItem(3, &MDI[i]);
		ptrFtoI(&MDI[i].pValue, mbs_table, mbsCnt+=2);
	}

	MDO[1].config(sym_o, 1, 0, 2000, 1, 1, adr += 4, OUT_VALUE);
	MDO[2].config(sym_o, 2, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[3].config(sym_o, 3, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[4].config(sym_o, 4, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[5].config(sym_o, 5, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[6].config(sym_o, 6, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[7].config(sym_o, 7, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[8].config(sym_o, 8, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[9].config(sym_o, 9, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[10].config(sym_o, 10, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[11].config(sym_o, 11, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[12].config(sym_o, 12, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[13].config(sym_o, 13, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[14].config(sym_o, 14, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[15].config(sym_o, 15, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[16].config(sym_o, 16, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[17].config(sym_o, 17, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[18].config(sym_o, 18, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[19].config(sym_o, 19, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[20].config(sym_o, 20, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[21].config(sym_o, 21, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[22].config(sym_o, 22, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[23].config(sym_o, 23, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[24].config(sym_o, 24, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[25].config(sym_o, 25, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[26].config(sym_o, 26, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[27].config(sym_o, 27, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[28].config(sym_o, 28, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[29].config(sym_o, 29, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[30].config(sym_o, 30, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[31].config(sym_o, 31, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[32].config(sym_o, 32, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[33].config(sym_o, 33, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[34].config(sym_o, 34, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[35].config(sym_o, 35, 0, 2000, 1, 1, adr += 4, PARAMETR);
	MDO[36].config(sym_o, 36, 0, 2000, 1, 1, adr += 4, PARAMETR);

	Menu.addRoot(4, &MDO[1]); ptrFtoI(&MDO[1].pValue, mbs_table, mbsCnt+=2);
	for (u8 i = 2; i < 37; i++)
	{
		Menu.addItem(4, &MDO[i]);
		ptrFtoI(&MDO[i].pValue, mbs_table, mbsCnt+=2);
	}

}

void profibusDataExchange(void)
{

}


void gpioInit(IO_7segment* SevenSeg, softSpi* spiFlash)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GpioSpi spiPinStruct;

	ADC_InitTypeDef ADC_InitStructure;    // ADC
	ADC_CommonInitTypeDef ADC_CommonInitStruct; // ADC
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // GPIO_A
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // GPIO_B
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // GPIO_C
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // GPIO_D
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // GPIO_E
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); // GPIO_F
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); // GPIO_G

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	spiPinStruct.csPORT = GPIOB;
	spiPinStruct.csPIN = GPIO_Pin_5;

	spiPinStruct.clkPORT = GPIOB;
	spiPinStruct.clkPIN = GPIO_Pin_4;

	spiPinStruct.siPORT = GPIOC;
	spiPinStruct.siPIN = GPIO_Pin_10;

	spiPinStruct.soPORT = GPIOC;
	spiPinStruct.soPIN = GPIO_Pin_11;

	spiFlash->init(spiPinStruct);

	SevenSeg->GpioIO.A.Port = GPIOC; //
	SevenSeg->GpioIO.B.Port = GPIOC; //
	SevenSeg->GpioIO.C.Port = GPIOC; //
	SevenSeg->GpioIO.D.Port = GPIOC; //
	SevenSeg->GpioIO.E.Port = GPIOC; //
	SevenSeg->GpioIO.F.Port = GPIOB; //
	SevenSeg->GpioIO.G.Port = GPIOB; //

	SevenSeg->GpioIO.DP.Port = GPIOC; //
	SevenSeg->GpioIO.SG.Port = GPIOB; //

	SevenSeg->GpioIO.A.Pin = GPIO_Pin_13; //
	SevenSeg->GpioIO.B.Pin = GPIO_Pin_14; //
	SevenSeg->GpioIO.C.Pin = GPIO_Pin_15; //
	SevenSeg->GpioIO.D.Pin = GPIO_Pin_4; //
	SevenSeg->GpioIO.E.Pin = GPIO_Pin_5; //
	SevenSeg->GpioIO.F.Pin = GPIO_Pin_0; //
	SevenSeg->GpioIO.G.Pin = GPIO_Pin_1; //
	SevenSeg->GpioIO.DP.Pin = GPIO_Pin_8; //
	SevenSeg->GpioIO.SG.Pin = GPIO_Pin_12; //

	SevenSeg->Digit[0].Port = GPIOB; //
	SevenSeg->Digit[1].Port = GPIOB; //
	SevenSeg->Digit[2].Port = GPIOB; //
	SevenSeg->Digit[3].Port = GPIOB; //

	SevenSeg->Digit[3].Pin = GPIO_Pin_2; //
	SevenSeg->Digit[2].Pin = GPIO_Pin_10; //
	SevenSeg->Digit[1].Pin = GPIO_Pin_11; //
	SevenSeg->Digit[0].Pin = GPIO_Pin_13; //

	SevenSeg->GpioIOConfig();


#ifdef USE_COMUNICATION_LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = COMUNICATION_LED_PIN;
	GPIO_Init(COMUNICATION_LED_PORT, &GPIO_InitStructure);
#endif


	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_CommonInitStruct.ADC_Mode = ADC_DualMode_RegSimult;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);

	//ADC1
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);


	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);


	//ADC2
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC2, &ADC_InitStructure);


	ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_3Cycles);

//	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &aADCDualConvertedValue;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t) 0x40012308);
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 2;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	/* DMA2_Stream0 enable */
	//DMA_Cmd(DMA2_Stream0, ENABLE);

	//ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_SoftwareStartConv(ADC1);

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

void pinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // GPIO_F
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // GPIO_F
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // GPIO_F
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // GPIO_F

	/* Address pin INPUT (ground control)
	 PA9  = adres1
	 PA10 = adres2
	 PA11 = adres3
	 PA12 = adres4
	 PA15 = adres5
	 PC10 = adres6
	 PC11 = adres7
	 *PC12 = adres8
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Fault pin INPUT
	 PC13 = FAULT_1_iso
	 PC14 = FAULT_2_iso
	 PC15 = FAULT_3_iso
	 PC0  = FAULT_4_iso
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* State pin INPUT
	 PC1 = 1_open
	 PC2 = 1_close
	 PC3 = 2_open
	 PA0 = 2_close
	 PA1 = 3_open
	 PA2 = 3_close
	 PA3 = 4_open
	 PC4 = 4_close
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* State Power pin INPUT
	 PC5 = POWER_1_OK_iso
	 PB0 = POWER_2_OK_iso
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* State Reserved pin INPUT
	 PB15 = res_in_1
	 PC8  = res_in_2
	 PC9  = res_in_3
	 PA8  = res_in_4
	 PD2  = res_in_5
	 PB3  = res_in_6
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*Relay pin OUTPUT
	 PB1 = rel_1
	 PB2 = rel_2
	 PB10 = rel_3
	 PB11 = rel_4
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*Reserved pin OUTPUT
	 PB12 = res_rel_1
	 PB13 = res_rel_2
	 PB14 = res_rel_3
	 PB6  = res_rel_4
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*Led Comunication pin OUTPUT
	 PB5  = LedComunication
	 */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

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
bool tof(bool input, u16 delayTime)
{
	static u32 cmpVal = 0; //setpoint value
	static bool p = 0;     //one cycle var
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

