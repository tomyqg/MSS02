#include "user_conf.h"
#include "stm32f4xx_conf.h"

#define FALSE 0
#define TRUE 1

extern u8 uart_buffer[UART_BUFFER_SIZE]; // uart temporary buffer
extern u16 uart_byte_cnt;                // uart receive byte counter

u8 profibus_status;

u16 uart_transmit_cnt = 0;

// Profibus flags and variables

u8 diagnose_status_1;
u8 slave_addr;
u8 master_addr;
u8 group;

#if (OUTPUT_DATA_SIZE > 0)
extern u8 profibus_out_table[OUTPUT_DATA_SIZE];
#endif
#if (INPUT_DATA_SIZE > 0)
extern u8 profibus_in_table[INPUT_DATA_SIZE];
#endif
#if (USER_PARA_SIZE > 0)
u8 User_Para[USER_PARA_SIZE];
#endif
#if (EXT_DIAG_DATA_SIZE > 0)
u8 Diag_Data[EXT_DIAG_DATA_SIZE];
#endif
#if (VENDOR_DATA_SIZE > 0)
u8 Vendor_Data[VENDOR_DATA_SIZE];
#endif

u8 User_Para_size;
u8 Input_Data_size;
u8 Output_Data_size;

u8 Module_cnt;
u8 Module_Data_size[MODULE_CNT][2]; // [][0] = = Number Corridors, [], [1] = Number
u8 Vendor_Data_size; //Number of read-bytes Vendor-specific

void initProfibus(void)
{

	u8 cnt; // cycle var

	// Variable initialization
	profibus_status = PROFIBUS_WAIT_SYN; //Current status -> wait sync
	diagnose_status_1 = STATION_NOT_READY_; //Current diag -> not ready
	Input_Data_size = 0;
	Output_Data_size = 0;
	User_Para_size = 0;
	Vendor_Data_size = 0;
	group = 0;

	//Set slave address
	slave_addr = 1; //Set Slave Address

	// If slave address not set or bad
	if ((slave_addr == 0) || (slave_addr > 126))
		slave_addr = DEFAULT_ADD;

	// Init output register
#if (OUTPUT_DATA_SIZE > 0)
	for (cnt = 0; cnt < OUTPUT_DATA_SIZE; cnt++)
	{
		profibus_out_table[cnt] = 0xFF;
	}
#endif

	// Init input register
#if (INPUT_DATA_SIZE > 0)
	for (cnt = 0; cnt < INPUT_DATA_SIZE; cnt++)
	{
		profibus_in_table[cnt] = 0x00;
	}
#endif

	// Init parametr register
#if (USER_PARA_SIZE > 0)
	for (cnt = 0; cnt < USER_PARA_SIZE; cnt++)
	{
		User_Para[cnt] = 0x00;
	}
#endif

	// Init diag register
#if (DIAG_DATA_SIZE > 0)
	for (cnt = 0; cnt < DIAG_DATA_SIZE; cnt++)
	{
		Diag_Data[cnt] = 0x00;
	}
#endif

} //end of init_Profibus

void profibus_RX(void)
{
	u8 cnt;
	u8 telegramm_type;
	u8 process_data;

	// Profibus data types
	u8 destination_add;
	u8 source_add;
	u8 function_code;
	u8 FCS_data; // Frame Check Sequence
	u8 PDU_size; // PDU size
	u8 DSAP_data; // SAP destination
	u8 SSAP_data; // SAP source

	process_data = FALSE;

	telegramm_type = uart_buffer[0];

	switch (telegramm_type)
	{
	case SD1: // Telegram without data, max. 6 bytes

		if (uart_byte_cnt != 6)
			break;

		destination_add = uart_buffer[1];
		source_add = uart_buffer[2];
		function_code = uart_buffer[3];
		FCS_data = uart_buffer[4];

		if (addmatch(destination_add) == FALSE)
			break;
		if (checksum(&uart_buffer[1], 3) != FCS_data)
			break;

		process_data = TRUE;

		break;

	case SD2: //Telegram with variable data length

		if (uart_byte_cnt != uart_buffer[1] + 6)
			break;

		PDU_size = uart_buffer[1]; // DA+SA+FC+Nutzdaten
		destination_add = uart_buffer[4];
		source_add = uart_buffer[5];
		function_code = uart_buffer[6];
		FCS_data = uart_buffer[PDU_size + 4];

		if (addmatch(destination_add) == FALSE)
			break;
		if (checksum(&uart_buffer[4], PDU_size) != FCS_data)
			break;

		process_data = TRUE;

		break;

	case SD3: //Message with 5 bytes of data, max. 11 bytes

		if (uart_byte_cnt != 11)
			break;

		PDU_size = 8; //DA+SA+FC+Nutzdaten
		destination_add = uart_buffer[1];
		source_add = uart_buffer[2];
		function_code = uart_buffer[3];
		FCS_data = uart_buffer[9];

		if (addmatch(destination_add) == FALSE)
			break;
		if (checksum(&uart_buffer[1], 8) != FCS_data)
			break;

		process_data = TRUE;

		break;

	case SD4: //Token with 3 bytes of data

		if (uart_byte_cnt != 3)
			break;

		destination_add = uart_buffer[1];
		source_add = uart_buffer[2];

		if (addmatch(destination_add) == FALSE)
			break;

		break;

	default:

		break;

	} // Switch Ende

	// Only evaluate if data are OK
	if (process_data == TRUE)
	{
		master_addr = source_add; // Master Adresse ist Source Adresse

		// Service Access Point detected?
		if ((destination_add & 0x80) && (source_add & 0x80))
		{
			DSAP_data = uart_buffer[7];
			SSAP_data = uart_buffer[8];

			// Ablauf Reboot:
			// 1) SSAP 62 -> DSAP 60 (Get Diagnostics Request)
			// 2) SSAP 62 -> DSAP 61 (Set Parameters Request)
			// 3) SSAP 62 -> DSAP 62 (Check Config Request)
			// 4) SSAP 62 -> DSAP 60 (Get Diagnostics Request)
			// 5) Data Exchange Request (normaler Zyklus)

			// Siehe Felser 8/2009 Kap. 4.1

			switch (DSAP_data)
			{
			case SAP_SET_SLAVE_ADR: // Set Slave Address (SSAP 62 -> DSAP 55)

				// Siehe Felser 8/2009 Kap. 4.2

				// Nur im Zustand "Wait Parameter" (WPRM) moeglich

				//new_addr = uart_buffer[9];
				//IDENT_HIGH_BYTE = uart_buffer[10];
				//IDENT_LOW_BYTE = uart_buffer[11];
				//if (uart_buffer[12] & 0x01) adress_aenderung_sperren = TRUE;

				profibus_send_CMD(SC, 0, SAP_OFFSET, &uart_buffer[0], 0);

				break;

			case SAP_GLOBAL_CONTROL: // Global Control Request (SSAP 62 -> DSAP 58)

				// Siehe Felser 8/2009 Kap. 4.6.2

				//If "Clear Data" high, then the PLC CPU to "Stop"
				if (uart_buffer[9] & CLEAR_DATA_)
				{
					// LED_ERROR_AN;  // Status "PLC not ready"
				}
				else
				{
					// LED_ERROR_AUS; // Status "SPS OK"
				}

				// calculate group
				for (cnt = 0; uart_buffer[10] != 0; cnt++)
					uart_buffer[10] >>= 1;

				// If command for us is
				if (cnt == group)
				{
					if (uart_buffer[9] & UNFREEZE_)
					{
						// Delete FREEZE state
					}
					else if (uart_buffer[9] & UNSYNC_)
					{
						//Delete SYNC state
					}
					else if (uart_buffer[9] & FREEZE_)
					{
						// No longer Reread Corridors
					}
					else if (uart_buffer[9] & SYNC_)
					{
						// Outputs set only when SYNC command
					}
				}

				break;

			case SAP_SLAVE_DIAGNOSIS: // Get Diagnostics Request (SSAP 62 -> DSAP 60)

				// Siehe Felser 8/2009 Kap. 4.5.2

				// After receiving the diagnosis of the DP slave goes from state
				// "Power on Reset" (POR) in the state of "Wait parameters" (WPRM)
				// At the end of initialization (the "Data Exchange" (DXCHG))
				// The master sends a second time a request to the Diagnostics
				//Check / correct configuration

				if (function_code == (REQUEST_ + FCB_ + SRD_HIGH))
				{
					//Getting a diagnosis query (call telegram)

					//uart_buffer[4]  = master_addr;                  // Target master (with SAP offset)
					//uart_buffer[5]  = slave_addr + SAP_OFFSET;      // Source slave (with SAP offset)
					//uart_buffer[6]  = SLAVE_DATA;
					uart_buffer[7] = SSAP_data; // Target SAP Master
					uart_buffer[8] = DSAP_data; // Source SAP Slave
					uart_buffer[9] = diagnose_status_1; // Status 1
					uart_buffer[10] = STATUS_2_DEFAULT + PRM_REQ_; // Status 2
					uart_buffer[11] = DIAG_SIZE_OK; // Status 3
					uart_buffer[12] = MASTER_ADD_DEFAULT; // address Master
					uart_buffer[13] = IDENT_HIGH_BYTE; // Ident high
					uart_buffer[14] = IDENT_LOW_BYTE; // Ident low
#if (EXT_DIAG_DATA_SIZE > 0)
							uart_buffer[15] = EXT_DIAG_GERAET+EXT_DIAG_DATA_SIZE+1; // Diagnosis (type and number of bytes)
							for (cnt = 0; cnt < EXT_DIAG_DATA_SIZE; cnt++)
							{
								uart_buffer[16+cnt] = Diag_Data[cnt];
							}

							profibus_send_CMD(SD2, DATA_LOW, SAP_OFFSET, &uart_buffer[7], 9 + EXT_DIAG_DATA_SIZE);
#else

					profibus_send_CMD(SD2, DATA_LOW, SAP_OFFSET, &uart_buffer[7], 8);
#endif

				}
				else if (function_code == (REQUEST_ + FCV_ + SRD_HIGH) || function_code == (REQUEST_ + FCV_ + FCB_ + SRD_HIGH))
				{

					// Diagnostic query to check data from Check Config Request
					// Diagnostic request slave diagnostics if requested query

					//uart_buffer [4] = master_addr; //Target master (with SAP offset)
					//uart_buffer [5] = slave_addr; // SAP_OFFSET Source slave (with SAP offset)
					//uart_buffer [6] = SLAVE_DATA;
					uart_buffer[7] = SSAP_data; // Target SAP Master
					uart_buffer[8] = DSAP_data; // Source SAP Slave
					uart_buffer[9] = diagnose_status_1; // Status 1
					uart_buffer[10] = STATUS_2_DEFAULT; // Status 2
					uart_buffer[11] = DIAG_SIZE_OK; // Status 3
					uart_buffer[12] = master_addr - SAP_OFFSET; // Adresse Master
					uart_buffer[13] = IDENT_HIGH_BYTE; // Ident high
					uart_buffer[14] = IDENT_LOW_BYTE; // Ident low
#if (EXT_DIAG_DATA_SIZE > 0)
							uart_buffer[15] = EXT_DIAG_GERAET+EXT_DIAG_DATA_SIZE+1; // Diagnose (Typ und Anzahl Bytes)
							for (cnt = 0; cnt < EXT_DIAG_DATA_SIZE; cnt++)
							{
								uart_buffer[16+cnt] = Diag_Data[cnt];
							}

							profibus_send_CMD(SD2, DATA_LOW, SAP_OFFSET, &uart_buffer[7], 9 + EXT_DIAG_DATA_SIZE);
#else

					profibus_send_CMD(SD2, DATA_LOW, SAP_OFFSET, &uart_buffer[7], 8);
#endif
				}

				break;

			case SAP_SET_PRM: // Set Parameters Request (SSAP 62 -> 61 DSAP)

				// Siehe Felser 8/2009 Kap. 4.3.1

				// After obtaining the parameters of the DP slave changes from the status
				// "Wait parameters" (WPRM) to the state "Wait Configuration" (WCFG)
				// Data only for our device Accept
				if ((uart_buffer[13] == IDENT_HIGH_BYTE) && (uart_buffer[14] == IDENT_LOW_BYTE))
				{
					//uart_buffer[9]  = Befehl
					//uart_buffer[10] = Watchdog 1
					//uart_buffer[11] = Watchdog 2
					//uart_buffer[12] = Min TSDR
					//uart_buffer[13] = Ident H
					//uart_buffer[14] = Ident L
					//uart_buffer[15] = Gruppe
					//uart_buffer[16] = User Parameter

					// Bei DPV1 Unterstuetzung:
					//uart_buffer[16] = DPV1 Status 1
					//uart_buffer[17] = DPV1 Status 2
					//uart_buffer[18] = DPV1 Status 3
					//uart_buffer[19] = User Parameter

					// User Parameter groeï¿½e = Laenge - DA, SA, FC, DSAP, SSAP, 7 Parameter Bytes
					User_Para_size = PDU_size - 12;

					// User Parameter einlesen
#if (USER_PARA_SIZE > 0)
					for (cnt = 0; cnt < User_Para_size; cnt++) User_Para[cnt] = uart_buffer[16+cnt];
#endif

					// Gruppe einlesen
					for (group = 0; uart_buffer[15] != 0; group++)
						uart_buffer[15] >>= 1;

					profibus_send_CMD(SC, 0, SAP_OFFSET, &uart_buffer[0], 0);
				}

				break;

			case SAP_CHK_CFG: // Check Config Request (SSAP 62 -> DSAP 62)

				// Siehe Felser 8/2009 Kap. 4.4.1

				// After obtaining the configuration of the DP slave changes from the status
				// "Wait Configuration" (WCFG) to the state "Data Exchange" (DXCHG)

				// IO configuration:
				// Compact size for max. 16/32 byte IO
				// Special format for max. 64/132 byte IO

				Module_cnt = 0;
				Vendor_Data_size = 0;

				// File size evaluate multiple bytes, depending on the PDU
				// LE / LEr - (DA + SA + FC + DSAP + SSAP) = number of bytes Config
				for (cnt = 0; cnt < uart_buffer[1] - 5; cnt++)
				{
					switch (uart_buffer[9 + cnt] & CFG_DIRECTION_)
					{
					case CFG_INPUT:

						//Input_Data_size = (uart_buffer[9+cnt] & CFG_BYTE_CNT_) + 1;
						//if (uart_buffer[9+cnt] & CFG_WIDTH_ & CFG_WORD)
						//  Input_Data_size = Input_Data_size*2;

						Module_Data_size[Module_cnt][0] = (uart_buffer[9 + cnt] & CFG_BYTE_CNT_) + 1;

						if (uart_buffer[9 + cnt] & CFG_WIDTH_ & CFG_WORD)
							Module_Data_size[Module_cnt][0] = Module_Data_size[Module_cnt][0] * 2;

						Module_cnt++;

						break;

					case CFG_OUTPUT:

						//Output_Data_size = (uart_buffer[9+cnt] & CFG_BYTE_CNT_) + 1;
						//if (uart_buffer[9+cnt] & CFG_WIDTH_ & CFG_WORD)
						//  Output_Data_size = Output_Data_size*2;

						Module_Data_size[Module_cnt][1] = (uart_buffer[9 + cnt] & CFG_BYTE_CNT_) + 1;
						if (uart_buffer[9 + cnt] & CFG_WIDTH_ & CFG_WORD)
							Module_Data_size[Module_cnt][1] = Module_Data_size[Module_cnt][1] * 2;

						Module_cnt++;

						break;

					case CFG_INPUT_OUTPUT:

						//Input_Data_size = (uart_buffer[9+cnt] & CFG_BYTE_CNT_) + 1;
						//Output_Data_size = (uart_buffer[9+cnt] & CFG_BYTE_CNT_) + 1;
						//if (uart_buffer[9+cnt] & CFG_WIDTH_ & CFG_WORD)
						//{
						//  Input_Data_size = Input_Data_size*2;
						//  Output_Data_size = Output_Data_size*2;
						//}

						Module_Data_size[Module_cnt][0] = (uart_buffer[9 + cnt] & CFG_BYTE_CNT_) + 1;
						Module_Data_size[Module_cnt][1] = (uart_buffer[9 + cnt] & CFG_BYTE_CNT_) + 1;

						if (uart_buffer[9 + cnt] & CFG_WIDTH_ & CFG_WORD)
						{
							Module_Data_size[Module_cnt][0] = Module_Data_size[Module_cnt][0] * 2;
							Module_Data_size[Module_cnt][1] = Module_Data_size[Module_cnt][1] * 2;
						}

						Module_cnt++;

						break;

					case CFG_SPECIAL:

						// Special format

						// Manufacturer-specific bytes available?
						if (uart_buffer[9 + cnt] & CFG_SP_VENDOR_CNT_)
						{
							// Number of secure data Manufacturers
							Vendor_Data_size += uart_buffer[9 + cnt] & CFG_SP_VENDOR_CNT_;

							//Vendor_Data[] = uart_buffer[];

							// Subtract number of total number
							uart_buffer[1] -= uart_buffer[9 + cnt] & CFG_SP_VENDOR_CNT_;
						}

						// I / O data
						switch (uart_buffer[9 + cnt] & CFG_SP_DIRECTION_)
						{
						case CFG_SP_VOID: // Empty module (1 byte)

							Module_Data_size[Module_cnt][0] = 0;
							Module_Data_size[Module_cnt][1] = 0;

							Module_cnt++;

							break;

						case CFG_SP_INPUT: //Input (2 bytes)

							//Input_Data_size = (uart_buffer[10+cnt] & CFG_SP_BYTE_CNT_) + 1;
							//if (uart_buffer[10+cnt] & CFG_WIDTH_ & CFG_WORD)
							//  Input_Data_size = Input_Data_size*2;

							Module_Data_size[Module_cnt][0] = (uart_buffer[10 + cnt] & CFG_SP_BYTE_CNT_) + 1;
							if (uart_buffer[10 + cnt] & CFG_WIDTH_ & CFG_WORD)
								Module_Data_size[Module_cnt][0] = Module_Data_size[Module_cnt][0] * 2;

							Module_cnt++;

							cnt++; // Second byte we have already

							break;

						case CFG_SP_OUTPUT: // Output (2 bytes)

							//Output_Data_size = (uart_buffer[10+cnt] & CFG_SP_BYTE_CNT_) + 1;
							//if (uart_buffer[10+cnt] & CFG_WIDTH_ & CFG_WORD)
							//  Output_Data_size = Output_Data_size*2;

							Module_Data_size[Module_cnt][1] = (uart_buffer[10 + cnt] & CFG_SP_BYTE_CNT_) + 1;
							if (uart_buffer[10 + cnt] & CFG_WIDTH_ & CFG_WORD)
								Module_Data_size[Module_cnt][1] = Module_Data_size[Module_cnt][1] * 2;

							Module_cnt++;

							cnt++; // Second byte we have already

							break;

						case CFG_SP_INPUT_OPTPUT: // Ein- und Ausgang (3 Byte)

							// Only output ...
							//Output_Data_size = (uart_buffer[10+cnt] & CFG_SP_BYTE_CNT_) + 1;
							//if (uart_buffer[10+cnt] & CFG_WIDTH_ & CFG_WORD)
							//  Output_Data_size = Output_Data_size*2;

							// Then input
							//Input_Data_size = (uart_buffer[11+cnt] & CFG_SP_BYTE_CNT_) + 1;
							//if (uart_buffer[11+cnt] & CFG_WIDTH_ & CFG_WORD)
							//  Input_Data_size = Input_Data_size*2;

							// Erst Ausgang...
							Module_Data_size[Module_cnt][0] = (uart_buffer[10 + cnt] & CFG_SP_BYTE_CNT_) + 1;
							if (uart_buffer[10 + cnt] & CFG_WIDTH_ & CFG_WORD)
								Module_Data_size[Module_cnt][0] = Module_Data_size[Module_cnt][0] * 2;

							// Dann Eingang
							Module_Data_size[Module_cnt][1] = (uart_buffer[11 + cnt] & CFG_SP_BYTE_CNT_) + 1;
							if (uart_buffer[11 + cnt] & CFG_WIDTH_ & CFG_WORD)
								Module_Data_size[Module_cnt][1] = Module_Data_size[Module_cnt][1] * 2;

							Module_cnt++;

							cnt += 2; // Second and third bytes we have already

							break;

						} // Switch Ende

						break;

					default:

						//Input_Data_size = 0;
						//Output_Data_size = 0;

						break;

					} // Switch Ende
				} // For Ende

				if (Vendor_Data_size != 0)
				{
					// evaluate
				}

				// after error -> Send CFG_FAULT_ in diagnosis
#if (VENDOR_DATA_SIZE > 0)
				if (Module_cnt > MODULE_CNT || Vendor_Data_size != VENDOR_DATA_SIZE)
				diagnose_status_1 |= CFG_FAULT_;
#else
				if (Module_cnt > MODULE_CNT)
				{
					diagnose_status_1 |= CFG_FAULT_;
				}
#endif
				else
				{
					diagnose_status_1 &= ~(STATION_NOT_READY_ + CFG_FAULT_);
				}

				//Short acknowledgment
				profibus_send_CMD(SC, 0, SAP_OFFSET, &uart_buffer[0], 0);

				break;

			default:

				// Unknown SAP

				break;

			} // Switch DSAP_data end

		}

		//Goal: Slave address
		else if (destination_add == slave_addr)
		{

			// Status query
			if (function_code == (REQUEST_ + FDL_STATUS))
			{
				profibus_send_CMD(SD1, FDL_STATUS_OK, 0, &uart_buffer[0], 0);
			}

			//Master sends output data and requires input data (Send and Request Data)
			else if (function_code == (REQUEST_ + FCV_ + SRD_HIGH) || function_code == (REQUEST_ + FCV_ + FCB_ + SRD_HIGH))
			{

				// Read data from master
#if (INPUT_DATA_SIZE > 0)
				for (cnt = 0; cnt < INPUT_DATA_SIZE; cnt++)
				{
					profibus_in_table[cnt] = uart_buffer[cnt + 7];
				}
#endif

				// Write data for Master in Buffer
#if (OUTPUT_DATA_SIZE > 0)
				for (cnt = 0; cnt < OUTPUT_DATA_SIZE; cnt++)
				{
					uart_buffer[cnt + 7] = profibus_out_table[cnt];
				}
#endif

// #if (OUTPUT_DATA_SIZE > 0)
// 				if (diagnose_status_1 & EXT_DIAG_)
// 					profibus_send_CMD(SD2, DATA_HIGH, 0, &uart_buffer[7], 0); //Request diagnosis query
// 				else
// 					profibus_send_CMD(SD2, DATA_LOW, 0, &uart_buffer[7], 0); // send data
// #else
// 				if (diagnose_status_1 & EXT_DIAG_ || (get_Address() & 0x80))
// 				profibus_send_CMD(SD1, DATA_HIGH, 0, &uart_buffer[7], 0); // diagnosis query
// 				else
// 				profibus_send_CMD(SC, 0, 0, &uart_buffer[7], 0); // short acknowledgment
// #endif

				profibus_send_CMD(SD2, DATA_LOW, 0, &uart_buffer[7], OUTPUT_DATA_SIZE);

#ifdef USE_COMUNICATION_LED
				COMUNICATION_LED_PORT->BSRRH = COMUNICATION_LED_PIN;
#endif

			}
		}

	}

}

/*!
 * \brief Profibus Telegramm zusammenstellen und senden
 * \param type          Telegrammtyp (SD1, SD2 usw.)
 * \param function_code Function Code der uebermittelt werden soll
 * \param sap_offset    Wert des SAP-Offset
 * \param pdu           Pointer auf Datenfeld (PDU)
 * \param length_pdu    Laenge der reinen PDU ohne DA, SA oder FC
 */
void profibus_send_CMD(u8 type, u8 function_code, u8 sap_offset, u8 *pdu, u8 length_pdu)
{
	u8 length_data;

	switch (type)
	{
	case SD1:

		uart_buffer[0] = SD1;
		uart_buffer[1] = master_addr;
		uart_buffer[2] = slave_addr + sap_offset;
		uart_buffer[3] = function_code;
		uart_buffer[4] = checksum(&uart_buffer[1], 3);
		uart_buffer[5] = ED;

		length_data = 6;

		break;

	case SD2:

		uart_buffer[0] = SD2;
		uart_buffer[1] = length_pdu + 3; // Laenge der PDU inkl. DA, SA und FC
		uart_buffer[2] = length_pdu + 3;
		uart_buffer[3] = SD2;
		uart_buffer[4] = master_addr;
		uart_buffer[5] = slave_addr + sap_offset;
		uart_buffer[6] = function_code;

		// Daten werden vor Aufruf der Funktion schon aufgefuellt

		uart_buffer[7 + length_pdu] = checksum(&uart_buffer[4], length_pdu + 3);
		uart_buffer[8 + length_pdu] = ED;

		length_data = length_pdu + 9;

		break;

	case SD3:

		uart_buffer[0] = SD3;
		uart_buffer[1] = master_addr;
		uart_buffer[2] = slave_addr + sap_offset;
		uart_buffer[3] = function_code;

		// Daten werden vor Aufruf der Funktion schon aufgefuellt

		uart_buffer[9] = checksum(&uart_buffer[4], 8);
		uart_buffer[10] = ED;

		length_data = 11;

		break;

	case SD4:

		uart_buffer[0] = SD4;
		uart_buffer[1] = master_addr;
		uart_buffer[2] = slave_addr + sap_offset;

		length_data = 3;

		break;

	case SC:

		uart_buffer[0] = SC;

		length_data = 1;

		break;

	default:

		break;

	}

	profibus_TX(&uart_buffer[0], length_data);

}

void profibus_TX(u8 *data, u8 length)
{
	u16 i;

	// Ohne Interrupt senden
	// DEBUG
	for (i = 0; i < length; i++)
	{
		PROFIBUS_USART->DR = data[i] & 0x01FF;
		while (!USART_GetFlagStatus(PROFIBUS_USART, USART_SR_TXE))
		{
		}
	}

	PROFIBUS_TIMER->CNT = 0;
	PROFIBUS_TIMER->ARR = TIMEOUT_MAX_TX_TIME;
	profibus_status = PROFIBUS_SEND_DATA;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//Return Checksumme
u8 checksum(u8 *data, u8 length)
{
	u8 csum = 0;

	while (length--)
	{
		csum += data[length];
	}

	return csum;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//Return TRUE if the destination address is ours, FALSE if not
u8 addmatch(u8 destination)
{
	if ((destination != slave_addr) && // Slave
			(destination != slave_addr + SAP_OFFSET) && // SAP Slave
			(destination != BROADCAST_ADD) && // Broadcast
			(destination != BROADCAST_ADD + SAP_OFFSET)) // SAP Broadcast
		return FALSE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
