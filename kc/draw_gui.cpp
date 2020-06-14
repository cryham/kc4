#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Draw  main
//....................................................................................
void Gui::Clear()
{
	yy = ym1[ym];

	//  Clear  if not full screen demo
	bool demo = ym == M_Demos && mlevel == 2;
	bool no = demo && (yy == D_Rain || yy == D_Plasma || yy == D_Wave || yy == D_Fire);
	if (!no)
	{
		d->waitUpdateAsyncComplete();
		//d->fillScreen(ILI9341_BLACK);
		memset(demos.data, 0, 2*W*H);
	}
	if (!demo)
	{
		//d->setFont(&FreeSans9pt7b);
		d->setFont(OpenSans12);
		d->setCursor(0,4);
	}
}


//  Draw  main
//....................................................................................
void Gui::Draw()
{
	d->setFont(OpenSans20);  // for title

	//  Main menu
	//------------------------------------------------------
	if (mlevel==0)
	{
		d->setClr(6,19,31);
		d->print("Main Menu");

		DrawMenu(M_All,strMain, C_Main,RGB(20,25,29),RGB(5,7,9), M_Next);
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
			demos.Draw(yy);
		}else
		{	//  menu
			d->setClr(25,16,28);
			d->print(strMain[ym]);

			DrawMenu(D_All,strDemo, C_Demos,RGB(27,27,30),RGB(6,6,9), D_Next);
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
	case M_Info:      DrawInfo();  return;

	//  Display, Help
	case M_Display: DrawDisplay();  return;
	case M_Clock:   DrawClock();  return;
	case M_Help:    DrawHelp();  return;
	}
}
