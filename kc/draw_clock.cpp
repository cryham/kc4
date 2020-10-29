#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kc_data.h"
#include "TimeLib.h"


//  const
const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char* wkdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


//  util
void Gui::DrawClockCur(int i, int16_t y)
{
	d->setCursor(2, y);
	int c = abs(i - ym2Clock);  // dist dim
	uint16_t bck = !c ? RGB(4,7,10) : 0;
	if (bck)
	{
		d->fillRect(0, y-2, W/2, 20, bck);
		d->setColor(RGB(21,26,31), bck);
		d->print(">");
	}
	d->setCursor(20, y);

	FadeClr(C_Clock, 4, c, 1, bck);
}

//  color from value
void Gui::ClrPress(int pm)
{
	if (pm >240)  d->setClr(31, 31,31);  else
	if (pm >210)  d->setClr(23, 24,24);  else
	if (pm >180)  d->setClr(22, 22,31);  else
	if (pm >150)  d->setClr(31, 18,22);  else
	if (pm >120)  d->setClr(31, 10,10);  else

	if (pm > 90)  d->setClr(31, 16, 2);  else
	if (pm > 70)  d->setClr(31, 20, 2);  else
	if (pm > 50)  d->setClr(24, 24, 4);  else
	if (pm > 30)  d->setClr( 6, 24, 6);  else
	if (pm > 10)  d->setClr( 4, 22,22);  else
				  d->setClr( 6, 16,24);
}
void Gui::ClrTemp(int tm, int q)
{
	if (tm >224)  d->setClr(31/q, 10/q,10/q);  else
	if (tm >192)  d->setClr(31/q, 18/q, 4/q);  else
	if (tm >160)  d->setClr(30/q, 30/q, 2/q);  else
	if (tm >128)  d->setClr(16/q, 29/q, 2/q);  else
	if (tm > 96)  d->setClr( 6/q, 27/q, 6/q);  else
	if (tm > 64)  d->setClr( 6/q, 26/q,26/q);  else
	if (tm > 32)  d->setClr(10/q, 20/q,30/q);  else
				  d->setClr( 6/q, 16/q,29/q);
}

//  Display
//....................................................................................
void Gui::DrawClock()
{
	char a[64], f[32];

	//  time  ---
	unsigned long t = rtc_get(), to, ti;
	int h = hour(), m = minute(), s = second(),
		wk = weekday(), dy = day(), mth = month(), yr = year();
	to = t - kc.tm_on;

	bool adjust = pgClock == Cl_Adjust;
	bool simple = pgClock == Cl_Simple;
	bool clock = simple || adjust;

	bool date = yr >= 2020; // rtc set

	bool ext = pgClock == Cl_StatsExt;
	bool stats = pgClock == Cl_Stats || ext;

	//  x,y pos ~~~
	int16_t x, y;
	const int16_t x0 = W / 2, yTime = 92, yGraph = yTime - 4,
		yUptime = H - 25, yDate = yTime + 34, yTemp = yTime + 9,
		yPressed = yTime + 38, yPressMin = yPressed + 20, yPressMin1 = yPressMin - 26,
		yInact = yUptime - 30, yActive = yInact - 26;


	//  Clock Big new
	//--------------------------------------------
	if (simple || kc.forceClock)
	{
		//DrawBmp(&bmpAbs, 0, 192);
		d->setClr(12, 16, 20);
		d->setFont(OpenSans72);
		d->setCursor(h < 10 ? 72 : 36, 10);
		//  time big
		sprintf(a, "%d:%02d", h, m);  d->print(a);

		d->setClr(12, 14, 18);
		d->setFont(OpenSans24);
		//  date
		x = 50;  y = H/2 +10;  d->setCursor(x, y);
		d->setClr(11, 14, 17);
		sprintf(a, "%s  %d   %d  %s", wkdays[wk - 1], dy, mth, months[mth - 1]);
		d->print(a);

		#ifdef TEMP1
		if (fTemp > -90.f)
		{
			x = H/2 -10;  y = H-40;  d->setCursor(x, y);
			d->setClr(14, 17, 20);
			dtostrf(fTemp, 4,1, f);
			sprintf(a, "%s  'C", f);
			d->print(a);
		}
		#endif
		return;
	}

//  Light Graph
//--------------------------------------------
#ifdef LIGHT_SENS
	uint16_t light = 0;
	float fLight = 0.f;
	#define AVG 20  // 30 64
	static uint16_t lar[AVG]={0};
	static int8_t li = 0;
	static uint16_t gl[W]={0};  // graph
	static uint16_t gi = 0;
	{
		//  measure
		uint16_t val = analogRead(LIGHT_SENS);
		lar[li] = val;
		li = (li+1) % AVG;
		
		int avg = 0;  // average
		for (int i=0; i < AVG; ++i)
			avg += lar[i];
		light = avg / AVG;
		fLight = 100.f - 100.f * light / 4096.f;

		gl[gi] = light;
		gi = (gi+1) % W;
	}
	if (pgClock == Cl_GraphLight)
	{
		const int divs = 12;
		for (int i=0; i < H; ++i)  // ruler =
		if ((i+1) % divs == 0)
		{
			int n = (i+1) / divs;  // 20x, 5%
			int c = n % 5 == 0 ? 13 : n % 2 == 0 ? 10 : 8;
			// d->drawFastHLine(W-1 -c*2, i, c*2, RGB(c,c,c));
			d->drawFastHLine(0, i, W, RGB(c,c,c));
		}
		for (int i=0; i < W; ++i)
		{
			y = gl[(gi + i +W)%W] * 239 / 4096;
			int n = (W-i) / 32;  // 24  fade
			d->drawPixel(i, y, RGB(24-n*2, 28-n, 31-n/2));
		}

		d->setFont(OpenSans12);
		d->setClr(26, 26, 28);
		d->setCursor(6, 10);
		d->print("Light ");
		dtostrf(fLight, 4, 1, f);  d->print(f);  d->print(" %");

		d->setCursor(6, 30);
		sprintf(a, " %d  %d", light / 18, light);  d->print(a);
		return;
	}
#endif

	
	//  late hours Background
	//--------------------------------------------
	if (date && kc.kbdSend)  // not in menu
	{
		int8_t r = 0, g = 0, b = 0, hf = m >= 30 ? 5 : 0;
		if (h == 22){  r = hf + 10;  g = 4;  }  else
		if (h == 23){  r = 19;  g = hf + 6;  }  else
		if (h == 0) {  r = 25 + hf;  g = hf + 12;  b = 6;  }  else
		if (h < 6)  {  r = 31;  g = 24;  b = 10;  }

		if (r)
		for (int y=0; y < r + 15; ++y)  //par intens-
		{
			d->drawFastHLine(0, y, W - 1, RGB(r, g, b));
			if (r > 0 && y%2==0)  --r;
			if (g > 0 && y%2==0)  --g;
			if (b > 0 && y%3==0)  --b;
		}
	}

	//  title
	d->setClr(12, 14, 17);
	if (clock)
		DrawTitleMain(RGB(12,14,17), &bmpCLOCK, 160);


	//  Graphs  ~~~~~~~~~~~~~~~~
	if (pgClock == Cl_Graphs)
	{
		DrawGraph(0, W-1,  0,   H/2,  0, true, 0);
		DrawGraph(0, W-1,  H/2, H-1,  1, true, 0);
		return;
	}
	else if (pgClock == Cl_GraphsDaily)
	{
		DrawGraph(0, W-1,  0,   H/2,  0, true, 1);
		DrawGraph(0, W-1,  H/2, H-1,  1, true, 1);
		return;
	}
	else if (ext)  // small half graphs
	{
		DrawGraph(W/2, W-1,  0, yGraph,  0, false, 0);  // press right
		DrawGraph(0, W/2-1,  0, yGraph,  1, false, 0);  // temp left  top
	}


	//  Time  ----------------
	x = x0;  y = yTime;
	if (clock || date)
	{
		d->setFont(OpenSans24);
		d->setCursor(x, y);
		if (clock)
			d->setClr(22, 24, 26);
		else
			d->setClr(18, 22, 26);

		sprintf(a, "%d:%02d", h, m);  d->print(a);

		if (!ext)
		{	d->setFont(OpenSans12);  // sec smaller
			d->moveCursor(2, 9);
			d->setClr(15, 19, 23);
			sprintf(a, ":%02d", s);  d->print(a);
	}	}


	//  Uptime, since on  --------
	if (!adjust)
	{
		d->setFont(OpenSans20);
		h = to / 3600 % 24;  m = to / 60 % 60;  s = to % 60;
		y = yUptime;
		d->setCursor(x, y);
		d->setClr(18, 22, 28);
		sprintf(a, "%d:%02d", h, m);  d->print(a);
		
		if (!ext)
		{	d->setFont(OpenSans12);  // sec smaller
			d->moveCursor(2, 6);
			d->setClr(15, 19, 25);
			sprintf(a, ":%02d", s);  d->print(a);
	}	}


	//  time Inactive  --------
	if (!clock)
	{
		ti = t - kc.tm_key;  // actual
		bool inact = ti > 60 * par.minInactive;
		h = ti / 3600 % 24;  m = ti / 60 % 60;

		s = h*60 + m;
		if (s > 90)  d->setClr(30, 24, 30);  else
		if (s > 60)  d->setClr(24, 22, 28);  else
		if (s > 30)  d->setClr(20, 20, 26);  else
		             d->setClr(14, 18, 24);
		y = yInact;
		d->setFont(OpenSans18);
		d->setCursor(x, y);
		sprintf(a, "%d:%02d %c", h, m, inact?'*':' ');  d->print(a);


		//  time Active  --------
		if (stats)
		{
			ti = kc.tm_key - kc.tm_keyAct;
			h = ti / 3600 % 24;  m = ti / 60 % 60;

			s = h*60 + m;
			if (s > 180)  d->setClr(28, 10,  5);  else
			if (s > 110)  d->setClr(26, 13, 16);  else
			if (s > 50)   d->setClr(24, 18, 10);  else
			if (s > 25)   d->setClr(10, 22, 24);  else
			              d->setClr(15, 15, 20);
			y = yActive;
			d->setFont(OpenSans18);
			d->setCursor(x, y);
			sprintf(a, "%d:%02d%c", h, m, inact?' ':'*');  d->print(a);
		}


		//  Press/min  --------
		x = stats ? 6 : x0;
		y = yPressMin;
		d->setFont(OpenSans18);
		d->setCursor(x, y);

		float fto = to ? to : 1;
		float pm = 60.f * cnt_press / fto;
		dtostrf(pm, ext ? 1 : 3, ext ? 0 : 1, f);

		ClrPress(pm);
		d->print(f);
	}


	//  Temp'C  val  --------
	#ifdef TEMP1
	if (!adjust && fTemp > -90.f)
	{
		d->setFont(OpenSans20);
		dtostrf(fTemp, 4,2, f);
		if (pgClock == 0)
		{	d->setClr(18,22,26);
			d->setCursor(6, yTime);
		}else
		{	if (pgClock == 2)
				d->setClr(16,20,24);
			else
				d->setClr(14,18,22);
			d->setCursor(6, yTime);
		}
		sprintf(a, "%c%c%c", f[0],f[1],f[2]);  d->print(a);
		d->setFont(OpenSans14);  // dot smaller
		d->moveCursor(2, 4);
		sprintf(a, ext ? "%c" : "%c%c", f[3],f[4]);  d->print(a);
	}
	#endif


	//  Layer  --------
	if (stats)
	{
		bool lock = kc.nLayerLock >= 0, held = kc.nLayerHeld >= 0;

		if (lock) d->setClr(28, 23, 30);  else
		if (held) d->setClr(17, 17, 28);  else
		  /*def*/ d->setClr(14, 14, 22);

		d->setFont(OpenSans20);
		d->setCursor(6, yUptime);
		sprintf(a, "L%d %c", kc.nLayer, lock ? '*' : held ? '+' : ' ');
		d->print(a);
	}


	//  Date big  --------
	if (clock && date)
	{
		int x2 = x0 + 36;
		//  day, week
		x = x0 + 4;  y = yDate;
		d->setFont(OpenSans14);
		d->setCursor(x, y);
		d->setClr(20, 23, 26);
		sprintf(a, "%d", dy);  d->print(a);

		d->setCursor(x2, y);
		d->print(wkdays[wk - 1]);

		y += 20;  // month
		d->setCursor(x, y);
		sprintf(a, "%d", mth);  d->print(a);

		d->setCursor(x2, y);
		d->print(months[mth - 1]);

		y += 20;  // year
		d->setFont(OpenSans10);
		d->setCursor(x, y);
		sprintf(a, "%04d", yr);  d->print(a);
	}


	//  small fonts  ----------------------
	d->setFont(OpenSans10);

	//  page / all  ---
	d->setClr(12, 16, 22);
	if (!stats)
	{
		d->setCursor(W-1 - 40, 4);
		sprintf(a, "%d/%d", pgClock + 1, Cl_All);  d->print(a);
	}

	//  subtitle
	if (!stats && !simple)
	{
		d->setCursor(W/2 - 2 * 6, 4);
		d->print(strClock[pgClock]);
	}


	//  labels, par values  ====--------
	d->setFont(0);
	
	int pg = ClockVars(pgClock);
    #ifdef TEMP1
    bool temp = temp1 > 2;
    #endif
    y = yTime;
	switch (pgClock)
	{

	case Cl_Stats:  //  new  no labels
	case Cl_StatsExt:  //------------
	{
		#ifdef TEMP1
		if (temp)  // 'C
		{	d->setCursor(78, yTemp);  d->print("\x01""C");  }
		#endif
		if (ext)
		{
			//  press k
			d->setFont(OpenSans10);
			d->setClr(9, 18, 22);
			x = 76;  y = yPressMin1 +2;
			d->setCursor(x, y);
			sprintf(a, "%dk", cnt_press / 1000);  d->print(a);

			//  date small  ----
			if (date)
			{
				//  day, week
				x = x0 + 8*0;  y = yDate;
				d->setFont(OpenSans14);
				d->setClr(14, 18, 23);
				d->setCursor(x, y);
				d->print(wkdays[wk - 1]);
				x += 50;
				d->setCursor(x, y);
				sprintf(a, "%d  %d", dy, mth);  d->print(a);
			}

			//  press / 1min
			x = 6;  y = yPressMin1;
			d->setCursor(x, y);

			int m1 = kc.min1_Keys;
			ClrPress(m1);
			dtostrf(m1, 1,0, f);  d->print(f);


			//  inactive times  ====
			//  Previous 2
			x = W - 1;  int w;
			ti = kc.tInact1;	h = ti / 60 % 24;  m = ti % 60;

			d->setFont(OpenSans14);
			d->setClr(15, 19, 24);
			sprintf(a, "%d:%02d", h, m);  w = GetWidth(a);
			d->setCursor(x-w, yInact + 22);  d->print(a);

			ti = kc.tInact2;	h = ti / 60 % 24;  m = ti % 60;

			d->setClr(12, 16, 22);
			sprintf(a, "%d:%02d", h, m);  w = GetWidth(a);
			d->setCursor(x-w, yInact);  d->print(a);

			//  Sum
			ti = kc.tInactSum;	h = ti / 60 % 24;  m = ti % 60;

			d->setClr(18, 18, 24);
			sprintf(a, "%d:%02d", h, m);  w = GetWidth(a);
			d->setCursor(x-w, yActive);  d->print(a);

		#ifdef LIGHT_SENS
			d->setFont(OpenSans12);
			d->setClr(16, 16, 18);
			d->setCursor(76, yPressMin + 2);
			//d->setCursor(W -50, yTime + 6);
			dtostrf(fLight, 3, 1, f);  d->print(f);  //d->print(" %");
		#endif
		}


		//  Sequence, prv  --------
		if (kc.visSeq < 0)
			kc.visSeq = 31;  //par
		if (kc.visSeq > 0)
		{	--kc.visSeq;	// start blink vis
			uint16_t c = kc.visSeq == 0 ? 0 :
				RGB(kc.visSeq/2+5, kc.visSeq/2+5, kc.visSeq);
			d->drawFastHLine(84, yUptime - 5, 30, c);
		}
		if (kc.inSeq[0] >= 0)
		{
			d->setFont(OpenSans18);
			d->setCursor(84, yUptime);
			d->setClr(25, 23, 31);
			sprintf(a, "S%d", kc.inSeq[0]);  d->print(a);

			d->setFont(OpenSans12);
			d->setCursor(6, yUptime - 28);
			DrawSeq(kc.inSeq[0], 2, 0);
		}

	}	break;


	case Cl_Adjust:  //  Adjust  ----
		y = yTime - 20;
		for (int i = 0; i <= pg; ++i)
		{
			d->setFont(OpenSans12);
			DrawClockCur(i, y);
			int8_t h = 2;
			switch (i)
			{
			case 0:  sprintf(a, "Hour");  break;
			case 1:  sprintf(a, "Minute");break;
			case 2:  sprintf(a, "Second");  h = 4;  break;

			case 3:  sprintf(a, "Day");  break;
			case 4:  sprintf(a, "Month");  break;
			case 5:  sprintf(a, "Year");  h = 6;  break;

			case 6:  sprintf(a, "RTC Compensate:  %d", par.rtcCompensate);  h = 2;  break;
			}
			d->print(a);
			y += 2*(h + 8);
		}
		break;

	}
}
