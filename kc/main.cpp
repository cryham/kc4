#include <Arduino.h>
#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include "usb_keyboard.h"
#include "gui.h"


#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);

uint16_t data[1][320 * 240];  // screen buffers
uint8_t buf = 0;

uint16_t cc = 0;

Gui gui;


//....................................................................................
extern "C" int main(void)
{
	//  pin 19, PWM brightness to display LED
	pinMode(19, OUTPUT);
	analogWrite(19, 10); // 0-255

	//  Init display
	tft.setFrameBuffer(data[0]);
	tft.useFrameBuffer(true);

	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE);
	tft.setFont(OpenSans12);


	typedef unsigned long ulong;
	ulong all = 0, send = 0;

	gui.Init(&tft);

	while (1)
	{
		ulong start = micros();

		gui.Draw();

		//tft.setFont(OpenSans10);
		tft.setCursor(0, 0);
		//tft.setCursor(W/2, H-80);

		/*tft.setFont( OpenSans12 );
		tft.setTextColor(RGB(26,25,31), ILI9341_BLACK);*/
		//tft.print("c ");  tft.println(cc);

		//tft.setFont( OpenSans16 );
		tft.setTextColor(RGB(26, 25, 31), ILI9341_BLACK);
		
		//tft.print("Fps ");
		if (all > 0)
			tft.println(int(1000000.f / all));
		//tft.println(send);
		
		ulong draw = all - send;
		if (draw > 0)
			tft.println(int(1000000.f / draw));/**/

		++cc;
		if (cc % 1000 == 0)
		{
			gui.NextDemo();
			//Keyboard.press(KEY_A);
			//Keyboard.send_now();
			//Keyboard.releaseAll();
		}


		//tft.updateScreen();
		ulong st_send = micros();
		tft.updateScreenAsync();
		tft.waitUpdateAsyncComplete();

		//++buf;  if (buf >= 2)  buf = 0;
		//tft.setFrameBuffer(data[buf]);  // switch to 2nd buf
		//tft.useFrameBuffer(true);

		ulong us = micros();
		send = us - st_send;
		all = us - start;
	}
}
