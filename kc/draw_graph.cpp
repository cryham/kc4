#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "kc_data.h"


//  temp val
int Gui::TempFtoB(float tm)
{
	int t = 255.f * (tm - par.minTemp) / (par.maxTemp - par.minTemp);
	t = t > 255 ? 255 : (t < 1 ? 1 : t);  // 0 not measured
	return t;
}
float Gui::TempBtoF(uint8_t b)
{
	return b / 255.f * (par.maxTemp - par.minTemp) + par.minTemp;
}


//  Graphs ~ ~
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void Gui::DrawGraph(bool legend, int8_t var, int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax)
{
	char a[64];
	int xc = par.xCur;
	bool cursor = legend && xc < W;
	const int16_t xLen = xMax - xMin, yLen = yMax - yMin, top = legend ? 11 : 5;  // marg
	int i,n, x,y0, v,y,h;

	auto getPv = [&](auto i){  int ii = kc.grPpos + i + W;  return kc.grPMin[ii % W];  };
	auto getTv = [&](auto i){  int ii =    grTpos + i + W;  return    grTemp[ii % W];  };

	auto xtP = [&](auto tm){  return xMax - tm * 60000 / t1min(par);  };   // x time, press
	auto xtT = [&](auto tm){  return xMax - tm * 60000 / tTgraph(par);  }; // x time, temp

	auto GridLine = [&](int x, uint16_t c, const char* s)
	{
		if (x <= xMin)  return;
		d->drawFastVLine(x, yMin + top, yLen - top, c);
		x -= 6;
		if (x < xMin+12 || x >= xMax-10)  return;
		d->setCursor(x, yMin);
		d->setClr(c);
		d->print(s);
	};

	//  Press/1min
	//------------------------------------------------------------------------
	if (var == 0)
	{
		//  grid  |||
		d->setFont(OpenSans10);

		if (par.time1min && legend)
		{
			GridLine(xtP( 10), RGB(14, 14, 14), "10");  // m
			GridLine(xtP( 30), RGB(14, 14, 14), "30");
			GridLine(xtP( 60), RGB(18, 18, 18), "1h");  // h
			GridLine(xtP(120), RGB(16, 16, 16), "2h");
			GridLine(xtP(240), RGB(16, 16, 16), "4h");
			GridLine(xtP(480), RGB(16, 16, 16), "8h");
		}

		//  vertical  ===
		for (i = 1; i <= 4; ++i)
		{
			v = i * 50;
			h = legend ? 2 * v / 4 : v / 3;  // sc
			y = yMax - h;
			if (y >= yMin && y <= yMax)
			{
				n = i%10==0 ? 12 : i%5==0 ? 10 : i%2==0 ? 8 : 6;
				if (!legend)  n /= 2;

				d->drawFastHLine(xMin,y, xLen,RGB(n,n,n));
		}	}

		//  graph  Press/1min  ~~~
		for (i = xMin; i <= xMax; ++i)
		{
			v = getPv(i);
			if (v > 0)
			{
				ClrPress(v);

				h = legend ? 2 * v / 4 : v / 3;  // scale ..
				if (h > yLen)  h = yLen;

				y = yMax - h;
				if (y < 0)  y = 0;

				if (y+h < H)
				if (legend)
					d->drawFastVLine(i, y, h, d->getClr());
				else
					d->drawPixel(i, y, d->getClr());

				if (i == xc)
					d->drawPixel(i, y, RGB(31,31,31));  //.
		}	}

		//  legend
		x = xMin;  y0 = yMin + top;
		if (legend)
		{
			d->setClr(20, 20, 25);
			d->setCursor(x, y0);
			d->println("Press/min");
			d->setFont(OpenSans12);
			d->moveCursor(0,4);

			if (cursor)
			{
				v = getPv(xc);
				ClrPress(v);
				sprintf(a,"%d", v);  d->println(a);

				d->moveCursor(0,2);
				PrintInterval(t1min(par)*(W-1-xc));  d->println("");
			}
			//  max
			v = yLen * 4 / 2;  // sc
			d->setClr(16, 16, 20);
			d->moveCursor(0,4);
			if (!cursor)
			{	d->print("max ");
				sprintf(a,"%d", v);  d->println(a);
			}
		}
	}

	#ifdef TEMP1   //  Temp'C
	//------------------------------------------------------------------------
	if (var == 1)
	{
		//  auto range  get min,max
		if (grTempUpd)
		{	grTempUpd = 0;

			uint8_t tmin = 255, tmax = 0;
			for (i=0; i <= W-1; ++i)
			{
				uint8_t t = grTemp[i];
				if (t > 0)  // measured
				{
					if (t > tmax)  tmax = t;
					if (t < tmin)  tmin = t;
				}
			}
			if (tmin > tmax)  // none yet
			{
				grFmin = 20;  grBmin = TempFtoB(grFmin);
				grFmax = 23;  grBmax = TempFtoB(grFmax);
			}else
			{	grFmin = floor(TempBtoF(tmin));
				grFmax =  ceil(TempBtoF(tmax));  // in 'C
				if (grFmin + 3 >= grFmax)  // min range 3'C
				{	--grFmin;  ++grFmax;  }
				
				grBmin = TempFtoB(grFmin);
				grBmax = TempFtoB(grFmax);
				if (grBmin == grBmax)
					++grBmax;
			}
		}

		//  grid  |||
		d->setFont(OpenSans10);
		if (legend)
		{
			GridLine(xtT(  5), RGB(10, 10, 10), "5" );
			GridLine(xtT( 10), RGB(15, 15, 15), "10");  // m
			GridLine(xtT( 30), RGB(13, 13, 13), "30");
			GridLine(xtT( 60), RGB(18, 18, 18), "1h");  // h
			GridLine(xtT(120), RGB(16, 16, 16), "2h");
			GridLine(xtT(240), RGB(16, 16, 16), "4h");
			GridLine(xtT(480), RGB(16, 16, 16), "8h");
		}

		//  vertical  ===
		if (grFmax > grFmin)
		for (i = grFmin; i <= grFmax; ++i)  // 'C
		{
			y = yMax - yLen * (float(i) - grFmin) / (grFmax - grFmin);
			if (y >= yMin && y <= yMax)
			{
				n = i%10==0 ? 12 : i%5==0 ? 10 : i%2==0 ? 8 : 6;
				if (!legend)  n /= 2;
				d->drawFastHLine(xMin,y, xLen,RGB(n,n,n));
		}	}

		//  graph  Temp  ~~~
		if (grBmax > grBmin)
		for (i = xMin; i <= xMax; ++i)
		{
			v = getTv(i - xMax);
			if (v > 0)
			{	ClrTemp(v);

				y = yMax - yLen * (float(v) - grBmin) / (grBmax - grBmin);
				if (y >= yMin && y <= yMax)
					d->drawPixel(i,y, i == xc ? RGB(31,31,31) : d->getClr());
		}	}

		//  legend
		x = xMin;  y0 = yMin + top;
		d->setCursor(x, y0);
		if (!legend)
		{
			d->setClr(12, 13, 14);
			d->setFont();
			sprintf(a,"%d", grFmax);  d->println(a);
			sprintf(a,"%d", grFmin);  d->println(a);
		}
		else  // big
		{
			d->setClr(18, 22, 25);
			d->println("Temp \x01""C");
			d->setFont(OpenSans12);
			d->moveCursor(0,2);

			if (cursor)
			{
				d->drawPixel(xc, yMin, RGB(29,29,29));  //^
				d->drawPixel(xc, yMax, RGB(29,29,29));  //v

				v = getTv(xc);
				ClrTemp(v);

				float f = xc == W-1 ? fTemp : // latest
					TempBtoF(v);
				dtostrf(f,4,2,a);  d->println(a);

				d->moveCursor(0,1);
				PrintInterval(tTgraph(par)*(W-1-xc));  d->println("");
			}

			//  max min
			d->setClr(17, 19, 22);
			d->moveCursor(0,2);
			if (!cursor)  d->print("max ");
			sprintf(a,"%d", grFmax);  d->println(a);

			if (!cursor)  d->print("min ");
			sprintf(a,"%d", grFmin);  d->println(a);
		}
	}
	#endif
}
