#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"


//  Pick  key codes list
//....................................................................................
void Gui::DrawPickKey()
{
	int x=0, y=2, xg = W*2/3 -10;

	d->setFont(OpenSans12);
	d->setCursor(22,y);
	d->setClr(28,28,26);
	d->print("Pick key..");
	DrawBmp(&bmpKEY,0,0);

	d->setCursor(W/3 +10, y);
	d->setClr(24,22,22);
	//sprintf(a,"%3d/%d", keyCode, KEYS_ALL_EXT);
	//d->print(a);
	if (grpFilt)  d->print("/  Group:");
			else  d->print("/  All");

	//  line  --
	y = 20;
	d->drawFastHLine(0, y, W-1,
		grpFilt ? RGB(12,9,9) : RGB(9,9,12));

	x = (W-1-5) * keyCode / KEYS_ALL_EXT;
	d->drawFastHLine(x, y, 5,
		grpFilt ? RGB(30,23,26) : RGB(26,24,28));

	//  key, grp
	y = 32;
	d->setCursor(xg, y);
	d->setClr(25,28,31);
	d->print(cKeyStr[keyCode]);
	d->setCursor(0, y);

	//  seq preview  ---
	if (keyCode >= K_Seq0 && keyCode <= K_SeqLast)
	{
		int8_t seq = keyCode - K_Seq0;
		if (seq < KC_MaxSeqs)
			DrawSeq(seq, 2, 0);
	}
	else if (keyCode >= K_Fun0 && keyCode <= K_FunLast)
	{
		int8_t fun = keyCode - K_Fun0;
		d->print(cFunStr[fun]);
	}

	y = 2;
	d->setCursor(xg, y);  // group
	d->setClr(31,15,21);

	const int xw = 60, h = 17, y0 = 66;

	//  Filtered view by group, 3 cols
	if (grpFilt)
	{
		const uint8_t gr = keyGroup;
		d->print(cGrpName[gr]);

		int gn = grpStart[gr], y = y0, x = xw;
		gn += (keyCode - gn)/PickR*PickR;

		while (gn <= grpEnd[gr] && x < W-xw)
		{
			d->setCursor(x, y+1);
			int q = abs(gn - keyCode);  // diff clr

			uint16_t bck = !q ? RGB(10,10,11) : 0;
			if (bck)  //  cursor
				d->fillRect(x-3,y-1, xw,h, bck);

			FadeGrp(gr, 9, q, 3, bck);
			d->print(cKeyStr[gn]);
			if (bck)  //  [ ]
				d->drawRect(x-4,y-1, xw,h, RGB(20,23,26));

			++gn;
			y += h;  if (y > H-1-h)
			{	y = y0;  x += xw;  }
		}
		return;
	}

	//  All view  list
	d->print(cGrpName[cKeyGrp[keyCode]]);

	const static int pgMin = ((KEYS_ALL_EXT-PickR*4)/PickR +1)*PickR;
	int pg = max(-PickR*1, min(pgMin, keyCode/PickR*PickR -PickR*1));

	for (int c=grpFilt ? 1:0; c < 5; ++c)  // cols
	for (int i=0; i < PickR; ++i)  // rows
	{
		x = c * xw;  y = y0 + i*h;
		d->setCursor(x, y+1);
		int k = (pg + i + c*PickR + KEYS_ALL_EXT) % KEYS_ALL_EXT;
		int q = abs(k - keyCode);  // diff clr
		uint8_t gr = cKeyGrp[k];
		{
			uint16_t bck = !q ? RGB(10,10,11) : 0;
			if (bck)  //  cursor
				d->fillRect(x-3,y-1, xw,h+1, bck);
			
			FadeGrp(gr, 9, q, 3, bck);
			d->print(cKeyStr[k]);
			if (bck)  //  [ ]
				d->drawRect(x-4,y-1, xw,h+1, RGB(20,23,26));
		}
	}
}


//  Mapping
//....................................................................................
void Gui::DrawMapping()
{
	char a[64];
	int16_t x=2, y=0;

	if (pickCode)
	{
		DrawPickKey();
		return;
	}


	d->setClr(13,13,20);
	d->print(strMain[ym]);
	d->setFont(OpenSans12);

	int id = scId;
	if (drawKeys[drawId].sc == NO)  id = -1;
	else  id = drawKeys[drawId].sc;

	bool layUse = nLay == KC_MaxLayers;  // vis mode, key used in layers counts
	bool layPress = nLay == KC_MaxLayers+1;  // vis mode, key pressed counts

	//x=2;  y = yTitle -12;
	x = W/2 -20;  y = 16;
	
	for (int i=0; i < 3; ++i)
	{
		uint16_t bck = 0;
		a[0]=0;

		switch (i)
		{
		case 0:  // press
			switch (pressKey)
			{
			case 0:
			case 1:  sprintf(a," /  Press");  break;
			case 2:  sprintf(a," /  Press key ..");  bck = RGB(2,6,8);
				d->fillRect(x-6, y-1, W/4, 20, bck);
				/*d->drawRect(0, y-1, 2*W/3, 20, RGB(20,28,12));*/  break;
			}
			break;

		case 1:
			if (layUse)
				sprintf(a,"p  Layer:  Use");
			else if (layPress)
				sprintf(a,"p  Layer:  Heat map");
			else
				sprintf(a,"p  Layer:  %d", nLay);
			break;

		case 2:
			x = 12;  y = yTitleUp;
			d->setCursor(x,y);
			if (id < 0 || id >= kc.set.nkeys())
				sprintf(a,"-  Key:  NONE");
			else
			{
				uint8_t u = kc.set.key[layUse || layPress ? 0 : nLay][id];
					 if (u == KEY_NONE)      sprintf(a,"-  Key:  None");
				else if (u >= KEYS_ALL_EXT)  sprintf(a,"-  Key:  OUT");
				else
				{
					FadeGrp(cKeyGrp[u], 9, 0, 3, 0);
					sprintf(a,"-  Key:  %s", cKeyStr[u]);
					d->print(a);
					y += 22;  d->setCursor(x, y);

					if (layPress)
					{
						sprintf(a,"Pressed count:  %d", cnt_press_key[id]);
						d->setClr(10,22,31);
						d->print(a);
					}
					//  seq preview  ---
					else if (u >= K_Seq0 && u <= K_SeqLast)
					{
						int8_t seq = u - K_Seq0;
						if (seq < KC_MaxSeqs)
							DrawSeq(seq, 2, 0);
					}
					else if (u >= K_Fun0 && u <= K_FunLast)
					{
						int8_t fun = u - K_Fun0;
						d->print(cFunStr[fun]);
					}
					a[0]=0;
			}	}
			break;
		}
		FadeClr(c_Map, 4, i+1, 1, bck);
		d->setCursor(x,y);
		d->print(a);

		y += 18;
	}

	//  ids dbg -
	/*d->setClr(16,20,12);
	d->setCursor(0, 6*8+4);
	sprintf(a,"sc: %d dr %d x %2d y %2d", scId, drawId, drawX, drawY);  d->print(a);*/


	//  key binds, lay data
	//. . . . . . . . . . . . . . . . . . . . . . . . .
	x = W-72;  y=2;
	d->setClr(10,12,21);
	d->setCursor(x,y);
	int si = kc.set.nkeys();
	//sprintf(a,"all %d", si);  d->print(a);  y+=10;

	d->setCursor(x,y);
	if (!layPress)
	if (id >= 0 && id < si)
	{
		//sprintf(a,"L  key");  // hdr
		//d->print(a);  y += 18;

		for (int i=0; i < KC_MaxLayers && y < H/2; ++i)
		{
			uint8_t kd = kc.set.key[i][id];
			if (kd != KEY_NONE)
			{
				uint16_t bck = i == nLay ? RGB(4,7,10) : 0;
				if (bck)
					d->fillRect(x-2, y-1, W-1-x+2, 17, bck);
				FadeGrp(cKeyGrp[kd < KEYS_ALL_EXT ? kd : 0],
					9, 0/*fade*/, 2, bck);
				d->setCursor(x,y);
				sprintf(a,"%d  %s",i,
					kd < KEYS_ALL_EXT ? cKeyStr[kd] : "OUT");
				d->print(a);  y += 17;
		}	}
	}
	else
		d->print("key:  no");


	DrawLayout(true);
}
