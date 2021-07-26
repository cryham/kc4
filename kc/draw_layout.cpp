#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"


//  layer use count colors
const uint8_t cluM = 9;
const uint16_t clu[cluM] = {
	RGB(11,11,31), RGB(11,21,31), RGB(1,31,15), RGB(15,31,1),
	RGB(31,31,11), RGB(31,15,1), RGB(31,1,1), RGB(31,15,31), RGB(28,28,28) };

uint16_t Gui::HeatClr(float f)
{
	switch (heatTheme)
	{	// hue: 0 red 1 yellow 2 green 3 cyan 4 blue 5 magenta 6 red
	case 0:  return HSV(
		max(1, int(4*43 - 6*43 * f)),  // rainbow
		255 - max(0, min(254, int(255 * 3.f*(f-0.7f)))),  // red to white
		125 + 130*min(1.f, f*4.f));  // fade in blue  to green to red
	case 1:  return HSV(
		max(3*43, int(4.9f*43 - 2.6f*43 * f)),  // viol-blue-cyan-white
		255 - max(0, min(254, int(255 * 2.5f*(f-0.6f)))),  // to white
		90 + 165*min(1.f, f*1.5f));  // fade in
	case 2:  return HSV(
		int(4*43 + 3.2f*43 * f) % 255,  // viol-magenta-red-orange-yellow-white
		255 - max(0, min(254, int(255 * 5.f*(f-0.8f)))),  // red to white
		75 + 180*min(1.f, f*1.f));  // fade in
	}
}


//  kbd draw   Layout
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void Gui::DrawLayout(bool edit)
{
	int16_t x=2, y=0;

	for (int i=0; i < nDrawKeys; ++i)
	{
		const DrawKey& k = drawKeys[i], pk = drawKeys[max(0,i-1)];
		bool no = k.sc == NO;

		//  find if pressed
		int f = !edit && !no &&
			Matrix_scanArray[k.sc].state == KeyState_Hold ? 1 : 0;

		//  mapping cursor
		if (edit && drawId >= 0 && i == drawId)  f = 2;


		//  vars  layer keys visible on all layers ``
		uint8_t kL0 = kc.set.key[0][k.sc];
		bool layKey = kL0 >= K_Layer1 && kL0 < K_Layer1+KC_MaxLayers;
		
		bool visLayUse = edit && nLay == KC_MaxLayers;  // vis modes
		bool heatMap = edit && nLay == KC_MaxLayers+1;
		uint16_t press = cnt_press_key[k.sc];
		
		bool tiny = false; //k.w < 12;
		bool layKeyCur = edit || layKey || heatMap ? false :
			layKey && nLay == kL0 -K_Layer1 +1;  // cur layer key


		//  set coords or advance
		if (k.x >=0)  x = k.x;  else  x -= k.x;
		if (k.y > 0)  y = k.y + yPosLay;  else
		{	if (pk.w < 12)  y += k.y;  else  y -= k.y;  }  // tiny up

		if (i == drawId)  // save center for move
		{	drawX = x + k.w/2;
			drawY = y + k.h/2;  }

		// backgr  ----
		uint16_t bck = 0;
		if (f)
		{	bck = clrRect[k.o] - RGB(3,3,3);
			d->fillRect(x+1, y-1, k.w-1, k.h-1, bck);
		}
		uint16_t  // clr frame []
			cR = f==2 ? RGB(31,30,29) : f==1 ? RGB(28,28,29) :
				layKeyCur ? RGB(31,26,28) : clrRect[k.o];

		//  darken  if draw has NO scId
		if (edit && no)  cR = RGB(9,9,9);

		d->drawRect(x, y-2, k.w, k.h, cR);  // frame []


		if (heatMap)  // heat map color  . .. ... .
		{
			#if 0  //  test h,s,v-
			for (int j=0; j<25; ++j)
			for (int i=0; i<W; ++i)
			{
				bck = HSV(255.f*i/(W-1), 10*j, 255);
				demos.data[i+ (60+j)*W] = bck;
				bck = HSV(255.f*i/(W-1), 255, 255-10*j);
				demos.data[i+ (85+j)*W] = bck;
			}
			#endif
			if (heatTest)  //  test theme
			for (int i=0; i<W; ++i)
			{
				bck = HeatClr(float(i) / (W-1));
				d->fillRect(i, 89, 1,6, bck);
			}

			//  hue color
			const float f =
				heatTest ? min(1.f, float(x) / (W-20))
				: 1.2f * float(press) / cnt_press_max;  /// par
			if (!press && !heatTest)
				bck = 0;
			else if (f > 0.99f)
				bck = 0xFFFF;
			else
				bck = HeatClr(powf(f,0.5f));  /// par, avg/max?

			d->fillRect(x+1, y-1, k.w-2, k.h-2, bck);
		}
		else if (layKeyCur)  // cur lay key backg
		{
			bck = RGB(20,10,14);
			d->fillRect(x+1, y-1, k.w-1, k.h-1, bck);
		}


		//  text  ----
		bool right = k.o==5 || k.o==7;  // right align
		d->setCursor(
			right ? x + k.w - 13 :
			(k.o==3 ? x+1 : x+2),  // symb 3
			k.h == fH ? y-2 : y-1);  // short

		if (!heatMap && !no && k.sc < kc.set.nkeys())
		{
			uint8_t kk = layKey ? kL0 :
				kc.set.key[edit ? nLay : kc.nLayer][k.sc];

			const char* ch = cKeySh[kk];
			//  font size
			bool m = tiny || strlen(ch) >= 2 || ch[0] < 32 ||
				kk == K_ESC || kk == K_MENU || kk == K_PRTSCR ||
				kk == K_TILDE || (kk >= K_MINUS && kk <= K_NON_US_BS) ||  // symbols
				(kk >= KP_DIV && kk <= KP_ADD);  // numpad
			bool m1 = m && strlen(ch) == 1;

			if (visLayUse && !layKey)
			{
				//  layer use vis  ----
				uint8_t u = 0;  // count
				for (int l=0; l < KC_MaxLayers; ++l)
					if (kc.set.key[l][k.sc] != KEY_NONE)  ++u;

				if (u > 0)
				{	d->moveCursor(1, 3);
					d->setFont();  // small 5x7

					uint16_t c = clu[ (u-1) % cluM ];
					d->setColor(c, c); //bck);
					d->print(u);
				}
			}
			else if (kk != KEY_NONE)
			{
				const char* s = tiny && layKey ? &ch[1] : ch;
				//  normal  ----
				if (m)	d->setFont();  // small
				else	d->setFont(OpenSans12);
				d->moveCursor(right ? 0 :
					m ? (m1 ? 2 : 0) : (k.w - GetWidth(s))/2 -2,  // center big
					m ? 2 : 1);

				const uint8_t* c = &cGrpRgb[cKeyGrp[kk]][0][0];
				uint16_t cl = RGB(c[0],c[1],c[2]);

				if (tiny)  // tiny rect for color, no text
					d->drawRect(d->getCursorX(), d->getCursorY()+1, 2,2, cl);
				else
				{	d->setColor(cl, m ? cl : bck);
					d->print(s);
				}
		}	}

		if (f==2)  // frame [] cursor on top
			d->drawRect(x, y-2, k.w, k.h, cR);
	}
	d->setFont(0);
}
