#pragma once

extern void EE_SPI_Start();
extern void EE_SPI_End();
extern uint8_t EE_SPI_Status();

extern void EE_SPI_Wren();
extern void EE_SPI_Write(uint16_t adr, uint8_t* bytes, int length);
extern uint8_t EE_SPI_Read(uint16_t adr);
