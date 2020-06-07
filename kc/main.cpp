#include <Arduino.h>

#include "SPI.h"
#include "ILI9341_t3n.h"

#define TFT_DC  9  
#define TFT_CS  10
#define TFT_RST 8
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);


extern "C" int main(void)
{

    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.println("Waiting...");

    uint16_t cc=0;

    //  pin 19 PWM to display LED
    pinMode(19, OUTPUT);
    while(1)
    {
        analogWrite(19, cc%10);  // 0-255
        delay(100);
        ++cc;
    }
}
