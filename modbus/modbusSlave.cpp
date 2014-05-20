#include "modbusSlave.h"
#include "main.h"
#include "stm32f4xx_conf.h"
#include "user_conf.h"

extern u8 uart_buffer[UART_BUFFER_SIZE];
extern u16 uart_byte_cnt;

modbusSlave::modbusSlave()
{
	slaveAddress = 255;
}

uint16_t modbusSlave::crc(uint8_t *buf, uint8_t start, uint8_t cnt)
{
	uint8_t i, j;
	unsigned temp, temp2, flag;

	temp = 0xFFFF;

	for (i = start; i < cnt; i++)
	{
		temp = temp ^ buf[i];

		for (j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp = temp >> 1;
			if (flag)
				temp = temp ^ 0xA001;
		}
	}

	/* Reverse byte order. */
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;

	return (temp);
}

/***********************************************************************
 *
 * 	The following functions construct the required query into
 * 	a modbus query packet.
 *
 ***********************************************************************/

/*
 * Start of the packet of a read_holding_register response
 */
void modbusSlave::build_read_packet(uint8_t function, uint8_t count, uint8_t *packet)
{
	packet[SLAVE] = slaveAddress;
	packet[FUNC] = function;
	packet[2] = count * 2;
}

/*
 * Start of the packet of a preset_multiple_register response
 */
void modbusSlave::build_write_packet(uint8_t function, uint16_t start_addr, uint8_t count, uint8_t *packet)
{
	packet[SLAVE] = slaveAddress;
	packet[FUNC] = function;
	packet[START_H] = start_addr >> 8;
	packet[START_L] = start_addr & 0x00ff;
	packet[REGS_H] = 0x00;
	packet[REGS_L] = count;
}

/*
 * Start of the packet of a write_single_register response
 */
void modbusSlave::build_write_single_packet(uint8_t function, uint16_t write_addr, uint16_t reg_val, uint8_t* packet)
{
	packet[SLAVE] = slaveAddress;
	packet[FUNC] = function;
	packet[START_H] = write_addr >> 8;
	packet[START_L] = write_addr & 0x00ff;
	packet[REGS_H] = reg_val >> 8;
	packet[REGS_L] = reg_val & 0x00ff;
}

/*
 * Start of the packet of an exception response
 */
void modbusSlave::build_error_packet(uint8_t function, uint8_t exception, uint8_t *packet)
{
	packet[SLAVE] = slaveAddress;
	packet[FUNC] = function + 0x80;
	packet[2] = exception;
}

/*************************************************************************
 *
 * modbus_query( packet, length)
 *
 * Function to add a checksum to the end of a packet.
 * Please note that the packet array must be at least 2 fields longer than
 * string_length.
 **************************************************************************/

void modbusSlave::modbus_reply(uint8_t *packet, uint8_t string_length)
{
	int16_t temp_crc;

	temp_crc = crc(packet, 0, string_length);
	packet[string_length] = temp_crc >> 8;
	string_length++;
	packet[string_length] = temp_crc & 0x00FF;
}

/***********************************************************************
 *
 * send_reply( query_string, query_length )
 *
 * Function to send a reply to a modbus master.
 * Returns: total number of int8_tacters sent
 ************************************************************************/

int16_t modbusSlave::send_reply(uint8_t *query, uint8_t string_length)
{
	//uint8_t i;

	modbus_reply(query, string_length);
	string_length += 2;

	send_USART_dma(query, string_length);
//	for (i = 0; i < string_length; i++)
//	{
//		MODBUS_USART->DR = query[i] & 0x01FF;
//		while (!USART_GetFlagStatus(MODBUS_USART, USART_SR_TXE))
//		{
//		}
//	}

	return 0; /* it does not mean that the write was succesful, though */
}

/*********************************************************************
 *
 * 	modbus_request(request_data_array)
 *
 * Function to the correct request is returned and that the checksum
 * is correct.
 *
 * Returns:	string_length if OK
 * 		0 if failed
 * 		Less than 0 for exception errors
 *
 * 	Note: All functions used for sending or receiving data via
 * 	      modbus return these return values.
 *
 **********************************************************************/

int16_t modbusSlave::modbus_request(uint8_t *data)
{
	int16_t response_length;
	uint16_t crc_calc = 0;
	uint16_t crc_received = 0;

	response_length = uart_byte_cnt;

	if (response_length > 0)
	{
		crc_calc = crc(data, 0, response_length - 2);
		crc_received = data[response_length - 2];
		crc_received = (unsigned) crc_received << 8;
		crc_received = crc_received | (unsigned) data[response_length - 1];

		/*********** check CRC of response ************/
		if (crc_calc != crc_received)
		{
			return NO_REPLY;
		}

		/* check for slave id */
		if (slaveAddress != data[SLAVE])
		{
			return NO_REPLY;
		}
	}
	return (response_length);
}

/*********************************************************************
 *
 * 	validate_request(request_data_array, request_length, available_regs)
 *
 * Function to check that the request can be processed by the slave.
 *
 * Returns:	0 if OK
 * 		A negative exception code on error
 *
 **********************************************************************/

int16_t modbusSlave::validate_request(uint8_t *data, uint8_t length, uint16_t regs_size)
{
	uint16_t i, fcnt = 0;
	uint16_t regs_num = 0;
	uint16_t start_addr = 0;
	uint8_t max_regs_num;

	/* check function code */
	for (i = 0; i < sizeof(fsupported); i++)
	{
		if (fsupported[i] == data[FUNC])
		{
			fcnt = 1;
			break;
		}
	}
	if (0 == fcnt)
		return EXC_FUNC_CODE;

	if (FC_WRITE_REG == data[FUNC])
	{
		/* For function write single reg, this is the target reg.*/
		regs_num = ((int) data[START_H] << 8) + (int) data[START_L];
		if (regs_num >= regs_size)
			return EXC_ADDR_RANGE;
		return 0;
	}

	/* For functions read/write regs, this is the range. */
	regs_num = ((int) data[REGS_H] << 8) + (int) data[REGS_L];

	/* check quantity of registers */
	if (FC_READ_REGS == data[FUNC])
		max_regs_num = MAX_READ_REGS;
	else if (FC_WRITE_REGS == data[FUNC])
		max_regs_num = MAX_WRITE_REGS;

	if ((regs_num < 1) || (regs_num > max_regs_num))
		return EXC_REGS_QUANT;

	/* check registers range, start address is 0 */
	start_addr = ((int) data[START_H] << 8) + (int) data[START_L];
	if ((start_addr + regs_num) > regs_size)
		return EXC_ADDR_RANGE;

	return 0; /* OK, no exception */
}

/************************************************************************
 *
 * 	write_regs(first_register, data_array, registers_array)
 *
 * 	writes into the slave's holding registers the data in query,
 * starting at start_addr.
 *
 * Returns:   the number of registers written
 ************************************************************************/

int16_t modbusSlave::write_regs(uint16_t start_addr, uint8_t *query, u16 **regs)
{
	int16_t temp;
	uint16_t i;

	for (i = 0; i < query[REGS_L]; i++)
	{
		/* shift reg hi_byte to temp */
		temp = (int) query[(BYTE_CNT + 1) + i * 2] << 8;
		/* OR with lo_byte           */
		temp = temp | (int) query[(BYTE_CNT + 2) + i * 2];

		*regs[start_addr + i] = temp;
	}
	return i;
}

/************************************************************************
 *
 * 	preset_multiple_registers(first_register, number_of_registers,
 * data_array, registers_array)
 *
 * 	Write the data from an array into the holding registers of the slave.
 *
 *************************************************************************/

int16_t modbusSlave::preset_multiple_registers(uint16_t start_addr, uint8_t count, uint8_t *query, u16 **regs)
{
	uint8_t function = FC_WRITE_REGS; /* Preset Multiple Registers */
	int16_t status = 0;
	uint8_t packet[RESPONSE_SIZE + CHECKSUM_SIZE];

	build_write_packet(function, start_addr, count, packet);

	if (write_regs(start_addr, query, regs))
	{

		status = send_reply(packet, RESPONSE_SIZE);

	}

	if (act == 0)
	{
		act=1;
		mbToFlashData.mbAddr = start_addr;
		mbToFlashData.mbValue = ItoF(regs, start_addr);

	}

	return (status);
}

/************************************************************************
 *
 * write_single_register(slave_id, write_addr, data_array, registers_array)
 *
 * Write a single int16_t val into a single holding register of the slave.
 *
 *************************************************************************/

int16_t modbusSlave::write_single_register(uint16_t write_addr, uint8_t *query, u16 **regs)
{
	uint8_t function = FC_WRITE_REG; /* Function: Write Single Register */
	int16_t status = 0;
	uint16_t reg_val;
	uint8_t packet[RESPONSE_SIZE + CHECKSUM_SIZE];

	reg_val = query[REGS_H] << 8 | query[REGS_L];
	build_write_single_packet(function, write_addr, reg_val, packet);
	*regs[write_addr] = (int) reg_val;
	/*
	 written.start_addr=write_addr;
	 written.num_regs=1;
	 */
	status = send_reply(packet, RESPONSE_SIZE);

	return (status);
}

/************************************************************************
 *
 * 	read_holding_registers(first_register, number_of_registers,
 * registers_array)
 *
 * reads the slave's holdings registers and sends them to the Modbus master
 *
 *************************************************************************/

int16_t modbusSlave::read_holding_registers(uint16_t start_addr, uint8_t reg_count, u16 **regs)
{
	uint8_t function = FC_READ_REGS; /* Read Holding Registers */
	int16_t packet_size = 3;
	int16_t status;
	uint16_t i;
	uint8_t packet[MAX_MESSAGE_LENGTH];

	build_read_packet(function, reg_count, packet);

	for (i = start_addr; i < (start_addr + (uint16_t) reg_count); i++)
	{
		packet[packet_size] = *regs[i] >> 8;
		packet_size++;
		packet[packet_size] = *regs[i] & 0x00FF;
		packet_size++;
	}

	status = send_reply(packet, packet_size);

	return (status);
}

/*
 * configure(slave)
 *
 * sets the communication parameters for of the serial line.
 *
 * slave: identification number of the slave in the Modbus network (1 to 127)
 */

void modbusSlave::configureAddress(uint8_t slaveNum)
{
	slaveAddress = slaveNum;

}

/*
 * update(regs, regs_size)
 *
 * checks if there is any valid request from the modbus master. If there is,
 * performs the requested action
 *
 * regs: an array with the holding registers. They start at address 1 (master point16_t of view)
 * regs_size: total number of holding registers.
 * returns: 0 if no request from master,
 * 	NO_REPLY (-1) if no reply is sent to the master
 * 	an exception code (1 to 4) in case of a modbus exceptions
 * 	the number of bytes sent as reply ( > 4) if OK.
 */

int16_t modbusSlave::update(u16 **regs, u16 regs_size)
{
	uint8_t errpacket[EXCEPTION_SIZE + CHECKSUM_SIZE];
	uint16_t start_addr;
	int16_t exception;
	int8_t length;

	length = modbus_request(uart_buffer);
	if (length < 1)
	{
		return length;
	}

	exception = validate_request(uart_buffer, length, regs_size);

	if (exception)
	{
		build_error_packet(uart_buffer[FUNC], exception, errpacket);
		send_reply(errpacket, EXCEPTION_SIZE);
		return (exception);
	}

#ifdef USE_COMUNICATION_LED
	//COMUNICATION_LED_PORT->BSRRL = COMUNICATION_LED_PIN;
	COMUNICATION_LED_PORT->ODR ^= COMUNICATION_LED_PIN;
#endif
	start_addr = ((int) uart_buffer[START_H] << 8) + (int) uart_buffer[START_L];

	switch (uart_buffer[FUNC])
	{
	case FC_READ_REGS:

		return read_holding_registers(start_addr, uart_buffer[REGS_L], regs);
		break;
	case FC_WRITE_REGS:

		return preset_multiple_registers(start_addr, uart_buffer[REGS_L], uart_buffer, regs);
		break;
	case FC_WRITE_REG:

		write_single_register(start_addr, uart_buffer, regs);
		break;
	}

	return 0;
}

