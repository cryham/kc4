#include "gui.h"
#include "matrix.h"
#include "kc_data.h"

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Draw End
//....................................................................................
void Gui::DrawEnd()
{
	DrawOperInfo();

	//  Fps  ---------
	++fps_cnt;
	uint32_t ti = millis(),
		ms_diff = ti - oldti;
	if (ms_diff >= 1000)  // 1.0s upd
	{
		oldti = ti;
		float fr = float(fps_cnt) * 1000.f / ms_diff;
		fps = fr;  // / 1000 Hz
		fps_cnt = 0;
	}

	bool sc = ym == M_Matrix && yy == X_Scan;
	if (demos.iFps == 2 ||
		(mlevel == 2 && (sc || (demos.iFps && ym == M_Demos))))
	{
		d->setFont(OpenSans12);
		uint16_t c = RGB(29,30,31);
		d->setColor(c,0);
	#if 0  // old way
		float fr = 1000.f / (ti - oldti);
		int fps = fr;
		oldti = ti;
	#else
		d->setCursor(W - (fps > 99 ? 33 : 19), H-1 - 12);
		d->print(fps);
	#endif
	}
}


//  Menu
//....................................................................................
void Gui::DrawTitleMain(uint16_t clr, const Bmp20* bmp)
{
	DrawTitle(strMain[ym], clr, bmp);
}
void Gui::DrawTitle(const char* str, uint16_t clr, const Bmp20* bmp)
{
	d->setCursor(27,4);  //par
	d->setColor(clr, 0);
	d->print(str);
	if (bmp)  DrawBmp(bmp,0,4);
}

void Gui::DrawMenu(
	int cnt, const char** str, const Bmp20** bmp,
	EFadeClr ec, uint16_t curClr, uint16_t bckClr,
	int8_t nextCol, int8_t numGap, int8_t numGap2)
{
	const int16_t xw = W/2, y1 = yTitle, yadd = 22;  // par
	const int16_t my = mlevel==0 ? ym : yy;
	d->setFont(OpenSans14);

	int16_t x=0, y=y1, c;
	for (int i=0; i < cnt; ++i)
	{
		d->setCursor(x+4,y);
		d->setColor(curClr, bckClr);
		if (i == my)
			d->fillRect(x, y-3, W/2-2, yadd, bckClr);
		if (i == my)
			d->print(">");

		c = abs(i - my);  // dist dim
		FadeClr(ec, 4, c, 1, i == my ? bckClr : 0);
		d->setCursor(x + 43,y);
		d->print(str[i]);

		if (bmp && bmp[i])
			DrawBmp(bmp[i],x+20-2,y-2, 256-c*(192/9));

		//  next, extras
		y += yadd;

		if (i == numGap || i == numGap2)
			y += yadd/2;

		if (i+1 == nextCol)
		{	x += xw;  y = y1;  }
	}
}

//  cursor
void Gui::DrawDispCur(int i, int16_t y)
{
	uint16_t bck = RGB(8,8,4);
	int c = abs(i - ym2Disp);  // dist dim
	if (!c)
	{	d->fillRect(0, y-2, W-1, 20, bck);
		d->setColor(RGB(31,22,6), bck);
		
		d->setCursor(4, y);
		d->print(">");
	}
	d->setCursor(20, y);

	FadeClr(C_Disp, 4, c, 1, !c ? bck : 0);
}


//  draw utils
//....................................................................................
int16_t Gui::GetWidth(const char* s)
{
	int16_t x1, y1;   uint16_t w, h;
	d->getTextBounds(s, 0,0, &x1,&y1, &w,&h);
	return w;
}
void Gui::PrintR(const char* s, int16_t x, int16_t y)
{
	//d->setCursor(x, y);
	// int16_t x1, y1;   uint16_t w, h;
	// d->getTextBounds(s, x,y, &x1,&y1, &w,&h);
	d->setCursor(x - GetWidth(s), y);
	d->print(s);
}

//  bitmap
void Gui::DrawBmp(const Bmp20* bmp, int16_t x, int16_t y, uint al)
{
	DrawBmp((const uint8_t*)bmp, x, y, 20, 20, al);
}
void Gui::DrawBmp(const uint8_t* bmp, int16_t x, int16_t y, int16_t w, int16_t h, uint al)
{
	const uint8_t* q = bmp;
	int i,j;
	for (j=0; j < h; ++j)
	{
		uint a = (y+j)*W + x;
		for (i=0; i < w; ++i,++a)
		{
		#if 0  // no alpha
			uint8_t b = *q;  ++q;
			uint8_t g = *q;  ++q;
			uint8_t r = *q;  ++q;
			/*uint8_t al = *q;*/  ++q;
			demos.data[(y+j)*W + x+i] = RGB2(r/8,g/4,b/8);
		#else
			uint b = *q;  ++q;
			uint g = *q;  ++q;
			uint r = *q;  ++q;
			int aa = *q;  ++q;
			b = b * al * aa / 65536 /8;
			g = g * al * aa / 65536 /4;
			r = r * al * aa / 65536 /8;
			demos.data[a] = RGB2(r, g, b);
		#endif
	}	}
}

//  time
void Gui::PrintInterval(uint32_t t)
{
	char a[32];
	int ms = t/100%10;
	int s = t/1000;  t=s;  s%=60;
	int m = t/60;  t=m;  m%=60;
	int h = t/60;
	if (h)
		sprintf(a,"%dh%02dm", h, m);
	else if (m)
		sprintf(a,"%dm%02ds", m, s);
	else //if (s)
		sprintf(a,"%d.%ds", s, ms);
	d->print(a);
}

//  fade
void Gui::FadeClr(EFadeClr ec, const uint8_t mi, const uint8_t mul, const uint8_t div, uint16_t bckClr)
{
	const uint8_t* clr = &Mclr[ec][0][0];
	const uint8_t* cmu = &Mclr[ec][1][0];

	d->setColor( RGB(
		max(mi, clr[0] - cmu[0] * mul / div),
		max(mi, clr[1] - cmu[1] * mul / div),
		max(mi, clr[2] - cmu[2] * mul / div)), bckClr );
}
void Gui::FadeGrp(uint8_t g, const uint8_t mi, const uint8_t mul, const uint8_t div, uint16_t bckClr)
{
	const uint8_t* clr = &cGrpRgb[g][0][0];
	const uint8_t* cmu = &cGrpRgb[g][1][0];

	d->setColor( RGB(
		max(mi, clr[0] - cmu[0] * mul / div),
		max(mi, clr[1] - cmu[1] * mul / div),
		max(mi, clr[2] - cmu[2] * mul / div)), bckClr );
}


//  key utils
//....................................................................................
void Gui::Check_ym1()
{
	if (ym1[ym] >= YM1[ym])  ym1[ym] = 0;
	if (ym1[ym] < 0)  ym1[ym] = YM1[ym]-1;
}

//  returns pressed scan id or -1
int Gui::PressKey(int8_t& pressKey)
{
	if (!pressKey)  return -1;

	int c = 0, ii = -1;
	for (uint i=0; i < ScanKeys; ++i)
	{	if (Matrix_scanArray[i].state != KeyState_Off)  ++c;
		if (Matrix_scanArray[i].state == KeyState_Press)  ii = i;
	}
	if (pressKey == 1)
	{	//  wait until all keys are off
		if (c == 0)
			pressKey = 2;
		return -1;
	}
	else if (pressKey == 2 && ii != -1)
		return ii;

	return -1;
}

//  key auto repeat,  row, col, ms
int8_t Gui::kr(uint8_t sc, uint16_t dt)
{
	KeyPosition st = Matrix_scanArray[sc].state;
	int16_t& m = Matrix_scanArray[sc].autoRepeat;

	if (st == KeyState_Press)
	{
		m = -1;  // start delay
		return 1;
	}
	else if (st == KeyState_Hold)
	{
		if (m < 0)
		{	m -= dt;
			if (m < -par.krDelay*5)  // delay ms
			{	m = 0;  return 1;  }
		}else
		{	m += dt;
			if (m > par.krRepeat*5)  // repeat freq
			{	m = 0;  return 1;  }
		}
	}
	return 0;
}

//  value add, range
int16_t Gui::RangeAdd(int16_t val, int16_t add, int16_t vmin, int16_t vmax, int8_t cycle)
{
	int16_t v = val;
	if (cycle)
	{	int16_t r = vmax - vmin + 1;
		v = (v + add + r) % r;
	}else
	{	v += add;
		v = min(vmax, max(vmin, v));
	}
	return v;
}


//  save KC
void Gui::Save()
{
	kc.Save();  infType = 2;  tInfo = -1;
}
//  load
void Gui::Load(int8_t reset)
{
	if (reset)
	{	kc.set.InitCK();  infType = 0;  }
	else
	{	kc.Load();  infType = 1;  }  tInfo = -1;
}
