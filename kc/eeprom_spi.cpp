#include "def.h"
#include "eeprom_spi.h"
#ifdef EEPROM_CS

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include <SPI.h>


#define DATAOUT  11  // MOSI spi pins
#define DATAIN   12  // MISO
#define SPICLOCK 13  // SCK

//  serial eeprom opcodes, from 25LC256.pdf
#define WRSR  1
#define WRITE 2
#define READ  3
#define WRDI  4
#define RDSR  5
#define WREN  6


// SPISettings spiSet(4*1000*1000, MSBFIRST, SPI_MODE2);

void EE_SPI_Start()
{
	delay(1);  // off LCD CS

	pinMode(TFT_CS, OUTPUT);
	digitalWrite(TFT_CS, HIGH);
	delay(1);  // off LCD CS

	pinMode(DATAOUT, OUTPUT);
	pinMode(DATAIN, INPUT);
	pinMode(SPICLOCK,OUTPUT);

	pinMode(EEPROM_CS, OUTPUT);
	digitalWrite(EEPROM_CS, HIGH);

	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV4);  //DIV2 12MHz  DIV4 4MHz
	
	// SPCR = (1<<SPE)|(1<<MSTR);
	// byte c=SPSR;  c=SPDR;
	delay(1);
}

void EE_SPI_End()
{
	delay(1);
}


//  read status
uint8_t EE_SPI_Status()
{
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(RDSR);
	uint8_t st = SPI.transfer(RDSR);

	digitalWrite(EEPROM_CS, HIGH);
	delayMicroseconds(10);
	return st;
}

//  write enable
void EE_SPI_Wren()
{
	digitalWrite(EEPROM_CS, LOW);
	SPI.transfer(WREN);
	digitalWrite(EEPROM_CS, HIGH);
	delayMicroseconds(10);
	//delay(5);
}

//  write page 64B max
void EE_SPI_Write(uint16_t adr, uint8_t* bytes, int length)
{
	EE_SPI_Wren();
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(WRITE);
	SPI.transfer(adr >> 8);
	SPI.transfer(adr & 0xFF);
	for (int i=0; i < length; ++i)
		SPI.transfer(bytes[i]);

	digitalWrite(EEPROM_CS, HIGH);
	delay(5);  // 5ms max
}

//  write 1B
void EE_SPI_Write(uint16_t adr, uint8_t byte1)
{
	EE_SPI_Wren();
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(WRITE);
	SPI.transfer(adr >> 8);
	SPI.transfer(adr & 0xFF);
	SPI.transfer(byte1);

	digitalWrite(EEPROM_CS, HIGH);
	delay(5);  // 5ms max
}

//  read byte
uint8_t EE_SPI_Read(uint16_t adr)
{
	digitalWrite(EEPROM_CS, LOW);

	SPI.transfer(READ);
	SPI.transfer(adr >> 8);
	SPI.transfer(adr & 0xFF);
	uint8_t read = SPI.transfer(0xFF);

	digitalWrite(EEPROM_CS, HIGH);
	delayMicroseconds(10);
	//delay(1);
	return read;
}

#endif
