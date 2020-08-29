#include <Arduino.h>
#include "ILI9341_t3n.h"
#include "usb_keyboard.h"
#include "IntervalTimer.h"
#include "TimeLib.h"

#include "gui.h"
#include "matrix.h"
#include "kc_data.h"


//  scan counter, freq
uint scan_cnt = 0, scan_freq = 0;
uint32_t us_scan = 0, ms_scan = 0;
uint8_t scan_n = 0;

Gui gui;
KC_Main kc;
IntervalTimer tim;
extern void ParInit();


#define TFT_DC 9
#define TFT_CS 10
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC);

#ifdef BUFx2
DMAMEM uint16_t data[2][W * H];
int buf = 0;
#else
DMAMEM uint16_t data[W * H];  // screen buffer
#endif


//  kbd  timer event,  scan, send
//------------------------------------------------
void main_periodic()
{
	uint32_t us = micros();

	//  freq info
	uint32_t ms = millis(),
		ms_diff = ms - ms_scan;
	if (ms_diff >= 1000)  // 1s
	{
		ms_scan = ms;
		scan_freq = scan_cnt;  // Hz
		scan_cnt = 0;
	}
	++scan_cnt;
	++scan_n;


	//  kbd scan
	bool bsc = false;
	if (gui.kbdSend
		// slower for demos
		|| gui.ym != M_Demos || scan_n % 2==0
	){
		Matrix_scan(0);  bsc = true;
	}


	//  gui keys
	//------------------------
	if (bsc)
		gui.KeyPress();


	//  keyboard send
	//------------------------
	kc.UpdLay(ms);

	if (gui.kbdSend)
		kc.Send(ms);


	//  scan time vs strobe delay, 18 cols
	//  289 us - 7,  145 us - 3,  44: 0
	us_scan = micros() - us;
}


time_t getTeensyTime()
{
	return rtc_get();
}


//  main
//-------------------------------------------------------------------------
int main()
{
	setSyncProvider(getTeensyTime);


	//  Init extra pins  ----
	//  PWM brightness to display LED
	pinMode(LCD_LED, OUTPUT);
	analogWriteResolution(12);
	analogWrite(LCD_LED, 100);  // 0-4095
	
	#ifdef LED_LAMP
	pinMode(LED_LAMP, OUTPUT);
	analogWrite(LED_LAMP, 0);  // off
	#endif

	#ifdef LIGHT_SENS
	analogReadResolution(12);
	#endif


	//  Init display  ----
	memset(data, 0, sizeof(data));
	tft.useFrameBuffer(true);
	#ifdef BUFx2
	tft.setFrameBuffer(data[0]);
	#else
	tft.setFrameBuffer(data);
	#endif

	tft.begin(60000000);  // 45 Fps  60 MHz  stable
	//tft.begin(80000000);  // 60 Fps  some jitter
	//tft.begin(112000000);  // 60 Fps  jitter
	tft.setRotation(1);
	tft.fillScreen(ILI9341_BLACK);
	tft.updateScreen();

	gui.Init(&tft);


	//  Init keyboard  ----
	ParInit();  // default params

	Matrix_setup();  // kbd pins

	tim.begin(main_periodic, 1000000 / (par.scanFreq * 20));

	//  load set from eeprom
	kc.Load();
	gui.SetScreen(par.startScreen);
	gui.kbdSend = 1;  // 1 release


#if 0  // 1 for new keyboard / test
	gui.kbdSend = 0;
	gui.SetScreen(ST_Matrix2);  // test matrix cols,rows
	//gui.SetScreen(ST_Test2+T_Pressed);  // test scan codes to fill kbd_layout.cpp
	par.brightness = 60;
	par.brightOff = 60;
#endif
#if 0  // 1 no keyboard, run all demos
	// gui.SetScreen(ST_Demos2+D_AutoAll);
	// gui.demos.bAuto = 1;
	par.brightness = 30;
	par.brightOff = 30;
#endif
	kc.setBright = 1;


	while (1)  //  ---
	{
		#ifdef BUFx2
		tft.setFrameBuffer(data[buf]);
		gui.Clear(data[buf]);
		
		if (!gui.Force1Buf())
			buf = 1-buf;
		#else
		gui.Clear(data);
		#endif

		gui.Draw();
		gui.DrawEnd();

	#if 1
		elapsedMillis em = 0;
		while (tft.asyncUpdateActive() && em < 100) ;
		tft.updateScreenAsync();
	#else
		tft.waitUpdateAsyncComplete();
		tft.updateScreenAsync();
	#endif

		//  temp get  --------
		#ifdef TEMP1  // Temp'C
		gui.GetTemp();
		#endif
	}
}
