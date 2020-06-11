#include "gui.h"
#include "ILI9341_t3n.h"


//  Draw  main
//....................................................................................
void Gui::Clear()
{
	//yy = D_CK_Logo;  // test

	//  Clear  if not full screen demo
	bool no = (yy == D_Rain || yy == D_Plasma || yy == D_Wave || yy == D_Fire);
	if (!no)
	{
		d->waitUpdateAsyncComplete();
		//d->fillScreen(ILI9341_BLACK);
		memset(demos.data, 0, 2*W*H);
	}
}

void Gui::Draw()
{
	demos.Draw(yy);
}
