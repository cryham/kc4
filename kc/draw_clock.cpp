#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kc_data.h"

#define TEMP1  // test

//  const
uint8_t MDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int8_t t12[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

const char* mths[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char* wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


//  date from day number
inline bool isLeap(int y)
{
	return y%400==0 || (y%4==0 && y%100!=0);
}

//  days in month 1..12
int getMthDays(bool leap, int m)
{
	int md = MDays[m];
	if (leap && m == 2)
		md = 29;
	return md;
}

//  day in year 0..365
void getMD(bool leap, int day, int* mday, int* mth)
{
	int m = 1, md;
	while (day >= (md = getMthDays(leap, m)))
	{
		day -= md;  ++m;
	}
	*mday = day+1;  *mth = m;
}

int DayOfWeek(int d, int m, int y)
{
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + int(t12[m-1]) + d) % 7;
}


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
	if (pm >210)  d->setClr(31, 31,31);  else
	if (pm >180)  d->setClr(31, 25,31);  else
	if (pm >150)  d->setClr(31, 20,25);  else
	if (pm >120)  d->setClr(31, 10,10);  else

	if (pm > 90)  d->setClr(31, 16, 2);  else
	if (pm > 70)  d->setClr(31, 20, 2);  else
	if (pm > 50)  d->setClr(24, 24, 4);  else
	if (pm > 30)  d->setClr( 6, 24, 6);  else
	if (pm > 10)  d->setClr( 4, 22,22);  else
				  d->setClr( 6, 16,24);
}
void Gui::ClrTemp(int tm)
{
	if (tm >224)  d->setClr(31, 10,10);  else
	if (tm >192)  d->setClr(31, 18, 4);  else
	if (tm >160)  d->setClr(30, 30, 2);  else
	if (tm >128)  d->setClr(16, 29, 2);  else
	if (tm > 96)  d->setClr( 6, 27, 6);  else
	if (tm > 64)  d->setClr( 6, 26,26);  else
	if (tm > 32)  d->setClr(10, 20,30);  else
				  d->setClr( 6, 16,29);
}

//  Display
//....................................................................................
void Gui::DrawClock()
{
	char a[64], f[32];


	//  time  ---
	unsigned long t = rtc_get(), to, ti;
	int h = t / 3600 % 24, m = t / 60 % 60, s = t % 60,
		dt = t / 3600 / 24, yr = dt / 365 + 2000;
	to = t - kc.tm_on;

	bool adjust = pgClock == Cl_Adjust;
	bool simple = pgClock == Cl_Simple;
	bool clock = simple || adjust;

	bool date = yr >= 2019; // rtc set
	bool leap = isLeap(yr);

	bool ext = pgClock == Cl_StatsExt;
	bool stats = pgClock == Cl_Stats || ext;

	//  x,y pos ~~~
	int16_t x, y;
	const int16_t x0 = W / 2,
		yTime = 92, yUptime = H - 28, yDate = yTime + 34, yTemp = yTime + 6,
		yPressed = yTime + 36, yPressMin = yPressed + 20, yPressMin1 = yPressMin - 26,
		yInact = yUptime - 30, yActive = yInact - 26;


	//  late hours Background  --------
	if (date && kbdSend)  // not in menu
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
		d->print(strMain[ym]);


	//  Graphs  ~~~~~~~~~~~~~~~~
	int v, y0, ii, i;
	if (ext)
	for (i=0; i <= W-1; ++i)
	{
		ii = kc.grPpos + i - (W-1) + W;
		v = kc.grPMin[ii % W];
		if (v > 0)
		{
			ClrPress(v);  uint16_t c = d->getClr();
			y0 = yTime - v / 3;  // 96 is max  //par scale-
			if (y0 < 0)  y0 = 0;
			d->drawPixel(i,y0, c);
	}	}

	if (pgClock == Cl_Graphs)
	{
		DrawGraph();
		return;
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

		d->setFont(OpenSans12);  // sec smaller
		d->moveCursor(2, 9);
		d->setClr(15, 19, 23);
		sprintf(a, ":%02d", s);  d->print(a);
	}


	//  Uptime, since on  --------
	if (!adjust)
	{
		d->setFont(OpenSans20);
		h = to / 3600 % 24;  m = to / 60 % 60;  s = to % 60;
		y = yUptime;
		d->setCursor(x, y);
		d->setClr(18, 22, 28);
		sprintf(a, "%d:%02d", h, m);  d->print(a);
		
		d->setFont(OpenSans12);  // sec smaller
		d->moveCursor(2, 6);
		d->setClr(15, 19, 25);
		sprintf(a, ":%02d", s);  d->print(a);
	}


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
		y = yPressMin;  // + (pgClock == Cl_StatsText ? 26 : 28 -8);
		d->setFont(OpenSans18);
		d->setCursor(x, y);

		float fto = to ? to : 1;
		float pm = 60.f * cnt_press / fto;
		dtostrf(pm, 4, 1, f);

		ClrPress(pm);
		d->print(f);
	}


	//  Temp'C  val  --------
	#ifdef TEMP1
	float fTemp = 26.09f;  // test
	int temp1 = 4;

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
		d->print(f);
	}
	#endif


	//  Layer  --------
	if (stats)
	{
		bool lock = kc.nLayerLock >= 0,
			  def = kc.nLayer == par.defLayer;

		if (lock) d->setClr(28, 23, 30);  else
		if (def)  d->setClr(14, 14, 22);  else
		/*hold*/  d->setClr(17, 17, 28);

		d->setFont(OpenSans20);
		d->setCursor(6, yUptime);
		sprintf(a, "L%d %c", kc.nLayer, lock ? '*' : def ? ' ' : '+');
		d->print(a);
	}


	//  Date big  --------
	if (clock && date)
	{
		int mth = 0, day = 0, x2 = x0 + 36;
		getMD(leap, dt % 365, &day, &mth);
		//  day, week
		x = x0 + 4;  y = yDate;
		d->setFont(OpenSans14);
		d->setCursor(x, y);
		d->setClr(20, 23, 26);
		sprintf(a, "%02d", day);  d->print(a);

		d->setCursor(x2, y);
		d->print(wdays[DayOfWeek(day, mth, yr)]);

		y += 20;  // month
		d->setCursor(x, y);
		sprintf(a, "%d", mth);  d->print(a);

		d->setCursor(x2, y);
		d->print(mths[mth - 1]);

		y += 20;  // year
		d->setFont(OpenSans10);
		d->setCursor(x, y);
		sprintf(a, "%04d", yr);  d->print(a);
	}


	//  small font  ----------------------
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

	case Cl_Simple:
	{
		d->setClr(12, 18, 22);
		x = 6;  y = yUptime;
		d->setCursor(x, y + 4);  d->print("Uptime");

		#ifdef TEMP1  // Temp'C
		if (temp)
		{	d->setClr(12,20,25);
			d->setCursor(6, yTemp +24);  d->print("Temp \x01""C");
		}
		#endif
	}	break;

	case Cl_StatsText:  //  old, labels
	{
		d->setClr(12, 22, 30);
		x = x0 + 4;  y = yPressed;
		d->setCursor(x, y);
		sprintf(a, "%d", cnt_press);  d->print(a);
		//sprintf(a, "%lu", t);  d->print(a);

		d->setClr(10, 14, 18);
		x = 6;  y = yPressed;
		d->setCursor(x, y);  d->print("Pressed");
		y = yPressMin;
		d->setCursor(x, y);  d->print("Press/min");

		x = 6;  y = yInact;
		d->setCursor(x, y + 2);  d->print("Inactive");
		x = 6;  y = yUptime;
		d->setCursor(x, y + 4);  d->print("Uptime");

		#ifdef TEMP1
		if (temp)  // 'C
		{	d->setCursor(76, yTemp +2);  d->print("\x01""C");  }
		#endif
	}	break;

	case Cl_Stats:  //  new  no labels
	case Cl_StatsExt:  //------------
	{
		#ifdef TEMP1
		if (temp)  // 'C
		{	d->setCursor(76, yTemp +2);  d->print("\x01""C");  }
		#endif
		if (pgClock == Cl_StatsExt)
		{
			//  press k
			d->setFont(OpenSans10);
			d->setClr(9, 18, 22);
			x = 76;  y = yPressed +8;
			d->setCursor(x, y);
			sprintf(a, "%dk", cnt_press / 1000);  d->print(a);

			//  date small  ----
			if (date)
			{
				int mth = 0, day = 0;
				getMD(leap, dt % 365, &day, &mth);

				//  day, week
				x = x0 + 8*0;  y = yDate;
				d->setFont(OpenSans14);
				d->setClr(14, 18, 23);
				d->setCursor(x, y);
				d->print(wdays[DayOfWeek(day, mth, yr)]);
				x += 50;
				d->setCursor(x, y);
				sprintf(a, "%d  %d", day, mth);  d->print(a);
			}

			//  press / 1min
			x = 6;  y = yPressMin1;
			d->setCursor(x, y);

			int m1 = kc.min1_Keys;
			ClrPress(m1);
			dtostrf(m1, 3,0, f);  d->print(f);


			//  inactive times  --
			//  Previous 2
			x = W - 40;
			ti = kc.tInact1;
			h = ti / 60 % 24;  m = ti % 60;

			d->setFont(OpenSans14);
			d->setClr(15, 19, 24);
			d->setCursor(x, yInact + 22);
			sprintf(a, "%d:%02d", h, m);  d->print(a);

			ti = kc.tInact2;
			h = ti / 60 % 24;  m = ti % 60;

			d->setClr(12, 16, 22);
			d->setCursor(x, yInact - 0);
			sprintf(a, "%d:%02d", h, m);  d->print(a);

			//  Sum
			ti = kc.tInactSum;
			h = ti / 60 % 24;  m = ti % 60;

			d->setClr(18, 18, 24);
			d->setCursor(x, yActive - 0);
			sprintf(a, "%d:%02d", h, m);  d->print(a);
		}


		//  Sequence, prv  --------
		if (kc.inSeq[0] >= 0)
		{
			d->setCursor(0, yUptime - 13);
			d->setClr(25, 23, 31);
			sprintf(a, "S%d", kc.inSeq[0]);  d->print(a);

			d->setCursor(0, yUptime - 23);
			DrawSeq(kc.inSeq[0], 2, 0);
		}

	}	break;


	case Cl_Adjust:  //  Adjust  ----
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

			case 6:  sprintf(a, "RTC Compensate: %d", par.rtcCompensate);  h = 2;  break;
			}
			d->print(a);
			y += 2*(h + 8);
		}
		break;
	}
}
