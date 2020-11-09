#pragma once

//------  Setup K.C.4  -----//

//======------  Keyboard  ------======
//  Define just one type, changes need clean rebuild
//#define CKtest  // 2x4  test
//#define CK1  // 8x8  test
//#define CK8  // 20x8  wip CK8/5
#define CK9  // 18x8  CK9/6/3
//  rows, colums pins defined in matrix.h


//  Optional features, comment out to disable
//----------------------------------------
//#define DEMOS_OLD
//#define DEMOS_BITMAPS  // big, flash ! 614400 B  // todo: half it RGBA->R5G6B5
#define GAME

//  Uncomment to draw using double buffering
//  uses 69% RAM but has almost always 45 Fps flat on all demos
#define BUFx2  //  320x240x2 = 156300 B

#define TFT_DC 9  // LCD display pins
#define TFT_CS 10


//  Extra keyboard features, pins
//----------------------------------------
#define LCD_LED  19  // PWM brightness  // PWM on: 14,15, 18,19, 22,23

#ifdef CK9  //  Free pins: 14-,15,16,17, 18-, 20-,21-, 26
	#define EEPROM_CS  14  //  external serial EEPROM e.g. 25LC256, CS pin
	#define LED_LAMP  18   //  led light, keyboard lamp
	#define TEMP1  21   //  temperature 'C sensor DS18B20
	#define LIGHT_SENS  20  //  photoresistor divider for adc
#endif

#ifdef CK8  //  Free pins: 18, 20,21,22, 23, 26-
	#define TEMP1  23
#endif

#ifdef CKtest
	#define TEMP1  23  //  external serial EEPROM e.g. 25LC256, CS pin
	#define EEPROM_CS  18
#endif

#ifndef EEPROM_CS  //  if not defined, internal eeprom used
	#define ESlotSize  1080
	#define EESize  1080
	#define ESlots  1
#else  // external EEPROM params
	#define ESlotSize  8*1024  // par  max config size B
	#define EESize  64*1024  //  for 25LC256 = 256kbit = 64kB
	#define EEPage  64  // 64 bytes max for 1 page write
	#define ESlots  7  // (EESize / ESlotSize)  last is keyb data
#endif 


//----  Setup End ----//

#include "def_menu.h"
