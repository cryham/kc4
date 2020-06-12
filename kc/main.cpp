#include <Arduino.h>
#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include "usb_keyboard.h"

#include "gui.h"
#include "matrix.h"
//#include "periodic.h"
#include "IntervalTimer.h"  // todo ..
#include "kc_data.h"


//  scan counter, freq
uint scan_cnt = 0, scan_freq = 0;
uint32_t us_scan = 0, ms_scan = 0;
uint8_t scan_n = 0;

Gui gui;
KC_Main kc;
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
	if (gui.kbdSend ||  // slower for demos
		gui.ym != M_Demos || scan_n % 4==0)
	{	Matrix_scan(0);  bsc = true;  }  // K


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

	//  48 MHz/50 000 = 960 Hz   d: 52 fps
	//Periodic_init( par.scanFreq * 1000 );  // todo ..
	//Periodic_function( &main_periodic );

	#ifdef LED
	pinMode(LED, OUTPUT);
	digitalWrite(LED, gui.led ? LOW : HIGH);
	#endif

	//  pin 19, PWM brightness to display LED
	pinMode(19, OUTPUT);
	analogWrite(19, 15); // 0-255

	//  Init display
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
	gui.kbdSend = 0;  //1;  // release

#ifdef CK8
	gui.kbdSend = 0;  // release
	gui.SetScreen(ST_Test2+T_Matrix);
	par.brightness = 100;
	par.brightOff = 90;
#endif

#ifdef CK1aa
	gui.kbdSend = 0;  // release
	par.brightness = 65;
	par.brightOff = 85;
	par.debounce = 8;  // ms?
	par.strobe_delay = 8;
	par.scanFreq = 50;  // mul by 1 kHz

	par.krDelay = 250/5;  par.krRepeat = 80/5;  // ms
	par.mkSpeed = 100;  par.mkAccel = 100;
	par.mkWhSpeed = 100;  par.mkWhAccel = 100;
	par.quickKeys = 1;
	par.msLLTapMax = 0;  par.msLLHoldMin = 20;
	par.rtcCompensate = 0;

	par.dtSeqDef = 20;
	par.defLayer = 0;  par.editLayer = 2;
	gui.SetScreen(ST_Test2+T_Pressed);
	kc.Save();
#endif
	gui.SetScreen(ST_Demos);  //+


	ulong all = 0;

	gui.Init(&tft);

	while (1)
	{
		ulong stAll = micros();
		gui.Clear();

		ulong stDraw = micros();
		gui.Draw();
		tft.setFont(OpenSans12);

		tft.setCursor(0, H-28);
		tft.setColor(RGB(26, 25, 31), ILI9341_BLACK);
		//tft.print("t ");  tft.println(gui.demos.t);
		
		if (all > 0)
			tft.println(int(1000000.f / all));
		#if 1
		ulong draw = micros() - stDraw;
		if (draw > 0)
			tft.println(int(1000000.f / draw));
		#endif
#if 0		
		int y = 4;
		tft.setFont(OpenSans24);
		tft.setCursor(0, y);  y += 33;
		tft.println("24-acdtegiMkloprSt.");

		tft.setFont(OpenSans20);
		tft.setCursor(0, y);  y += 29;
		tft.println("20-acdtegiMkloprSty.");

		tft.setFont(OpenSans18);
		tft.setCursor(0, y);  y += 26;
		tft.println("18-ICDacdtegiMkloprSty.");

		tft.setFont(OpenSans16);
		tft.setCursor(0, y);  y += 24;
		tft.println("16-ICDacdrVwxyzQoprStuyz.");

		tft.setFont(OpenSans14);
		tft.setCursor(0, y);  y += 21;
		tft.println("14-AbcDefGhijklMnopqrStuVwxyz.");

		tft.setFont(OpenSans12);
		tft.setCursor(0, y);  y += 18;
		tft.println("12-GhijklMnopqRstuVwxyz-1234567890.");

		tft.setFont(OpenSans10);
		tft.setCursor(0, y);  y += 14;
		tft.println("10-abCdefGhijklMnopqRstuVwxyz-1234567890.");

		tft.setFont(OpenSans9);
		tft.setCursor(0, y);  y += 13;
		tft.println("9-QUIOP.ASDHJKL;ZXCVBNM+acdeprst=1234567890.");

		tft.setFont(OpenSans8);
		tft.setCursor(0, y);  y += 13;
		tft.println("8-QWERTY.ASDFGHJ;ZXCVBN+acdeprst=1234567890.");

		tft.setFont();
		tft.setCursor(0, y);
		tft.println("abCdefGhijklMnopqRstuVwxyz-1234567890.");
#endif
		tft.waitUpdateAsyncComplete();  // all 45 Fps 60 MHz, some tearing
		tft.updateScreenAsync();
		//tft.waitUpdateAsyncComplete();  // all 28-45 Fps 60 MHz

		all = micros() - stAll;

		//gui.DrawEnd();

		//  temp get  --------
		#ifdef TEMP1  // 18B20  Temp'C
		gui.GetTemp();
		#endif
	}
}
