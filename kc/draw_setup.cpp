#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "usb_keyboard.h"
#include "matrix.h"
#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"
#include "usb_mouse.h"

//  extern
extern uint scan_freq;  // scan counter, freq
extern uint32_t us_scan;


//  Setup
//....................................................................................
void Gui::DrawSetup()
{
	//  menu
	if (mlevel == 1)
	{
		d->setClr(21,23,23);
		d->print(strMain[ym]);

		DrawMenu(S_All,strSetup, C_Setup,RGB(18,24,22),RGB(4,6,6));
		pressGui = 0;
		return;
	}
	char a[64];
	int16_t y = yTitle;
	auto yadd = [&y](int16_t h){  y += h*2;  };


	int ii = ScanPages[yy], xw = W*2/3;
	uint16_t bck = RGB(3,6,6);

	auto DrawCursor = [&](auto clr)
	{
		d->setColor(clr, bck);
		d->fillRect(0, y-3, xw, 21, bck);
		d->setCursor(4,y);
		d->print(">");
	};


	//  title
	d->setClr(17,22,22);
	d->print(strSetup[yy]);
	d->setFont(OpenSans12);
	d->setClr(21,26,26);

	switch (yy)
	{
	//-----------------------------------------------------
	case S_Layer:
	{
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Lay);
			if (!c)
				DrawCursor(RGB(30,25,20));
			d->setCursor(20,y);

			FadeClr(C_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				sprintf(a,"Default layer:  %d", par.defLayer);  y-=2;  break;

			case 1:  // todo warning seq key not bound
				sprintf(a,"Sequence edit layer:  %d", par.editLayer);  break;

			case 2:
				d->print("Lock Fast max:  ");
				dtostrf(par.msLLTapMax*0.01f, 4,2, a);  d->print(a);
				d->print(" s");
				y-=2;  break;
			case 3:
				d->print("Lock Hold min:  ");
				dtostrf(par.msLLHoldMin*0.1f, 3,1, a);  d->print(a);
				d->print(" s");
				y+=2;  break;
			}
			if (i < 2)
				d->print(a);  yadd(8+4);
		}
	}	break;

	//-----------------------------------------------------
	case S_Keyboard:
	{
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Keyb);
			if (!c)
				DrawCursor(RGB(30,25,20));
			d->setCursor(20,y);

			FadeClr(C_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:  // todo warning layer empty
				sprintf(a,"Sequence delay:  %d ms", par.dtSeqDef);  break;

			case 1:
				if (pressGui)
					sprintf(a,"Gui toggle Key:  Press ..");
				else
					sprintf(a,"Gui toggle Key:  %d", par.keyGui);  break;
			}
			if (i < 4)
				d->print(a);  yadd(8+4);
		}
	}	break;


	//-----------------------------------------------------
	case S_Scan:
	{
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Scan);
			if (!c)
				DrawCursor(RGB(10,30,30));
			d->setCursor(20,y);

			FadeClr(C_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				sprintf(a,"Scan:  %u Hz", par.scanFreq * 20);  break;
			case 1:
				sprintf(a,"Strobe delay:  %d us", par.strobe_delay);  break;
			case 2:
				sprintf(a,"Debounce:  %d ms", par.debounce);  break;
			}
			d->print(a);  yadd(8+4);
		}

		d->setClr(22,23,23);
		d->setCursor(W-1-44,0);
		d->print("Fps");

		d->setCursor(2,H-80);
		sprintf(a,"  Time:  %lu us  %u Hz", us_scan, scan_freq);
		d->println(a);
		d->setClr(20,23,26);

		d->setCursor(0,H-40);
		sprintf(a,"Matrix keys:  %d = %d x %d", ScanKeys, NumCols, NumRows);
		d->println(a);

		d->setCursor(0,H-20);
		sprintf(a,"Layout keys:  %d  %s", nDrawKeys, CKname);
		d->println(a);

	}	break;


	//-----------------------------------------------------
	case S_Mouse:
	{
		bck = RGB(3,5,6);
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Mouse);
			if (!c)
				DrawCursor(RGB(15,23,30));
			d->setCursor(20,y);

			FadeClr(C_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				sprintf(a,"Speed:  %3d  Move", par.mkSpeed);  break;
			case 1:
				sprintf(a,"Accel:  %3d", par.mkAccel);  break;
			case 2:
				if (pressGui)
					sprintf(a,"Slow key:  Press ..");
				else
					sprintf(a,"Slow key:  %d", par.keyMouseSlow);
				break;
			case 3:
				sprintf(a,"Speed:  %3d  Wheel", par.mkWhSpeed);  break;
			case 4:
				sprintf(a,"Accel:  %3d", par.mkWhAccel);  break;
			}
			d->print(a);  yadd(8+1);
		}

		///  dbg  mouse accel  --
		const int16_t x0 = 8, x1 = W/4 +x0, x2 = 2*W/4 +x0, h = 16;
		y = H-1 - h;
		d->setClr(16,20,24);

		const static char ch[3]={'<','0','>'};  // input status
		#define Ch(v)  ch[max(0, min(2, v))]
		d->setCursor(0, y);  sprintf(a,"m-+  x%c y%c  wh x%c y%c  bt %d",
			Ch(Mouse_input_x/8+1), Ch(Mouse_input_y/8+1),
			Ch(Mouse_wheel_x+1), Ch(Mouse_wheel_y+1), usb_mouse_buttons_state);
		d->print(a);  y -= h+4;

		d->setCursor(x0,y);  dtostrf(my_holdtime, 4,2, a);  d->print(a);
		d->setCursor(x1,y);  sprintf(a,"%d", my_delay);  d->print(a);
		d->setCursor(x2,y);  sprintf(a,"%d", my_speed);  d->print(a);  y -= h;

		d->setCursor(x0,y);  dtostrf(mx_holdtime, 4,2, a);  d->print(a);
		d->setCursor(x1,y);  sprintf(a,"%d", mx_delay);  d->print(a);
		d->setCursor(x2,y);  sprintf(a,"%d", mx_speed);  d->print(a);  y -= h+2;

		d->setCursor(0, y);  d->print("Hold");
		d->setCursor(x1-x0, y);  d->print("Delay");
		d->setCursor(x2-x0, y);  d->print("Speed");

	}	break;

	}
}
