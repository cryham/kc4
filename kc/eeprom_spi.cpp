#include "def.h"
#include "eeprom_spi.h"
#ifdef EEPROM_CS

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include <SPI.h>

#define DATAOUT 11  // MOSI spi pins
#define DATAIN  12  // MISO 
#define SPICLOCK  13  // SCK
//  serial eeprom opcodes, in 25LC256.pdf
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2


void EE_SPI_Start()
{
	pinMode(TFT_CS, OUTPUT);
	digitalWrite(TFT_CS, HIGH);
	delay(10);
	pinMode(DATAOUT, OUTPUT);
	pinMode(DATAIN, INPUT);
	pinMode(SPICLOCK,OUTPUT);

	pinMode(EEPROM_CS, OUTPUT);
	digitalWrite(EEPROM_CS, HIGH);
	SPISettings set(4*1000*1000, MSBFIRST, SPI_MODE2);
	
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	
	SPCR = (1<<SPE)|(1<<MSTR);
	byte c=SPSR;  c=SPDR;
}

void EE_SPI_End()
{
	delay(10);
}

//  read status
uint8_t EE_SPI_Status()
{
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(RDSR);
	uint8_t st = SPI.transfer(RDSR);

	digitalWrite(EEPROM_CS, HIGH);
	delay(2);

	return st;
}

//  write enable
void EE_SPI_Wren()
{
	digitalWrite(EEPROM_CS, LOW);
	SPI.transfer(WREN);
	digitalWrite(EEPROM_CS, HIGH);
	delay(5);
}

//  write
void EE_SPI_Write(uint16_t adr, uint8_t* bytes, int length)
{
	EE_SPI_Wren();
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(WRITE);
	SPI.transfer(adr >> 16);
	SPI.transfer(adr & 0xFF);
	for (int i=0; i < length; ++i, ++bytes)
	{
		SPI.transfer(*bytes);
	}

	digitalWrite(EEPROM_CS, HIGH);
	delay(length * 5);  // 5ms par?
}

//  read byte
uint8_t EE_SPI_Read(uint16_t adr)
{
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(READ);
	SPI.transfer(adr >> 16);
	SPI.transfer(adr & 0xFF);
	uint8_t read = SPI.transfer(adr & 0xFF);

	digitalWrite(EEPROM_CS, HIGH);
	delay(5);
	
	return read;
}

#endif
