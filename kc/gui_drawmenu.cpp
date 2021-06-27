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

	bool sc = ym == M_Matrix && yy == Mx_Scan;
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
void Gui::DrawTitleMain(uint16_t clr, const Bmp20* bmp, uint al)
{
	DrawTitle(strMain[ym], clr, bmp, al);
}
void Gui::DrawTitle(const char* str, uint16_t clr, const Bmp20* bmp, uint al)
{
	d->setCursor(27,4);  //par
	d->setColor(clr, 0);
	d->print(str);
	if (bmp)  DrawBmp(bmp,0,4, 0,al);
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
		uint16_t bclr = i == my ? bckClr : 0;
		FadeClr(ec, 4, c, 1, bclr);
		
		d->setCursor(x + 43,y);
		d->print(str[i]);

		if (bmp && bmp[i])
			DrawBmp(bmp[i],x+20-2,y-2, bclr, 256-c*(192/9));

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

	FadeClr(c_Disp, 4, c, 1, !c ? bck : 0);
}


//  Info Operation
//....................................................................................
void Gui::DrawOperInfo()
{
	char a[32];
	const int16_t tInfoMax = 90;  // par
	if (tInfo < 0)  // trigger
		tInfo = tInfoMax;

	if (tInfo > 0)
	{	--tInfo;
		bool h = infType == 1 || infType == 2;
		int x = W-1 - 76, x1 = x+6, xe = 9*3,
			y = 3, yy = h ? 73/*+17*/ : 19;

		#define bk(a)  (a * tInfo / tInfoMax)
		int c = 10-bk(10);  // fade par
		uint16_t bck = RGB(bk(3), bk(5), 1+bk(6));
		
		d->fillRect(x-3, 0, W-1-(x-3), yy, bck);
		d->drawFastVLine(W-1, 0, yy * tInfo / tInfoMax, RGB(10,13,16));  // time|

		d->setFont(OpenSans12);
		d->setCursor(x, y);  y+=18;
		d->setColor(RGB(27-c, 29-c, 31-c), bck);

		const static char* strInf[6] = {
			"Reset", "Loaded", "Saved:", "Copied", "Pasted", "Swapped" };
		d->print(strInf[infType]);

		if (h)
		{	/*d->setColor(RGB(22-c, 16-c, 22-c), bck);  // slot
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"slot %d", kc.eSlot);  d->print(a);*/

			d->setColor(RGB(28-c, 25-c, 31-c), bck);  // mem`
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"%d B", kc.memSize);  d->print(a);

			d->setColor(RGB(24-c, 21-c, 28-c), bck);  // cnt
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"cnt %d", par.verCounter);  d->print(a);

			d->setColor(RGB(20-c, 16-c, 24-c), bck);  // ver-
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"ver %d", kc.set.ver);  d->print(a);

			if (kc.err != E_ok)  // error string
			{
				uint16_t bck = RGB(bk(8), bk(4), bk(4));
				d->fillRect(xe-3, y-2, x-3-(xe-3), 16, bck);
				d->setColor(RGB(31-c, 22-c, 21-c), bck);
				d->setCursor(xe, y);
				d->print(KCerrStr[kc.err]);
	}	}	}
}
