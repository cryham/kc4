#include <Arduino.h>
#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include "usb_keyboard.h"

#include "gui.h"
#include "matrix.h"
#include "IntervalTimer.h"
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
	}	// K


	//  gui keys
	//------------------------
	if (bsc)
		gui.KeyPress();


	//  keyboard send
	//------------------------
	kc.UpdLay(ms);

	if (gui.kbdSend)
		kc.Send(ms);


	//  scan time vs strobe delay
	// 570 us: 10,  353 us: 4  18x8
	// 147 us: 4,  90: 0  8x6
	us_scan = micros() - us;
}


//  main
//-------------------------------------------------------------------------
int main()
{
	//  PWM brightness to display LED
	pinMode(LCD_LED, OUTPUT);
	analogWriteResolution(12);
	analogWrite(LCD_LED, 100);  // 0-4095

	ParInit();  // par defaults


	//  kbd
	Matrix_setup();

	tim.begin(main_periodic, 1000000 / (par.scanFreq * 20));


	#ifdef LED
	pinMode(LED, OUTPUT);
	digitalWrite(LED, gui.led ? LOW : HIGH);
	#endif


	//  Init display
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

	tft.setColor(ILI9341_WHITE);
	tft.setFont(OpenSans12);

	gui.Init(&tft);


	//  load set from ee
	kc.Load();
	gui.SetScreen(par.startScreen);
	gui.kbdSend = 1;  // 1 release
	kc.setBright = 1;


#ifdef CK1  // uncomment for new keyboard / test
	gui.kbdSend = 0;
	//gui.SetScreen(ST_Test2+T_Matrix);  // test matrix cols,rows
	gui.SetScreen(ST_Test2+T_Pressed);  // test scan codes to fill kbd_layout.cpp
	par.brightness = 40;
	par.brightOff = 60;
#endif
#ifdef CK9test  // no keyboard, run all demos
	// gui.SetScreen(ST_Demos2+D_AutoAll);
	// gui.demos.bAuto = 1;
	par.brightness = 30;
	par.brightOff = 30;
#endif


	while (1)
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
		#ifdef TEMP1  // 18B20  Temp'C
		gui.GetTemp();
		#endif
	}
}
