#include "gui.h"
#include "kc_params.h"

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Help
//....................................................................................
void Gui::DrawHelp()
{
	char a[32];
	
	DrawTitleMain(RGB(21,26,31), &bmpHELP);
	d->setFont(OpenSans12);

	//  page / all
	d->setClr(22,22,23);
	d->setCursor(W-1 -44, 8);
	sprintf(a, "%2d/%d", hpage+1, HAll);
	d->print(a);

	//  titles
	const static char* title[HAll] = {"Main", "Quick",
		"Mapping", "Mapping",
		"Sequences", "Sequence", "Sequence", "Sequence",
		"Demos", "Sixtis", "Sixtis"},

	*title2[HAll] = {"", "Access",  // 2nd lines
		"", "Pick Key",
		" View", " Edit", "Commands", "Mouse",
		"", " 1/2", " 2/2"};

	int x = W/3 +30, x2, y = yTitleUp, yl;
	d->setClr(25,28,31);  d->setCursor(x, 2);  d->print(title[hpage]);
	d->setClr(21,24,28);  d->setCursor(x,20);  d->print(title2[hpage]);
	
	auto yadd = [&](int16_t ya){  y += ya*2;  };
	auto print1 = [&](const char* s)
	{	d->setCursor(x, y);  d->print(s);  yadd(yl);
	};
	auto print = [&](const char* s, const char* s2)
	{	//d->setCursor(x, y);  d->print(s);
		PrintR(s, x2 - 20, y);
		d->setCursor(x2, y);  d->print(s2);  yadd(yl);
	};

	//  text
	d->setCursor(0, yTitleUp);
	x = 0;  x2 = W*4/10;  yl = 9;  // defaults
	switch (hpage)
	{
	case 0:  //  Main  ------------------------
		d->setClr(20,26,31);
		print("^,v", "Move Cursor");
		print("> or Ent", "Enter");
		print("< or + or Bksp", "Go Back");  yadd(8);

		d->setClr(16,22,28);
		print("PgUp,PgDn", "Prev, Next Page");
		print("<,>", "Dec, Inc Value");
		d->setClr(12,18,24);  x = W/4;
		print("", "LCtrl- more   LShift- less");  yadd(8);

		d->setClr(16,22,28);
		print("F4", "Save");
		print("F5", "Load    Ctrl- Reset");
		break;

	case 1:  //  Quick
		d->setClr(16,22,28);  x2 = 80;  yl -= 1;
		print("F1", "Mapping     /=Shift");
		print("F2", "Sequences");
		print("F3", "Test Layout    /Pressed");
		yadd(4);
		print("F6", "Confg Storage    /Use");
		print("F7", "Setup Mouse");
		print("F8", "Setup Layer    /Keyboard");
		yadd(4);
		print("F9", "Demos Plasma    /Hedrons");
		print("F10", "Help");
		yadd(4);
		print("F11", "Display");
		print("F12", "Clock    /Stats    /Graphs");
		break;

	case 2:  //  Mapping  ------------------------
		d->setClr(25,28,23);  yl -= 1;
		print("^,v <,>", "Move Cursor");  //yadd(2);
		print("/", "Press key, set cursor");  yadd(6);
		print("PgUp,PgDn", "Prev / Next Layer");
		d->setClr(19,24,17);  x = W/4;
		print("", "Last is layer use, visual");  yadd(6);

		d->setClr(20,31,15);
		print("- or Ent", "Pick key");
		d->setClr(25,28,23);
		print("Ctrl-Ent", "Go to sequence");  yadd(6);

		print("C,V,X", "Copy, Paste, Swap");
		print("Ctrl-Del", "Delete key");  yadd(2);
		d->setClr(19,24,17);
		print("Ctrl-V", "Paste whole layer");
		break;

	case 3:  //  Pick key
		d->setClr(23,28,23);
		print("^,v", "Move");
		print("Home,End", "Move 4x");
		print("PgUp,PgDn", "Page");  yadd(6);

		d->setClr(15,31,15);
		print("+ or Ent", "Set key");
		d->setClr(21,25,21);
		print("- or Bksp", "Cancel");  yadd(6);

		d->setClr(21,25,21);
		print("/", "Group filter toggle");
		print("<,>", "Prev, Next Group");
		break;

	case 4:  //  Sequences  ------------------------
		d->setClr(21,28,28);
		print("^,v", "Move");
		print("PgUp,PgDn", "Page");
		print("Home,End", "Move 3x");  yadd(6);
		
		sprintf(a,"L%d-Enter", par.editLayer);
		d->setClr(20,30,30);
		print(a, "Edit sequence");
		d->setClr(21,28,28);
		print("Ctrl-Ent", "Go to mapping");  yadd(6);

		print("C,V,X", "Copy, Paste, Swap");
		print("Ctrl-Del", "Delete");
		break;

	case 5:  //  Edit Sequence
		d->setClr(6,30,30);
		sprintf(a,"All on Layer %d:", par.editLayer);
		x = 0;  print1(a);  yadd(6);

		d->setClr(18,29,29);
		print("<,>", "Move Cursor  Ctrl- 4x");
		print("Home,End", "To Start, End");  yadd(6);

		print("Del,Bksp", "Delete");
		print("Ins", "Insert / Overwrite");  yadd(6);

		d->setClr(13,30,30);
		print("Enter", "Exit Edit");
		break;

	case 6:  //  Seq Commands ___ help
		d->setClr(25,26,26);  x = 0;
		print1("Insert command in sequence:");  yadd(4);
		print("/", "Set Key Delay   D 1ms");
		print("*", "Wait   W 0.1s");  yadd(4);

		print("C", "Comment start / end  {}");
		print("X", "Hide rest   H>");  yadd(4);
		
		print("V", "Run other sequence  S");
		print("-", "Repeat self  Rpt");  yadd(4);

		print("^,v", "Adjust parameter");
		break;

	case 7:  //  Seq _Mouse Commands_ help
		d->setClr(24,25,23);  x = 0;
		print1("Shift-  Insert command:");  yadd(4);
		print("<,> ^,v", "Move   Ctrl- big");  yadd(4);
		print1("Buttons:");
		print("Home  Click", " End   Double");
		print("PgDn  Press", " PgUp  Release");  yadd(4);

		print("/,*", "Wheel move horiz, vert");
		print("/,*,-", "Adjust x, y, Set xy-");  yadd(4);

		print("^,v", "Adjust parameter");
		break;

	case 8:  //  Demos  ------------------------
		d->setClr(26,26,29);
		print("<,>", "Prev / Next Preset");
		print("^,v", "Dec, Inc Speed");  yadd(4);
		print("-", "Fps show, mode");
		print("*", "Params Info");  yadd(8);

		d->setClr(22,22,25);
		print("PgUp,PgDn", "");
		print("Home,End", "Change Params");  yadd(2);
		print("Ctrl-", "Others");
		print("Shift-", "Fine");
		break;

	case 9:  //  sixtis  ------------------------
		d->setClr(29,26,23);
		print("<,>", "Move");  yadd(6);
		print("^", "Rotate clockwise");
		print("5 or /", "Rotate ccw");  yadd(6);
		print("v", "Fall down");
		print("Ins", "Drop");
		break;

	case 10:  //  sixtis cd
		d->setClr(29,25,20);
		print("+ or Bksp", "Back");
		print("-", "Pause");
		print("Enter", "New Game");  yadd(6);
		print("*", "Options toggle");

		d->setClr(25,21,16);
		print("<,>", "Dec, Inc Value");
		print("PgUp,PgDn", "Page");
		break;
	}
}
