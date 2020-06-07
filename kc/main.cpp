#include <Arduino.h>

#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"
#include "gui.h"


#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);

uint16_t data[1][320 * 240];  // screen buffers
uint8_t buf = 0;

uint16_t cc = 190;

Gui gui;


//....................................................................................
extern "C" int main(void)
{
	//  pin 19, PWM brightness to display LED
	pinMode(19, OUTPUT);
	analogWrite(19, 50); // 0-255

	//  Init display
	tft.setFrameBuffer(data[0]);
	tft.useFrameBuffer(true);

	tft.begin();
	tft.setRotation(1);
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE);
	tft.setFont(OpenSans12);


	unsigned long tim = 0, start;

	gui.Init(&tft);

	while (1)
	{
		start = micros();
		gui.Draw();

		tft.setFont(OpenSans12);
		tft.setCursor(0, 0);
		//tft.setCursor(W/2, H-80);

		/*tft.setFont( OpenSans12 );
		tft.setTextColor(RGB(26,25,31), ILI9341_BLACK);*/
		tft.print("c ");  tft.println(cc);

		//tft.setFont( OpenSans16 );
		tft.setTextColor(RGB(26, 25, 31), ILI9341_BLACK);
		//tft.print("Fps ");
		if (tim > 0)
			tft.println(1000000.f / tim);

		++cc;
		if (cc % 600 == 0)
			gui.NextDemo();

		//tft.updateScreen();
		tft.updateScreenAsync();
		tft.waitUpdateAsyncComplete();

		//++buf;  if (buf >= 2)  buf = 0;
		//tft.setFrameBuffer(data[buf]);  // switch to 2nd buf
		//tft.useFrameBuffer(true);

		tim = micros() - start;
	}
}
