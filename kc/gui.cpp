#include "gui.h"
#include "def.h"


//  Main  ----
Gui::Gui() : d(0)
{
	Init(0);
}

void Gui::Init(ILI9341_t3n* tft)
{
	d = tft;

#ifdef DEMOS
	demos.Init(d);
#endif
}
