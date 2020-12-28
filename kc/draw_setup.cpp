#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "usb_keyboard.h"
#include "matrix.h"
//#include "kbd_layout.h"
#include "kc_data.h"
#include "keys_usb.h"
#include "usb_mouse.h"

#ifdef REMOTE
// #include <RH_ASK.h>
// #include <SPI.h> 
// extern RH_ASK remote;
#endif
extern bool remote_init;

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
		DrawTitle(strMain[ym], RGB(21,23,23), &bmpSETUP);
		DrawMenu(St_All,strSetup,0, c_Setup,RGB(18,24,22),RGB(4,6,6));
		pressGui = 0;
		return;
	}
	//  title
	DrawTitle(strSetup[yy], RGB(17,22,22), &bmpSETUP);
	d->setFont(OpenSans12);
	d->setClr(21,26,26);


	char a[64],b[64];
	int16_t y = yTitle, h,
		x0 = W / 2 + 20, x1 = x0 + 14, xm = 60;
	auto yadd = [&y](int16_t h){  y += h*2;  };

	int ii = ScanPages[yy];
	uint16_t bck = RGB(3,6,6);

	auto DrawCursor = [&](auto clr)
	{
		d->setColor(clr, bck);
		d->fillRect(0, y-3, yy == St_Mouse ? W*2/3 : W, 21, bck);
		d->setCursor(4,y);
		d->print(">");
	};

	switch (yy)
	{
	//-----------------------------------------------------
	case St_Layer:
	{
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Lay);
			if (!c)
				DrawCursor(RGB(30,25,20));
			d->setCursor(20,y);

			FadeClr(c_Setup2, 4, c, 1, !c ? bck : 0);
			h = 8+3;

			switch(i)
			{
			case 0:  // todo warning layer empty
				strcpy(a,"Default layer:");
				sprintf(b,"%d", par.defLayer);  break;
			case 1:  // todo warning seq key not bound
				strcpy(a,"Sequence edit layer:");
				sprintf(b,"%d", par.editLayer);  h+=2;  break;

			case 2:
				strcpy(a,"Lock Fast max:");
				dtostrf(par.msLLTapMax*0.01f, 4,2, b);  strcat(b," s");  break;
			case 3:
				strcpy(a,"Lock Hold min:");
				dtostrf(par.msLLHoldMin*0.1f, 3,1, b);  strcat(b," s");  break;
			}
			PrintR(a, x0, y);
			d->setCursor(x1, y);  d->print(b);  yadd(h);
		}
	}	break;

	//-----------------------------------------------------
	case St_Keyboard:
	{
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Keyb);
			if (!c)
				DrawCursor(RGB(30,25,20));
			d->setCursor(20,y);

			FadeClr(c_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				strcpy(a,"Sequence delay:");
				sprintf(b,"%d ms", par.dtSeqDef);  break;

			case 1:
				strcpy(a,"Gui toggle Key:");
				if (pressGui)
					strcpy(b,"Press ..");
				else
					sprintf(b,"%d", par.keyGui);  break;
			}
			PrintR(a, x0, y);
			d->setCursor(x1, y);  d->print(b);  yadd(8+4);
		}
	}	break;

	//-----------------------------------------------------
	case St_Mouse:
	{
		bck = RGB(3,5,6);
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Mouse);
			if (!c)
				DrawCursor(RGB(15,23,30));
			d->setCursor(20,y);

			FadeClr(c_Setup2, 4, c, 1, !c ? bck : 0);
			h = 8+1;
			
			switch(i)
			{
			case 0:
				strcpy(a,"Move Speed:");
				sprintf(b,"%d", par.mkSpeed);  break;
			case 1:
				strcpy(a,"Accel:");
				sprintf(b,"%d", par.mkAccel);  h+=2;  break;
			case 2:
				strcpy(a,"Slow key:");
				if (pressGui)
					sprintf(b,"Press ..");
				else
					sprintf(b,"%d", par.keyMouseSlow);
				h+=2;  break;
			case 3:
				strcpy(a,"Wheel Speed:");
				sprintf(b,"%d", par.mkWhSpeed);  break;
			case 4:
				strcpy(a,"Accel:");
				sprintf(b,"%d", par.mkWhAccel);  break;
			}
			PrintR(a, x0 -xm, y);
			d->setCursor(x1 -xm, y);  d->print(b);  yadd(h);
		}

		///  dbg  mouse accel  --
		const int16_t x0 = 8, x1 = W/5 +x0, x2 = 2*W/5 +x0, h = 16;
		y = H-1 -h;
		d->setClr(16,20,24);

		d->setCursor(x0,y);  dtostrf(my_holdtime, 4,2, a);  d->print(a);
		d->setCursor(x1,y);  sprintf(a,"%d", my_delay);  d->print(a);
		d->setCursor(x2,y);  sprintf(a,"%d", my_speed);  d->print(a);  y -= h;

		d->setCursor(x0,y);  dtostrf(mx_holdtime, 4,2, a);  d->print(a);
		d->setCursor(x1,y);  sprintf(a,"%d", mx_delay);  d->print(a);
		d->setCursor(x2,y);  sprintf(a,"%d", mx_speed);  d->print(a);  y -= h+2;

		d->setCursor(0, y);  d->print("Hold");
		d->setCursor(x1-x0, y);  d->print("Delay");
		d->setCursor(x2-x0, y);  d->print("Speed");


		//  mouse circle visual  o +
		const int16_t r = W/6, rm = 2, xc = W-1 -r -rm, yc = H-1 -r -rm, xt = xc - 30;
		const uint16_t c = RGB(8,10,12), q = RGB(29,30,31);
		d->drawCircle(xc, yc, r, c);  // o - |
		d->drawFastHLine(xc-r, yc, 2*r, c);
		d->drawFastVLine(xc, yc-r, 2*r, c);
		
		int i = r * mx_delay / 20000;  // del -
		static int8_t mx = 0, my = 0;
		int bx = Mouse_input_x / 8, by = Mouse_input_y / 8;
		if (bx)  mx = bx;
		if (by)  my = by;
		if (mx)
		d->drawFastHLine(mx>0 ? xc : xc-i, yc, i, q);

		i = r * my_delay / 20000;  // del |
		if (my)
		d->drawFastVLine(xc, my>0 ? yc : yc-i, i, q);
	
		d->drawCircle(xc + mx * r * mx_speed / 10,  // cur speed .
					  yc + my * r * my_speed / 10, rm, q);


		//  test input
		const static char chx[3]={'<',' ','>'}, chy[3]={'^',' ','v'};
		#define Ch(v)  max(0, min(2, v))
		uint8_t u = usb_mouse_buttons_state;
		
		y = 4;
		auto write = [&](int yy=4)
		{	d->setCursor(xt, y);  d->print(a);  y += h + yy;  };

		sprintf(a,"Testing");  write();

		sprintf(a," move %c %c",  chx[Ch(Mouse_input_x/8+1)], chy[Ch(Mouse_input_y/8+1)]);
		write();
		sprintf(a,"wheel %c %c",  chx[Ch(Mouse_wheel_x  +1)], chy[Ch(Mouse_wheel_y  +1)]);
		write();

		sprintf(a,"Buttons");  write();
		sprintf(a,"%c%c%c%c%c",
			u & MOUSE_LEFT ? 'L':' ',	u & MOUSE_MIDDLE ? 'M':' ',
			u & MOUSE_RIGHT ? 'R':' ',	u & MOUSE_BACK ? '<':' ',	u & MOUSE_FORWARD ? '>':' ');
		write();

	}	break;

	//-----------------------------------------------------
	case St_Remote:
	{
	#ifdef REMOTE
		for (int i=0; i <= ii; ++i)
		{
			int c = abs(i - ym2Remote);
			if (!c)
				DrawCursor(RGB(30,25,20));
			d->setCursor(20,y);

			FadeClr(c_Setup2, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				strcpy(a,"Send:");
				sprintf(b,"%d", remoteId);  break;

			case 1:
				strcpy(a,"Receive:");
				sprintf(b,"%d", remoteId);  break;
			}
			PrintR(a, x0, y);
			d->setCursor(x1, y);  d->print(b);  yadd(8+4);
		}

		//  info  ----
		d->setCursor(10, 110);
		d->print(remote_init ? "init Yes" : "int No");
		//  vis show memory
		{
			int s = 0;
			for (int y=0; y<6; ++y)
			for (int x=0; x<10; ++x)
			if (s < 128)
			{
				int g = (x%4 ? 2 : 0) + (x%2 ? 2 : 0) + (y%2 ? 2 : 0) + (y%4 ? 2 : 0);
				d->setClr(29-g*2, 29-g, 29-g/2);
				d->setCursor(x*30, H-1 -40 + (y-4)*16);

				sprintf(a,"%02X", remoteData[s++]);  d->print(a);
		}	}
	#else
		d->setCursor(10, 110);
		d->print("Not present");
	#endif

	}	break;

	}
}
