#include "gui.h"
#include "matrix.h"
#include "kc_data.h"

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  draw utils
int16_t Gui::GetWidth(const char* s)
{
	int16_t x1, y1;   uint16_t w, h;
	d->getTextBounds(s, 0,0, &x1,&y1, &w,&h);
	return w;
}
void Gui::PrintR(const char* s, int16_t x, int16_t y)
{
	d->setCursor(x - GetWidth(s), y);
	d->print(s);
}


//  bitmap
//....................................................................................
void Gui::DrawBmp(const Bmp20* bmp, int16_t x, int16_t y, uint16_t bck, uint al)
{
	DrawBmp((const uint8_t*)bmp, x, y, 20, 20, bck, al);
}
void Gui::DrawBmp(const Bmp320* bmp, uint16_t bck, uint al)
{
	DrawBmp((const uint8_t*)bmp, 0, 0, 320, 240, bck, al);
}
void Gui::DrawBmp(const uint8_t* bmp, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t bck, uint al)
{
	const uint8_t* q = bmp;
	int i,j;
	if (bck == 0)
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
	else
	{
		uint bb = (bck &0x1F) *8;
		uint bg = ((bck>>5) &0x3F) *4;
		uint br = ((bck>>11) &0x1F) *8;
		//uint bl = 256; // -al;
		for (j=0; j < h; ++j)
		{
			uint a = (y+j)*W + x;
			for (i=0; i < w; ++i,++a)
			{
				uint b = *q;  ++q;
				uint g = *q;  ++q;
				uint r = *q;  ++q;
				int aa = *q, ba = 256 -aa;  ++q;
				b = (b * al * aa + bb * 256 * ba)/ 65536/8;
				g = (g * al * aa + bg * 256 * ba)/ 65536/4;
				r = (r * al * aa + br * 256 * ba)/ 65536/8;
				demos.data[a] = RGB2(r, g, b);
		}	}
	}
}


//  fade
void Gui::FadeClr(EFadeClr ec, const uint8_t mi, const uint8_t mul, const uint8_t div, uint16_t bckClr)
{
	const uint8_t* clr = &gMenuClr[ec][0][0];
	const uint8_t* cmu = &gMenuClr[ec][1][0];

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
uint16_t Gui::FadeClr(uint8_t r, uint8_t g, uint8_t b, uint8_t mul, uint8_t div, uint8_t mi, uint16_t bckClr)
{
	uint16_t clr = RGB(
		max(mi, r * mul / div),
		max(mi, g * mul / div),
		max(mi, b * mul / div));
	d->setColor( clr, bckClr );
	return clr;
}


//  HSV 888  255,255,255  to  RGB 565 31,63,31
uint16_t Gui::HSV(int h, int s, int v)
{
	//  gray
	if (s == 0)
		return RGB(v/2, v, v/2);

	//  hue 0-5  255/6
	int hue = h / 43, rem, P,Q,T;

	//  remainder part  0-255
	rem = (h - (hue * 43)) * 6; 

	P = (v * (255 - s)) >> 8;
	Q = (v * (255 - ((s * rem) >> 8))) >> 8;
	T = (v * (255 - ((s * (255 - rem)) >> 8))) >> 8;

	int r,g,b;
	switch (hue)
	{
	case 0:	 r = v;  g = T;  b = P;  break;
	case 1:  r = Q;  g = v;  b = P;  break;
	case 2:  r = P;  g = v;  b = T;  break;
	case 3:  r = P;  g = Q;  b = v;  break;
	case 4:  r = T;  g = P;  b = v;  break;
	case 5:  r = v;  g = P;  b = Q;  break;
	}
	return RGB2(r/8,g/4,b/8);
}
