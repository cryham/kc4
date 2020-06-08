#include "gui.h"
#include "ILI9341_t3n.h"


//  Draw  main
//....................................................................................
void Gui::Draw()
{
	yy = D_Hedrons;  //D_CK_Logo;  // test

	//  Clear  if not full screen demo
	bool no = (yy == D_Rain || yy == D_Plasma || yy == D_Wave || yy == D_Fire);
	if (!no)
		d->fillScreen(ILI9341_BLACK);
		//memset(demos.data, 0, 2*W*H);

	//  Demos
	//------------------------------------------------------
	switch (yy)
	{
		case D_Plasma:   demos.Plasma();  break;
		case D_Wave:     demos.Wave();  break;
		case D_Fire:     demos.Fire();  break;
		case D_CK_Logo:  demos.CK_logo();  break;
		case D_Hedrons:  demos.Hedrons();  break;
	#ifdef DEMOS_OLD_PAR
		case D_Fonts:	 demos.Fonts();  break;
		case D_Ngons:    demos.Ngons();  break;

		case D_Fountain: demos.Fountain();  break;
		case D_Balls:    demos.Balls();  break;
		case D_Space:    demos.Space();  break;
	#endif
		case D_Rain:     demos.Rain();  break;
	}

	//  Auto next
	if (demos.bAuto)
	if (demos.t % 1000 == 0)
		NextDemo();
}

void Gui::NextDemo()
{
	++yy;
	if (yy >= D_All)
		yy = 0;
}
