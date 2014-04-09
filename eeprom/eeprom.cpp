#include "user_conf.h"

#include <cstring>
eeprom::eeprom(void){}

eeprom::eeprom(softSpi* Spi){
	spiNum = Spi;

}

uint8_t eeprom::readByte(u16 address)
{
	u8 rcvByte = 0;
	u8 tbuff[2];

	tbuff[0] = (address >> 8) & 0xFF;
	tbuff[1] = address & 0xFF;

	spiNum->CsActive();
	spiNum->pause();

	spiNum->Write(EEPROM_CMD_READ); //Command
	spiNum->Write(tbuff, 2); //Address register
	rcvByte = spiNum->Read(); //Read value

	spiNum->CsDeactive();
	spiNum->pause();

	return rcvByte;
}

float eeprom::readFloat(uint16_t address){

	u8 float_bytes[sizeof(float)];
	float retval;


		for (u8 i = 0; i < sizeof(float); i++)
		{
			float_bytes[i] = readByte(address++);
		}

		memcpy(&retval, float_bytes, sizeof(float));

		return retval;



}





void eeprom::writeByte(uint16_t address, uint8_t data)
{
	u8 tbuff[2];

	tbuff[0] = (address >> 8) & 0xFF;
	tbuff[1] = address & 0xFF;

	spiNum->CsActive();
	spiNum->pause();
	spiNum->Write(EEPROM_CMD_WREN);

	spiNum->CsDeactive();
	spiNum->pause();

	spiNum->CsActive();
	spiNum->Write(EEPROM_CMD_WRITE);
	spiNum->Write(tbuff, 2); //Address register
	spiNum->Write(data);

	spiNum->CsDeactive();
	spiNum->pause();
}

void eeprom::writeFloat(uint16_t address, float data)
{

	u8 float_bytes[sizeof(float)];
	memcpy(float_bytes, &data, sizeof(float));

	for (u8 i = 0; i < sizeof(float); i++)
	{
		writeByte(address++, float_bytes[i]);
	}


}

void eeprom::init()
{

	spiNum->CsActive();
	spiNum->pause();

	spiNum->Write(EEPROM_CMD_WREN); //WREN

	spiNum->CsDeactive();
	spiNum->pause();

}
