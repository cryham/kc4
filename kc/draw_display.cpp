#include "gui.h"
#include "kc_data.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


const char* sPgDisplay[Di_All] = {
	"Brightness", "Gui keys", "Intervals", "Temp adjust" };

//  Display
//....................................................................................
void Gui::DrawDisplay()
{		
	char a[64],b[64];
	DrawTitleMain(RGB(29,28,6), &bmpSCREEN);
	d->setFont(OpenSans12);

	//  subtitle
	d->setCursor(W/2 +6, 8);
	d->setClr(30,22,12);
	d->print(sPgDisplay[pgDisp]);

	//  page / all
	d->setCursor(W-1 -24, 8);
	d->setClr(28,22,10);
	sprintf(a,"%d/%d", pgDisp+1, Di_All);
	d->print(a);

	//  par values  ---
	int pg = DispPages[pgDisp];
	int16_t y = yTitle;
	auto yadd = [&y](int16_t h){  y += h*2;  };
	const int x0 = W / 2, x1 = x0 + 14;

	switch (pgDisp)
	{
	case Di_Bright:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);  int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"In GUI:");
			sprintf(b,"%d %%", par.brightness);  h = 2;  break;
		case 1:
			sprintf(a,"Off GUI:");
			sprintf(b,"%d %%", par.brightOff);  break;
		case 2:
			sprintf(a,"Lamp:");
			sprintf(b,"%s",
			#ifdef LED_LAMP
				kc.ledOn ? "on" :
			#endif
				"off");  h = 2;  break;
		case 3:
			sprintf(a,"Lamp:");
			sprintf(b,"%d %%", int(100.f * par.ledBright / 64.f));  break;
		}
		PrintR(a, x0, y);
		d->setCursor(x1, y);  d->print(b);  yadd(h+8);
	}	break;

	case Di_Keys:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);  int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Start screen:");
			sprintf(b,"%s", StrScreen(par.startScreen));  break;
		case 1:
			sprintf(a,"Key delay:");
			sprintf(b,"%d ms", par.krDelay*5);  h = 2;  break;
		case 2:
			sprintf(a,"Key repeat:");
			sprintf(b,"%d ms", par.krRepeat*5);  break;
		case 3:
			sprintf(a,"Quick keys F1-12:");
			sprintf(b,"%s", par.quickKeys ? "on" : "off");  break;
		}
		PrintR(a, x0, y);
		d->setCursor(x1, y);  d->print(b);  yadd(h+8);
	}	break;

	case Di_StatsGraph:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);  int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Press/min:");  break;
		case 1:
			sprintf(a,"Inactive after:");
			sprintf(b,"%d min", par.minInactive);  break;
		case 2:
			sprintf(a,"Temp read:");  h = 2;  break;
		case 3:
			sprintf(a,"Graph add:");  break;
		case 4:
			sprintf(a,"Daily graph:");
			sprintf(b,"%d hours", par.dailyHours);  break;
		}
		PrintR(a, x0, y);  d->setCursor(x1, y);
		switch(i)
		{
		case 0:  PrintInterval(t1min(par));  break;
		case 2:  PrintInterval(tTemp(par));  break;
		case 3:  PrintInterval(tTgraph(par));  break;
		default:  d->print(b);  break;
		}
		yadd(h+8);
	}	break;

	case Di_AdjustTemp:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);  int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Frames per sec:");
			switch(demos.iFps)
			{
			case 0:  strcpy(b,"off");  break;
			case 1:  strcpy(b,"in demos");  break;
			case 2:  strcpy(b,"always");  break;
			}	break;
		case 1:
			sprintf(a,"Temp offset:");
			dtostrf(0.03f * par.tempOfs, 4,2, b);
			strcat(b," ""\x01""C");  break;
		case 2:
			sprintf(a,"Temp min:");
			sprintf(b,"%d ""\x01""C", par.minTemp);  h = 2;  break;
		case 3:
			sprintf(a,"Temp max:");
			sprintf(b,"%d ""\x01""C", par.maxTemp);  break;
		}
		PrintR(a, x0, y);  d->setCursor(x1, y);
		d->print(b);  yadd(h+8);
	}	break;
	}
}
