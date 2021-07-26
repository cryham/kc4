#include <Arduino.h>
#include "ILI9341_t3n.h"
#include "usb_keyboard.h"
#include "IntervalTimer.h"
#include "TimeLib.h"

#include "gui.h"
#include "matrix.h"
#include "kc_data.h"

#ifdef GSM
	#include "SoftwareSerial.h"
	SoftwareSerial serGSM(GSM_RX, GSM_TX);
#endif

#ifdef REMOTE
	#include <RH_ASK.h>
	#include <SPI.h>
	RH_ASK remote(4000, REMOTE_RECV, REMOTE_SEND);
#endif
bool remote_init = true;


//  scan counter, freq
uint scan_cnt = 0, scan_freq = 0;
uint32_t us_scan = 0, ms_scan = 0;
uint8_t scan_n = 0;

Gui gui;
KC_Main kc;
IntervalTimer tim;
extern void ParInit();


ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC);

#ifdef BUFx2
	int buf = 0;
	DMAMEM uint16_t data[2][W * H];
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
	if (kc.kbdSend
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

	if (kc.kbdSend)
		kc.Send(ms);


	//  scan time vs strobe delay, 18 cols
	//  289 us - 7,  146 us - 3,  43 us - 0
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


	//  Init extra pins  --------
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

#ifdef REMOTE
    if (!remote.init())
		remote_init = false;

	uint8_t remoteId = 'a';

	#ifdef REMOTE_SEND  // send
	while(1)
	{	const uint8_t s = 64;
		uint8_t a[s] = {remoteId, remoteId, 0xAA, 0x11, 0x33};
		// for (int i=0; i < s; ++i)
		// 	a[i] = remoteId;
		remote.send(a, 2);
		remote.waitPacketSent();
		delay(1000);
		++remoteId;
	}
	#endif
#endif

#ifdef GSM
	serGSM.begin(115200);
#endif


	//  Init display  --------
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


	//  Init keyboard  --------
	ParInit();  // default params

	Matrix_setup();  // kbd pins

	tim.begin(main_periodic, 1000000 / (par.scanFreq * 20));


	int wait = 3;
#ifndef CKtest
	//  load set from eeprom
  #ifdef EEPROM_CS
	gui.eLoadSave = Gui::ee_Load;  // delayed..
	par.brightOff = 10;  // set after load
	par.brightness = 15;
  #else
	kc.Load();
  #endif

	gui.SetScreen(par.startScreen);
	kc.kbdSend = 1;  // 1 release

#else  // CKtest
	kc.kbdSend = 0;
	//gui.SetScreen(ST_Config2 + Cf_Storage);
	gui.SetScreen(ST_Setup2 + St_Remote);
	gui.SetScreen(ST_GSM);
	par.brightness = 44;
#endif

#if 0  // 1 for new keyboard / test
	kc.kbdSend = 0;
	//gui.SetScreen(ST_Matrix2);  // test matrix cols,rows
	gui.SetScreen(ST_Test2+Ts_Pressed);  // test scan codes to fill kbd_layout.cpp
	par.brightness = 60;
	par.brightOff = 60;
#endif
#if 0  // 1 no keyboard, run all demos
	gui.SetScreen(ST_Demos2+D_AutoAll);
	gui.demos.bAuto = 1;
	par.brightness = 30;
	par.brightOff = 30;
#endif
	kc.setBright = 1;

	
	while (1)  //------------------------
	{
		//  Draw
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

		if (gui.eLoadSave != Gui::ee_None)
		{
			tft.waitUpdateAsyncComplete();  // full wait
			bool start = false;
			if (wait > 0)
			{	--wait;  if (!wait)  start = true;  }
			
			if (!wait)
				gui.ExecLoadSave();
			if (start)
				gui.SetScreen(par.startScreen);
		}
		else
		{	elapsedMillis em = 0;
			while (tft.asyncUpdateActive() && em < 100) ;
		}
		tft.updateScreenAsync();


		//  Temp get  --------
	#ifdef TEMP1  // Temp'C
		gui.GetTemp();
	#endif

	#ifdef REMOTE_RECV  // receive
		{	uint8_t buf[16];
			uint8_t buflen = sizeof(buf);

			if (remote.recv(buf, &buflen))  // Non-blocking
			{
				for (int i=0; i < buflen; ++i)
					gui.remoteData[i] = buf[i];
				++gui.remoteId;
			}
		}
	#endif

	#ifdef GSM
		for (int i=0; i < 2; ++i)  // par
		if (serGSM.available())
		{
			char c = serGSM.read();
			std::string& s = gui.gsmStr;
			bool last10 = !s.empty() && (s.back() == 10 || s.back() == 13);
			if (!( (/*c == 10 ||*/ c == 13) && last10))  // skip empty lines
			if (c != 0)
				s += c;
		}
		if (gui.kSave)  // fix?
		{
			tft.useFrameBuffer(true);

			tft.begin(60000000);  // 45 Fps  60 MHz  stable
			tft.setRotation(1);
		}/**/
	#endif
	}
}
