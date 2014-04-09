#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f4xx_conf.h"


class eeprom {
public:
	eeprom(void);
	eeprom(softSpi* Spi);
	softSpi* spiNum;




	uint8_t readByte(uint16_t address);
	float readFloat(uint16_t address);
	void writeByte(uint16_t address, uint8_t data);
	void writeFloat(uint16_t address, float data);




	void init();

private:


	static const u8 EEPROM_CMD_WREN = 0x06; //WREN Set Write Enable Latch
	static const u8 EEPROM_CMD_WRDI = 0x04; //WRDI Write Disable
	static const u8 EEPROM_CMD_RDSR = 0x05; //RDSR Read Status Register
	static const u8 EEPROM_CMD_WRSR = 0x01; //WRSR Write Status Register
	static const u8 EEPROM_CMD_READ = 0x03; //READ Read Memory Data
	static const u8 EEPROM_CMD_WRITE = 0x02; //WRITE Write Memory Data


};









#endif /* EEPROM_H */




