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

uint16_t data[320 * 240];  // screen buffer



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
		// slower for demos ?
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
	ParInit();  // par defaults

	//  kbd
	Matrix_setup();

	tim.begin(main_periodic, 1000000 / (par.scanFreq * 20));


	#ifdef LED
	pinMode(LED, OUTPUT);
	digitalWrite(LED, gui.led ? LOW : HIGH);
	#endif

	//  pin 19, PWM brightness to display LED
	pinMode(19, OUTPUT);
	analogWrite(19, 15); // 0-255

	//  Init display
	memset(data, 0, sizeof(data));
	tft.setFrameBuffer(data);
	tft.useFrameBuffer(true);

	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_BLACK);
	tft.setColor(ILI9341_WHITE);
	tft.setFont(OpenSans12);

	gui.Init(&tft);


	//  load set from ee
	kc.Load();
	gui.SetScreen(par.startScreen);
	gui.kbdSend = 0;  //1;  // 1 release

#ifdef CK8
	gui.kbdSend = 0;  // release
	//gui.SetScreen(ST_Test2+T_Matrix);  // test +
	gui.SetScreen(ST_Test2+T_Pressed);  // test +
	par.brightness = 100;
	par.brightOff = 90;
#endif

	while (1)
	{
		gui.Clear();

		gui.Draw();
		gui.DrawEnd();

		tft.waitUpdateAsyncComplete();  // 60 MHz, all 45 Fps, 41 with kbd
		tft.updateScreenAsync();

		//  temp get  --------
		#ifdef TEMP1  // 18B20  Temp'C
		gui.GetTemp();
		#endif
	}
}
