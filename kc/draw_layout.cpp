#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"


//  kbd draw   Layout
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  layer use count colors
const uint8_t cluM = 11;
const uint16_t clu[cluM] = {
	RGB(19,19,31), RGB(16,24,31), RGB(5,30,30), RGB(30,30,4), RGB(31,17,4), RGB(31,5,5),
	RGB(30,18,31), RGB(28,28,28), RGB(10,31,21), RGB(11,31,12), RGB(21,31,9) };

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
		bool layUse = nLay == KC_MaxLayers;  // vis mode
		bool tiny = false; //k.w < 12;
		bool lk = layKey && nLay == kL0 -K_Layer1 +1;  // cur layer key


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
		{	bck = clrRect[k.o];
			d->fillRect(x+1, y-1, k.w-1, k.h-1, bck);
		}
		uint16_t  // clr frame []
			cR = f==2 ? RGB(31,30,29) : f==1 ? RGB(28,28,29) :
				 lk ? RGB(31,26,28) : clrRect[k.o];

		//  darken  if draw has NO scId
		if (edit && no)  cR = RGB(9,9,9);

		d->drawRect(x, y-2, k.w, k.h, cR);  // frame []


		if (lk)  // cur lay key backg
		{	bck = RGB(22,12,16);
			d->fillRect(x+1, y-1, k.w-1, k.h-1, bck);
		}

		//  text  ----
		d->setCursor(
			k.o==5 || k.o==7 ? x + k.w - 13 :  // right align
			(k.o==3 ? x+1 : x+2),  // symb 3
			k.h == fH ? y-2 : y-1);  // short

		if (!no && k.sc < kc.set.nkeys())
		{
			uint8_t kk = layKey ? kL0 :
				kc.set.key[edit ? nLay : kc.nLayer][k.sc];

			const char* ch = cKeySh[kk];
			//  font size
			bool m = tiny || strlen(ch) >= 2 || ch[0] < 32 ||
				kk == K_ESC || kk == K_MENU || kk == K_PRTSCR ||
				kk == K_TILDE || (kk >= K_MINUS && kk <= K_NON_US_BS) ||  // symbols
				(kk >= KP_DIV && kk <= KP_ADD);  // numpad

			if (edit && layUse && !layKey)
			{
				//  layer use vis
				uint8_t u = 0;  // count
				for (int l=0; l < KC_MaxLayers; ++l)
					if (kc.set.key[l][k.sc] != KEY_NONE)  ++u;

				if (u > 0)
				{	d->moveCursor(tiny ? 0 : 0, m ? 2 : 2);
					d->setFont();  // small 5x7

					uint16_t c = clu[ (u-1) % cluM ];
					d->setColor(c, c); //bck);
					d->print(u);
				}
			}
			else if (kk != KEY_NONE)
			{
				//  normal
				if (m)	d->setFont();  // small
				else	d->setFont(OpenSans12);
				if (m)  d->moveCursor(0, m ? 2 : 6);

				const uint8_t* c = &cGrpRgb[cKeyGrp[kk]][0][0];
				uint16_t cl = RGB(c[0],c[1],c[2]);
				if (tiny)  // tiny rect for color, no text
					d->drawRect(d->getCursorX(), d->getCursorY()+1, 2,2, cl);
				else
				{	d->setColor(cl, m ? cl : bck);
					d->print(tiny && layKey ? &ch[1] : ch);
				}
		}	}
	}
	d->setFont(0);
}
