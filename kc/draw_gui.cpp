#include "gui.h"
#include "kc_data.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Draw  main
//....................................................................................
void Gui::Clear(void* buf)
{
	yy = ym1[ym];
	demos.data = (uint16_t*)buf;  // change if double buffer

	//  Clear  if not full screen demo
	bool demo = ym == M_Demos && mlevel == 2;
	bool no = demo && FullScrDemo();
	if (!no)
	{
		#ifndef BUFx2
		d->waitUpdateAsyncComplete();
		#endif
		memset(buf, 0, 2*W*H);
		//d->fillScreen(ILI9341_BLACK);
	}
	if (!demo)
	{
		d->setFont(OpenSans12);
		d->setCursor(0,4);
	}
}

bool Gui::Force1Buf()
{
	bool demo = ym == M_Demos && mlevel == 2;
	return demo && yy == D_Rain;
}

bool Gui::FullScrDemo()
{
	return yy == D_Plasma || yy == D_Fractal || yy == D_Wave || yy == D_Fire || yy == D_Rain;
}


//  Draw  main
//....................................................................................
void Gui::Draw()
{
	if (kc.forceClock)
	{	DrawClock();  return;  }
	

	d->setFont(OpenSans20);  // for title

	//  Main menu
	//------------------------------------------------------
	if (mlevel==0)
	{
		d->setClr(6,19,31);
		d->print("Main Menu");

		DrawMenu(M_All,strMain,&bmpMain[0], c_Main,RGB(20,25,29),RGB(5,7,9),
			M_Next, M_Testing,M_Help);
		return;
	}
	d->setClr(12,22,31);


	switch (ym)
	{
	//  Demos
	//------------------------------------------------------
	case M_Demos:
		if (mlevel == 2)
		{
			d->setFont(0);
			if (demos.Draw(yy))
				NextDemo();
		}else
		{	//  menu
		DrawTitleMain(RGB(25,16,28), &bmpDEMO);
			DrawMenu(D_All,strDemo,0, c_Demos,RGB(27,27,30),RGB(6,6,9),
				D_Next, D_AutoAll);
		}
		return;

	#ifdef GAME   // game
	case M_Game:  game.Draw();  return;
	#endif

	//  Mappings,Seq  kbd
	case M_Mapping:   DrawMapping();  return;
	case M_Sequences: DrawSequences();  return;
	//  Testing,Setup  kbd
	case M_Testing:   DrawTesting();  return;
	case M_Setup:     DrawSetup();  return;
	case M_Matrix:    DrawMatrix();  return;
	case M_Config:    DrawConfig();  return;

	//  Display, Help
	case M_Display: DrawDisplay();  return;
	case M_Clock:   DrawClock();  return;
	case M_Help:    DrawHelp();  return;
	}
}
