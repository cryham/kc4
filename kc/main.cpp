#include <Arduino.h>
#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include "usb_keyboard.h"
#include "gui.h"


#define TFT_DC 9
#define TFT_CS 10
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC);
//#define TFT_RST 8
//ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);

uint16_t data[320 * 240];  // screen buffer

Gui gui;


//....................................................................................
extern "C" int main(void)
{
	//Keyboard.press(KEY_A);
	//Keyboard.send_now();
	//Keyboard.releaseAll();


	//  pin 19, PWM brightness to display LED
	pinMode(19, OUTPUT);
	analogWrite(19, 15); // 0-255

	//  Init display
	tft.setFrameBuffer(data);
	tft.useFrameBuffer(true);

	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE);
	tft.setFont(OpenSans12);


	ulong all = 0;

	gui.Init(&tft);

	while (1)
	{
		ulong stAll = micros();
		gui.Clear();

		ulong stDraw = micros();
		gui.Draw();

		tft.setCursor(0, H-28);
		tft.setTextColor(RGB(26, 25, 31), ILI9341_BLACK);
		//tft.print("t ");  tft.println(gui.demos.t);
		
		if (all > 0)
			tft.println(int(1000000.f / all));
		#if 1
		ulong draw = micros() - stDraw;
		if (draw > 0)
			tft.println(int(1000000.f / draw));
		#endif

		tft.waitUpdateAsyncComplete();  // all 45 Fps 60 MHz, some tearing
		tft.updateScreenAsync();
		//tft.waitUpdateAsyncComplete();  // all 28-45 Fps 60 MHz

		all = micros() - stAll;
	}
}
