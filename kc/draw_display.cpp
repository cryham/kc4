#include "gui.h"
#include "kc_data.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


const char* sPgDisplay[Di_All] = {
	"Bright", "Gui keys", "Intervals", "Graph""\x01""C", "Debug" };

//  Display
//....................................................................................
void Gui::DrawDisplay()
{		
	char a[64],b[64];
	DrawTitle(strMain[ym], RGB(29,28,6), &bmpSUN);
	d->setFont(OpenSans12);

	//  subtitle
	d->setCursor(W/2 -6, 4);
	d->setClr(30,22,12);
	d->print(sPgDisplay[pgDisp]);

	//  page / all
	d->setCursor(W-1 -24, 4);
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
		DrawDispCur(i, y);
		int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Brightness:");
			sprintf(b,"%d %%", par.brightness);  h = 2;  break;
		case 1:
			sprintf(a,"Off bright:");
			sprintf(b,"%d %%", par.brightOff);  break;
		case 2:
			sprintf(a,"Start with:");
			sprintf(b,"%s", StrScreen(par.startScreen));  break;
		}
		PrintR(a, x0, y);
		d->setCursor(x1, y);  d->print(b);  yadd(h+8);
	}	break;

	case Di_Key:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);
		int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Key delay:");
			sprintf(b,"%d ms", par.krDelay*5);  h = 2;  break;
		case 1:
			sprintf(a,"Key repeat:");
			sprintf(b,"%d ms", par.krRepeat*5);  break;
		case 2:
			sprintf(a,"Quick keys F1-12:");
			sprintf(b,"%s", par.quickKeys?"on":"off");  break;
		}
		PrintR(a, x0, y);
		d->setCursor(x1, y);  d->print(b);  yadd(h+8);
	}	break;

	case Di_Stats:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);
		int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Time for 1min:");
			sprintf(b,"%dm %02ds", t1min(par)/60, t1min(par)%60);  break;
		case 1:
			sprintf(a,"Inactive after:");
			sprintf(b,"%d min", par.minInactive);  break;
		}
		PrintR(a, x0, y);
		d->setCursor(x1, y);  d->print(b);  yadd(h+8);
	}	break;

	case Di_Graph:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);
		int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Temp read:");  h = 2;  break;
		case 1:
			sprintf(a,"Graph add:");  break;
		case 2:
			sprintf(a,"T min:");
			sprintf(b,"%d ""\x01""C", par.minTemp);  h = 2;  break;
		case 3:
			sprintf(a,"T max:");
			sprintf(b,"%d ""\x01""C", par.maxTemp);  break;
		}
		PrintR(a, x0, y);  d->setCursor(x1, y);
		switch(i)
		{
		case 0:  PrintInterval(tTemp(par));  break;
		case 1:  PrintInterval(tTgraph(par));  break;
		default:  d->print(b);
		}
		yadd(h+8);
	}	break;

	case Di_Debug:
	for (int i=0; i <= pg; ++i)
	{
		DrawDispCur(i, y);
		int8_t h = 4;
		switch(i)
		{
		case 0:
			sprintf(a,"Frames per sec:");
			sprintf(b,"%d", demos.iFps);
			PrintR(a, x0, y);
			d->setCursor(x1, y);
			d->print(b);  yadd(h+8);
			break;
		case 1:
			sprintf(a,"Temp offset:");
			PrintR(a, x0, y);  d->setCursor(x1, y);  yadd(h+8);
			dtostrf(0.03f * par.tempOfs, 4,2, a);
			d->print(a);  d->print(" ""\x01""C");
			break;
		}
	}
	}
}
