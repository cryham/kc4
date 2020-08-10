#pragma once

//------  Setup  -----//
//------  K.C. 4 -----//


//===---  Keyboard  ---===
//  Define just one type, for matrix.h
//#define CK1  // 8x8  test
#define CK9  // 18x8  new CK9/6/3
//#define CK8  // 20x8  wip CK8/5


//  Optional features, comment out to disable
//--------------------
//#define DEMOS_OLD
#define GAME

//  Uncomment to draw using double buffering
//  uses 75% RAM but has almost always 45 Fps flat on all demos
#define BUFx2


//  Extra keyboard features, pins
//--------------------
#define LCD_LED  19  // pwm brightness

#ifdef CK9
    //  led light
	//#define LED  12
	//  photoresistor divider for adc
	#define LIGHT_SENS  20
	//  temperature 'C sensor DS18B20
	#define TEMP1  21
#endif
#ifdef CK8
	#define TEMP1  23
#endif


//--------------------//
//----  Setup End ----//


#include "def_menu.h"
