#pragma once

//------  Setup  -----//
//------  K.C. 4 -----//


//======------  Keyboard  ------======
//  Define just one type, for matrix.h

//#define CK1  // 8x8  test
#define CK9  // 18x8  CK9/6/3
//#define CK8  // 20x8  wip CK8/5


//  Optional features, comment out to disable
//----------------------------------------
//#define DEMOS_OLD
#define DEMOS_BITMAPS  // big, flash ! 614400 B  // todo: half it RGBA->R5G6B5
#define GAME

//  Uncomment to draw using double buffering
//  uses 69% RAM but has almost always 45 Fps flat on all demos
//  320x240x2 = 156300 B
#define BUFx2


//  Extra keyboard features, pins
//----------------------------------------
#define LCD_LED  19  // PWM brightness
// PWM on: 14,15, 18,19, 22,23

#ifdef CK9
//  Free pins: 14,15,16,17, 18, 20,21, 26
    //  led light, keyboard lamp
	#define LED_LAMP  18
	//  temperature 'C sensor DS18B20
	#define TEMP1  21
	//  photoresistor divider for adc
	#define LIGHT_SENS  20
#endif

#ifdef CK8
//  Free pins: 18, 20,21,22, 23, 26-
	#define TEMP1  23
#endif


//--------------------//
//----  Setup End ----//


#include "def_menu.h"
