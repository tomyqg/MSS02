#ifndef PROFIBUS_H
#define PROFIBUS_H

#include "stm32f4xx_conf.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Ident Nummer DP Slave
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 IDENT_HIGH_BYTE = 0x80;
static const u8 IDENT_LOW_BYTE = 0x70;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Address
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 SAP_OFFSET = 0x80;  // Service Access Point Adress Offset
static const u8 BROADCAST_ADD = 127;
static const u8 DEFAULT_ADD = 126;   // Default slave address
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Delimiters
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 SD1 = 0x10;  // Request_FDL_Status (Field Data Link)
static const u8 SD2 = 0x68; // Telegram variable data units.
static const u8 SD3 = 0xA2;  // Telegram w/ fixed data units.
static const u8 SD4 = 0xDC;  // Token Telegram
static const u8 SC = 0xE5;  // Short Acknowledgement Telegram
static const u8 ED = 0x16;  // End Delimiter
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Function Code or Frame Control (FC)
///////////////////////////////////////////////////////////////////////////////////////////////////
/* FC Request */
static const u8 FDL_STATUS = 0x09;  // SPS: FDL Status With Reply
static const u8 SRD_HIGH = 0x0D;  // SPS: Send and Request Data with acknowledge
static const u8 FCV_ = 0x10;
static const u8 FCB_ = 0x20;
static const u8 REQUEST_ = 0x40;

/* FC Response */
static const u8 FDL_STATUS_OK = 0x00;  // SLA: ACK Positive
static const u8 DATA_LOW = 0x08;  // SLA: Data Low and Send Data OK
static const u8 DATA_HIGH = 0x0A;  // SLA: Data High and Send Data OK
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Service Access Points (DP Slave) MS0
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 SAP_SET_SLAVE_ADR = 55;  // Master set Slave Adresse, Slave responsed SC
static const u8 SAP_RD_INP = 56;  // Read Inputs (Rd_Inp)
static const u8 SAP_RD_OUTP = 57;  // Read Outputs (Rd_Outp)
static const u8 SAP_GLOBAL_CONTROL = 58;  // Control Commands to a DP Slave (Global_Control) Slave not response
static const u8 SAP_GET_CFG = 59;  // Read Configuration Data (Get_Cfg) Slave send Configuration
static const u8 SAP_SLAVE_DIAGNOSIS = 60;  // Read Diagnostic Data (Slave_Diagnosis) Slave send Diagnostic
//static const u8 SAP_SET_PRM = 61;

static const u8 SAP_SET_PRM = 61;  // Send Parameterization Data (Set_Prm) Slave send Parameterization
static const u8 SAP_CHK_CFG = 62;  // Check Configuration Data (Chk_Cfg) Slave send SC
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//SAP: Global Control (master data)
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 CLEAR_DATA_ = 0x02;
static const u8 UNFREEZE_ = 0x04;
static const u8 FREEZE_ = 0x08;
static const u8 UNSYNC_ = 0x10;
static const u8 SYNC_ = 0x20;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Diagnostic (Reply slave)
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Status Byte 1 */
static const u8 STATUS_1_DEFAULT = 0x00;
static const u8 STATION_NOT_EXISTENT_ = 0x01;
static const u8 STATION_NOT_READY_ = 0x02;
static const u8 CFG_FAULT_ = 0x04;
static const u8 EXT_DIAG_ = 0x08; // Advanced diagnostics available
static const u8 NOT_SUPPORTED_ = 0x10;
static const u8 INV_SLAVE_RESPONSE_ = 0x20;
static const u8 PRM_FAULT_ = 0x40;
static const u8 MASTER_LOCK = 0x80;

/* Status Byte 2 */
static const u8 STATUS_2_DEFAULT = 0x04;
static const u8 PRM_REQ_ = 0x01;
static const u8 STAT_DIAG_ = 0x02;
static const u8 WD_ON_ = 0x08;
static const u8 FREEZE_MODE_ = 0x10;
static const u8 SYNC_MODE_ = 0x20;
//#define free                  = 0x40
static const u8 DEACTIVATED_ = 0x80;

/* Status Byte 3 */
static const u8 DIAG_SIZE_OK = 0x00;
static const u8 DIAG_SIZE_ERROR = 0x80;

/* Adress */
static const u8 MASTER_ADD_DEFAULT = 0x02;

/* Advanced Diagnostics (EXT_DIAG_ = 1) */
static const u8 EXT_DIAG_TYPE_ = 0xC0;  // Bit 6-7 is diagnosed with type
static const u8 EXT_DIAG_BYTE_CNT_ = 0x3F;  // Bit 0-5 are number of diagnostic bytes

static const u8 EXT_DIAG_GERAET = 0x00;  // If bit 7 and 6 = 00, then devices Based
static const u8 EXT_DIAG_KENNUNG = 0x40;  // If bit 7 and 6 = 01, then ID Based
static const u8 EXT_DIAG_KANAL = 0x80;  // If bit 7 and 6 = 10, then channel relation
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Set Parameters Request (Daten Master)
///////////////////////////////////////////////////////////////////////////////////////////////////
/* Befehl */
static const u8 LOCK_SLAVE_ = 0x80;  // Slave locked for other masters
static const u8 UNLOCK_SLAVE_ = 0x40;  // Slave released for other masters
static const u8 ACTIVATE_SYNC_ = 0x20;
static const u8 ACTIVATE_FREEZE_ = 0x10;
static const u8 ACTIVATE_WATCHDOG_ = 0x08;

/* DPV1 Status Byte 1 */
static const u8 DPV1_MODE_ = 0x80;
static const u8 FAIL_SAVE_MODE_ = 0x40;
static const u8 PUBLISHER_MODE_ = 0x20;
static const u8 WATCHDOG_TB_1MS = 0x04;

/* DPV1 Status Byte 2 */
static const u8 PULL_PLUG_ALARM_ = 0x80;
static const u8 PROZESS_ALARM_ = 0x40;
static const u8 DIAGNOSE_ALARM_ = 0x20;
static const u8 VENDOR_ALARM_ = 0x10;
static const u8 STATUS_ALARM_ = 0x08;
static const u8 UPDATE_ALARM_ = 0x04;
static const u8 CHECK_CONFIG_MODE_ = 0x01;

/* DPV1 Status Byte 3 */
static const u8 PARAMETER_CMD_ON_ = 0x80;
static const u8 ISOCHRON_MODE_ON_ = 0x10;
static const u8 PARAMETER_BLOCK_ = 0x08;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// SAP: Check Config Request (Daten Master)
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 CFG_DIRECTION_ = 0x30;  // Bit 4-5 is the direction. 01 = input, output = 10, 11 = input / output
static const u8 CFG_INPUT = 0x10;  // Input
static const u8 CFG_OUTPUT = 0x20;  // Output
static const u8 CFG_INPUT_OUTPUT = 0x30;  // Input/Output
static const u8 CFG_SPECIAL = 0x00;  // Special format if more than 16/32 bytes are to be moved over

static const u8 CFG_KONSISTENZ_ = 0x80;  // Bit 7 is consistency. 0 = byte or word, 1 = About entire module
static const u8 CFG_KONS_BYTE_WORT = 0x00;  // Byte or Word
static const u8 CFG_KONS_MODUL = 0x80;  // Modul

static const u8 CFG_WIDTH_ = 0x40;  // Bit 6 is IO width 0 = byte (8 bits), 1 = Word (16bit)
static const u8 CFG_BYTE = 0x00;  // Byte
static const u8 CFG_WORD = 0x40;  // Word

/* Compact size */
static const u8 CFG_BYTE_CNT_ = 0x0F;  // Bits 0-3 are the number of bytes or words. 0 = 1 byte, 1 = 2 bytes, etc.

/* special format */
static const u8 CFG_SP_DIRECTION_ = 0xC0;  // Bit 6-7 is the direction. 01 = input, output = 10, 11 = input / output
static const u8 CFG_SP_VOID = 0x00;  // Void
static const u8 CFG_SP_INPUT = 0x40;  // Input
static const u8 CFG_SP_OUTPUT = 0x80;  // Output
static const u8 CFG_SP_INPUT_OPTPUT = 0xC0;  // Input/Output

static const u8 CFG_SP_VENDOR_CNT_ = 0x0F;  // Bit 0-3 Number of vendor specific bytes. 0 = no

/* special format / length byte */
static const u8 CFG_SP_BYTE_CNT_ = 0x3F;  // Bits 0-5 are the number of bytes or words. 0 = 1 byte, 1 = 2 bytes, etc.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Tbit config
///////////////////////////////////////////////////////////////////////////////////////////////////

//#define DELAY_TBIT            103  // UART @ 9600 b/s = 104mks
static const u8 DELAY_TBIT = 2; // UART @ 500000 b/s = 2mks TODO use var and auto set speed
//#define DELAY_TBIT            36  // UART @ 1500000 b/s= 0.6mks
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
// Timeout constant
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 TIMEOUT_MAX_SYN_TIME = 33 * DELAY_TBIT; // 33 TBit = TSYN
static const u8 TIMEOUT_MAX_RX_TIME = 15 * DELAY_TBIT;
static const u8 TIMEOUT_MAX_TX_TIME = 15 * DELAY_TBIT;
static const u8 TIMEOUT_MAX_SDR_TIME = 15 * DELAY_TBIT; // 15 Tbit = TSDR
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Profibus control flow
///////////////////////////////////////////////////////////////////////////////////////////////////
static const u8 PROFIBUS_WAIT_SYN = 1;
static const u8 PROFIBUS_WAIT_DATA = 2;
static const u8 PROFIBUS_GET_DATA = 3;
static const u8 PROFIBUS_SEND_DATA = 4;
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
void initProfibus(void);
void profibus_RX(void);
void profibus_send_CMD(u8 type, u8 function_code, u8 sap_offset, u8 *pdu, u8 length_pdu);
void profibus_TX(u8 *data, u8 length);
unsigned char checksum(u8 *data, u8 length);
unsigned char addmatch(u8 destination);

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
