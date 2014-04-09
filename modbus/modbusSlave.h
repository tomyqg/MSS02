#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H
#include "stm32f4xx_conf.h"

enum {
	MAX_READ_REGS = 0x7D, MAX_WRITE_REGS = 0x7B, MAX_MESSAGE_LENGTH = 256
};

enum {
	RESPONSE_SIZE = 6, EXCEPTION_SIZE = 3, CHECKSUM_SIZE = 2
};

/* exceptions code */
enum {
	NO_REPLY = -1,
	EXC_FUNC_CODE = 1,
	EXC_ADDR_RANGE = 2,
	EXC_REGS_QUANT = 3,
	EXC_EXECUTE = 4
};

/* positions inside the query/response array */
enum {
	SLAVE = 0,
	FUNC,
	START_H,
	START_L,
	REGS_H,
	REGS_L,
	BYTE_CNT
};

/* enum of supported modbus function codes. If you implement a new one, put its function code here ! */
enum {
	FC_READ_REGS = 0x03, //Read contiguous block of holding register
	FC_WRITE_REG = 0x06, //Write single holding register
	FC_WRITE_REGS = 0x10 //Write block of contiguous registers
};

/* supported functions. If you implement a new one, put its function code into this array! */
const uint8_t fsupported[] = { FC_READ_REGS, FC_WRITE_REG, FC_WRITE_REGS };



class modbusSlave{



public:
	modbusSlave();

	void configureAddress(uint8_t slaveAddress);

	int16_t update(u16 **regs, u16 regs_size);

private:
	u8 slaveAddress;

	uint16_t crc(uint8_t *buf, uint8_t start, uint8_t cnt);
	void build_read_packet(uint8_t function, uint8_t count, uint8_t *packet);
	void build_write_packet(uint8_t function, uint16_t start_addr, uint8_t count, uint8_t *packet);
	void build_write_single_packet(uint8_t function, uint16_t write_addr, uint16_t reg_val, uint8_t* packet);
	void build_error_packet(uint8_t function, uint8_t exception, uint8_t *packet);
	void modbus_reply(uint8_t *packet, uint8_t string_length);
	int16_t send_reply(uint8_t *query, uint8_t string_length);

	int16_t modbus_request(uint8_t *data);
	int16_t validate_request(uint8_t *data, uint8_t length, uint16_t regs_size);
	int16_t write_regs(uint16_t start_addr, uint8_t *query, u16 **regs);
	int16_t preset_multiple_registers(uint16_t start_addr, uint8_t count, uint8_t *query, u16 **regs);
	int16_t read_holding_registers(uint16_t start_addr, uint8_t reg_count, u16 **regs);
	int16_t write_single_register(uint16_t write_addr, uint8_t *query, u16 **regs);

};



#endif
