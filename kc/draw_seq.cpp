#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "usb_keyboard.h"
#include "matrix.h"
#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"


//  const
//......................................................
const static char* sMB[2][6]={
	{"LMB", "MMB", "RMB", "Mbk", "Mfw"},
	{"L", "M", "R", "b", "f"}};
char smx(int8_t cm) {  return cm > 0 ? '>' : '<';  }
char smy(int8_t cm) {  return cm > 0 ? 'v' : '^';  }


//  format seq cmd
void Gui::StrSeqCmd(int cmd, uint8_t cp, char* a, bool& inCmt, bool sh/*short 1 line*/)
{
	int8_t cm = cp-128;  // signed for mouse
	const char* mb = sMB[sh ? 1 : 0][cp];

	//  format command name and param  ---
	switch (cmd)
	{
	case CMD_SetDelay:
		sprintf(a, sh ? "%d" : "D%3dms", cp);  return;
	case CMD_Wait:
	{	char f[6];  dtostrf(cp*0.1f, 3,1, f);
		sprintf(a, sh ? "%s" : "W%ss", f);  }  return;

	case CMD_Comment:
		strcpy(a, inCmt ? "}" : "{");
		inCmt = !inCmt;  return;
	case CMD_Hide:
		strcpy(a, sh ? "" : "H>");  return;

	case CMD_RunSeq:
		sprintf(a, "S%2d", cp);  return;

	case CMD_Repeat:
		strcpy(a, /*sh ? "R" :*/ "Rpt");  return;
	}

	//  _mouse commands_
	if (sh)
	switch (cmd)  // short
	{
	case CM_x:  sprintf(a, "X%c", smx(cm));  break;
	case CM_y:  sprintf(a, "Y%c", smy(cm));  break;

	case CM_BtnOn:  sprintf(a,"%s+", mb);  break;
	case CM_BtnOff: sprintf(a,"%s-", mb);  break;

	case CM_Btn:   strcpy(a, mb);  break;
	case CM_Btn2x: sprintf(a,"%s2", mb);  break;

	case CM_WhX:   sprintf(a,"W%c", smx(cm));  break;
	case CM_WhY:   sprintf(a,"W%c", smy(cm));  break;

	case CM_xbig:  sprintf(a,"X|%c", smx(cm));  break;
	case CM_ybig:  sprintf(a,"Y|%c", smy(cm));  break;

	case CM_xset:  strcpy(a,"X=");  break;
	case CM_yset:  strcpy(a,"Y=");  break;
	case CM_mset:  strcpy(a, "XY");  break;
	}
	else switch (cmd)  // normal
	{
	case CM_x:  sprintf(a, "Mx%+4d", cm);  break;
	case CM_y:  sprintf(a, "My%+4d", cm);  break;

	case CM_BtnOn:  sprintf(a,"%s+", mb);  break;
	case CM_BtnOff: sprintf(a,"%s-", mb);  break;

	case CM_Btn:   strcpy(a, mb);  break;
	case CM_Btn2x: sprintf(a,"%s2", mb);  break;

	case CM_WhX:   sprintf(a,"Wx%2d", cm);  break;
	case CM_WhY:   sprintf(a,"Wy%2d", cm);  break;

	case CM_xbig:  sprintf(a,"MX%+4d", cm);  break;
	case CM_ybig:  sprintf(a,"MY%+4d", cm);  break;

	case CM_xset:  sprintf(a,"X=%+3d", cp);  break;
	case CM_yset:  sprintf(a,"Y=%+3d", cp);  break;
	case CM_mset:  strcpy(a, "XY");  break;
	}
}


//  write sequence,  short 1 line
//......................................................
const uint8_t sf = 16;  // seq fade
void Gui::DrawSeq(int8_t seq, int8_t q, uint16_t bck)
{
	char a[16];
	bool inCmt = false;

	int n=0, l = kc.set.seqs[seq].len();
	while (n < l)
	{
		uint8_t dt = kc.set.seqs[seq].data[n];
		bool isCmd = dt >= K_Cmd0 && dt <= K_CmdLast;

		//  clr
		const char* st = cKeyStr[dt];
		uint8_t gr = cKeyGrp[dt];
		const uint8_t* c = &cGrpRgb[gr][0][0];
		FadeClr(c[0],c[1],c[2], sf-q,sf, 7, bck);

		//  pos
		int16_t x = d->getCursorX(),
			xx = isCmd ? 4 : 6*strlen(st);

		if (x + xx >= W-1 -6*2)
		{	//  outside, more sign
			d->print(".");  d->moveCursor(-3,0);
			d->print(".");  return;
		}
		else
		{	//  commands ___
			if (isCmd)
			{
				int cmd = dt - K_Cmd0;
				int16_t y = d->getCursorY();
				const Clr& c = cCmdClr[cmd];

				++n;  // next, param
				uint8_t cpar = n < l ? kc.set.seqs[seq].data[n] : 0;

				StrSeqCmd(cmd, cpar, a, inCmt, true);

				int16_t w = GetWidth(a);
				uint16_t clr = FadeClr(c[0],c[1],c[2], sf-q,sf, 7, bck);

				d->print(a);
				d->drawFastHLine(x, y+14, w, clr);

				if (cmd == CMD_Hide)
					return;
			}
			else  //  keys
				d->print(st);

			d->moveCursor(6, 0);
			++n;
	}	}
}


//  Sequences kbd  view, edit
//....................................................................................
void Gui::DrawSequences()
{
	char a[64];

	//  View
	//......................................................
	if (!edit)
	{
		DrawTitle("View", RGB(12,26,26), &bmpDOTS3);
		d->setFont(OpenSans12);
		d->setClr(20,25,28);

		//  page / all, center   /
		d->setCursor(W/2 -14, 6);
		sprintf(a,"%d / %d", page+1, kc.set.seqSlots/iPage);  d->print(a);

		//  list slots
		int s = page * iPage, i, y, q;
		for (i=0; i < iPage && s < kc.set.seqSlots; ++i,++s)
		{
			y = yTitleUp + i*19;
			d->setCursor(0, y);
			//d->setClr(20,30,25);
			q = abs(i - slot);
			uint16_t bck = !q ? RGB(2,6,8) : 0;
			if (!q)  d->fillRect(0, y-2, W-1, 19, bck);
			FadeClr(C_Seq, 4, q, 2, bck);
			sprintf(a,"%2d",s);  d->print(a);

			d->setColor(RGB(0,30,30), bck);
			d->moveCursor(2,0);
			if (!q)  d->print(">");
			d->setColor(RGB(20,31,31), bck);

			d->setCursor(2*9, y);  // copy mark
			if (s == copyId)  d->print("*"); //\x7");
			d->setCursor(4*9, y);

			DrawSeq(s, q, bck);  // write
		}

		//  seq preview key(s), find  ---
		const int x = W-1 -5*9;
		q = K_Seq0 + seqId();  // code
		int l;  y=0;
		for (l=0; l < KC_MaxLayers; ++l)
		for (i=0; i < kc.set.scanKeys; ++i)
			if (kc.set.key[l][i] == q)  // found
			{
				d->setCursor(x-2*9 -4, y);
				d->setClr(25,18,21);
				sprintf(a,"L%d", l);  d->print(a);

				d->setCursor(x, y);
				int8_t dt = kc.set.key[0][i];  // layer 0 key
				FadeGrp(cKeyGrp[dt], 9, 0, 3, 0);
				sprintf(a,"%s", cKeyStr[dt]);  d->print(a);
				y += 18;
			}
	}
	//  Edit
	//......................................................
	else
	{
		DrawTitle("Edit", RGB(5,27,27), &bmpDOTS3);
		d->setFont(OpenSans18);

		int si = seqId();
		d->setClr(19,23,27);
		d->setCursor(W-1 -2*21, 4);  sprintf(a,"%2d", si);  d->print(a);
		d->setFont(OpenSans12);


		//  write sequence  ------------------------------------
		int n, l = kc.set.seqs[si].len();
		int16_t x = 1, y = yTitle, xx=0;

		bool inCmt = false;
		const int qdiv = 1 + min(11, l/20);  // less fade

		//  offset from cursor, big seqs
		n = max(0, edpos - 100);
		int ofs = n;

		if (n > 0)  // find out Cmt state
		for (int i=0; i < n; ++i)
		{
			uint8_t dt = kc.set.seqs[si].data[i];
			//  command
			bool isCmd = dt >= K_Cmd0 && dt <= K_CmdLast;
			int cmd = dt - K_Cmd0;

			if (isCmd)
			{	++n;  ++ofs;
				switch (cmd)
				{
				case CMD_Comment:
					inCmt = !inCmt;  break;
				}
			}
			// todo- get num lines, scroll-
		}

		while (n <= l && d->getCursorY() < H-1-2*12)  // +1 for cursor
		{
			xx = 0;
			bool cur = n == edpos;
			if (n < l)
			{
				uint8_t dt = kc.set.seqs[si].data[n];
				//  command
				bool isCmd = dt >= K_Cmd0 && dt <= K_CmdLast;
				int cmd = dt - K_Cmd0;
				uint16_t clr = 0;

				//  clr
				int gr = cKeyGrp[dt];
				int q = abs(n - edpos)/qdiv;
				FadeGrp(gr, 13, q, 4, 0);


				//  cmd string
				if (isCmd)
				{	++n;
					uint8_t cpar = n < l ? kc.set.seqs[si].data[n] : 0;

					StrSeqCmd(cmd, cpar, a, inCmt, false);

					const Clr& c = cCmdClr[cmd];
					clr = RGB(c[0],c[1],c[2]);
					d->setColor(clr, 0);
				}
				else  // key
					strcpy(a, cKeyStr[dt]);


				//  string width
				xx = GetWidth(a) + 6;
				if (x + xx > W-1)
				{	x = 1;  y += 16+2;  }  // new line

				d->setCursor(x,y);  d->print(a);

				if (isCmd)  // underline ___
					d->drawFastHLine(x, y+16, xx-6, clr);
			}
			if (cur)  // cursor
			{
				int16_t b = 16 * tBlnk / cBlnk;
				if (edins)  // ins |
					d->drawFastVLine(x-1, y-1-b+16, b+1, RGB(31-b/2,31-b/2,31));
				else  // ovr _
					d->drawFastHLine(x-1, y+16, b+1, RGB(31,31-b,31));
			}
			x += xx;  ++n;
		}

		//  header more signs < >
		x = W/2 - (demos.iFps ? 40 : 0);
		d->setFont(OpenSans14);
		d->setClr(21,25,28);
		d->setCursor(x, 4);  d->print(edins ? "Ins" : "Ovr");
		d->setClr(23,27,31);
		if (ofs > 0) {  d->setCursor(x -4*9, 4);  d->print("<");  }
		if (n < l) {    d->setCursor(x -2*9, 4);  d->print(">");  }

		//  cursor ofs pos/len debug
		if (demos.iFps)
		{
			d->setFont(OpenSans12);
			d->setClr(18,21,23);
			d->setCursor(x + 45, 6);
			sprintf(a,"%d %d/%d", ofs, edpos, l);  d->print(a);
		}

		++tBlnk;  // blink cur
		if (tBlnk > cBlnk)  tBlnk = 0;
	}
}


//  Info Operation
//....................................................................................
void Gui::DrawOperInfo()
{
	char a[32];
	if (tInfo < 0)  // trigger
		tInfo = 70;  // par

	if (tInfo > 0)
	{	--tInfo;
		bool h = infType == 1 || infType == 2;
		int x = W-1 - 76, x1 = x+6, xe = 9*3,
			y = 3, yy = h ? 73 : 19;

		uint16_t bck = RGB(4,6,8);
		d->fillRect(x-3, 0, W-1-(x-3), yy, bck);
		d->drawFastVLine(W-1, 0, yy * tInfo / 70, RGB(10,13,16));  // time|

		d->setFont(OpenSans12);
		d->setCursor(x, y);  y+=18;
		d->setColor(RGB(27,29,31), bck);

		const static char* strInf[6] = {
			"Reset", "Loaded", "Saved:", "Copied", "Pasted", "Swapped" };
		d->print(strInf[infType]);

		if (h)
		{	d->setColor(RGB(28,25,31), bck);  // mem`
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"%d B", kc.memSize);  d->print(a);

			d->setColor(RGB(24,21,28), bck);  // cnt
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"cnt %d", par.verCounter);  d->print(a);

			d->setColor(RGB(20,16,24), bck);  // ver-
			d->setCursor(x1, y);  y+=17;
			sprintf(a,"ver %d", kc.set.ver);  d->print(a);

			if (kc.err != E_ok)  // error string
			{
				uint16_t bck = RGB(6,4,4);
				d->fillRect(xe-3, y-2, x-3-(xe-3), 16, bck);
				d->setColor(RGB(31,22,21), bck);
				d->setCursor(xe, y);
				d->print(KCerrStr[kc.err]);
	}	}	}
}
