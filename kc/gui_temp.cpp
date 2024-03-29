#include "gui.h"
#include "ILI9341_t3n.h"

#include "kc_params.h"

#ifdef TEMP1

#include <OneWire.h>
#include <DS18B20.h>

//  sensor address
byte addr[8] = {0,};

OneWire onewire(TEMP1);  // pin
DS18B20 sensors(&onewire);


//  Get Temp'C
//....................................................................................
void Gui::GetTemp()
{
	uint32_t ms = millis();

	if (temp1 == 1)
	{	temp1 = 0;  //  first

		//  Look for 1-Wire devices
		if (onewire.search(addr))  // while
		if (OneWire::crc8(addr, 7) == addr[7])
		{
			//onewire.reset_search();
			//  setup
 			sensors.begin(12);  // quality bits
 			sensors.request(addr);
			temp1 = 2;
		}
	}
	if (temp1 >= 2)
	{
		if (ms - msTemp > tTemp(par) || ms < msTemp || temp1 == 2)
		{
			//  if measurement ready
			if (sensors.available())
			{
				msTemp = ms;
				temp1 = 3;
				//?  slower if not in gui, every 20, 10 sec

				fTemp = sensors.readTemperature(addr);
				fTemp += 0.03f * par.tempOfs;  // fix sensor error
				sensors.request(addr);  // next
			}
		}

		for (int d=0; d<2; ++d)
		if (ms - msTempGr[d] > (d ? tDaily(par) : tTgraph(par)) || ms < msTempGr[d])
		{
			msTempGr[d] = ms;
			//  graph inc pos
			++grTpos[d];
			if (grTpos[d] >= W)  grTpos[d] = 0;
			
			//  add to graph
			int t = TempFtoB(fTemp);
		#if 0  // test
			static uint8_t ii = 0;  ++ii;
			static uint8_t ti = 128;  if (ii%3==0)  ++ti;  t = ti;
		#endif
			t = t > 255 ? 255 : (t < 0 ? 0 : t);  // 0 not measured
			grTemp[d][grTpos[d]] = t;
			grTempUpd[d] = 1;
		}
	}
}

#endif
