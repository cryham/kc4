#include "gui.h"
#include "kc_params.h"

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Help
//....................................................................................
void Gui::DrawHelp()
{
	char a[32];
	
	DrawTitle(strMain[ym], RGB(21,26,31), &bmpHELP);
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

	const int x = W/3 +30;
	d->setClr(25,28,31);  d->setCursor(x, 2);  d->print(title[hpage]);
	d->setClr(21,24,28);  d->setCursor(x,20);  d->print(title2[hpage]);
	
	auto yadd = [&](int16_t y){  d->moveCursor(0, y*2);  };

	//  text
	d->setCursor(0, yTitleUp);
	switch (hpage)
	{
	case 0:  //  main
		d->setClr(20,26,31);
		d->println("^,v        Move Cursor");
		yadd(2);
		d->println("> or Ent        Enter");
		yadd(2);
		d->println("< or Bksp or +    Go Back");

		yadd(8);
		d->setClr(16,22,28);
		d->println("PgUp,PgDn    Prev, Next Page");
		yadd(2);
		d->println("<,>     Dec, Inc Value");
		d->setClr(12,18,24);
		d->println("   LCtrl- more   LShift- less");

		yadd(6);
		d->setClr(16,22,28);
		d->println("F4   Save");
		d->println("F5   Load    Ctrl- Reset");
		break;

	case 1:  //  quic

		d->setClr(16,22,28);
		d->println("F1   Mapping    /=Shift");
		d->println("F2   Sequences");
		d->println("F3   Test Layout  /Pressed");
		yadd(4);
		d->println("F6   Info Use");
		d->println("F7   Setup Mouse");
		d->println("F8   Setup Layer  /Keyboard");
		yadd(4);
		d->println("F9   Demos Plasma  /Hedrons");
		d->println("F10   Help");
		yadd(4);
		d->println("F11   Display");
		d->println("F12   Clock  /Stats  /Graphs");
		break;

	case 2:  //  mapping  ------------------------
		d->setClr(25,28,23);
		d->println("^,v <,>    Move Cursor");
		yadd(2);
		d->println("/    Press key, set cursor");
		yadd(6);
		d->println("PgUp,PgDn   Prev / Next Layer");
		//yadd(2);
		d->setClr(17,24,17);
		d->println("   Last is layer use, visual");

		yadd(6);
		d->setClr(25,28,23);
		d->println("- or Ent   Pick key list");
		d->println("Ctrl-Ent   Go to sequence");
		yadd(6);
		d->println("C,V,X      Copy, Paste, Swap");
		//yadd(2);
		d->println("Ctrl-Del   Delete key");
		yadd(4);
		d->setClr(17,24,17);
		d->print("Ctrl-V   Paste whole layer");
		break;

	case 3:  //  pick key
		d->setClr(23,28,23);
		d->println("^,v PgUp,PgDn <,>   Move");
		yadd(6);
		d->println("+ or Ent    Set key");
		yadd(2);
		d->println("- or Bksp   Cancel");

		yadd(6);
		d->setClr(21,25,21);
		d->println("/    Group filter toggle");
		yadd(2);
		d->println("<,>   Prev, Next Group");
		break;

	case 4:  //  sequences
		d->setClr(21,28,28);
		d->println("^,v PgUp,PgDn   Move");
		yadd(2);
		d->println("Home,End   Move 3x");
		yadd(6);
		sprintf(a,"L%d-Enter   Edit sequence", par.editLayer);
		d->println(a);
		d->println("Ctrl-Ent   Go to mapping");

		yadd(6);
		d->println("C,V,X      Copy, Paste, Swap");
		//yadd(2);
		d->println("Ctrl-Del   Delete");
		break;

	case 5:  //  edit seq
		d->setClr(6,30,30);
		sprintf(a,"All on Layer %d:", par.editLayer);
		d->println(a);
		d->setClr(18,29,29);
		yadd(6);
		d->println("<,>   Move Cursor  Ctrl- 4x");
		yadd(2);
		d->println("Home,End   To Start, End");
		yadd(6);
		d->println("Del,Bksp   Delete");
		d->println("Ins      Insert / Overwrite");
		yadd(6);
		d->println("Enter      Exit Edit");
		break;

	case 6:  //  seq commands ___ help
		d->setClr(25,26,26);
		d->println("Insert command in sequence:");
		yadd(6);
		d->println("/   Set Key Delay [1ms]");
		yadd(2);
		d->println("*   Wait [0.1s]");

		yadd(6);
		d->println("C   Comment start / end {}");
		//yadd(2);
		d->println("X   Hide> rest of sequence");
		yadd(2);
		d->println("V   Run other sequence");
		yadd(2);
		d->println("-   Repeat sequence, Gui quits");
		yadd(6);
		d->println("^,v   Adjust parameter");
		break;

	case 7:  //  seq _mouse commands_ help
		d->setClr(24,25,23);
		d->println("Shift-  Insert command:");
		yadd(6);
		d->println("<,> ^,v   Move   Ctrl- big");
		yadd(4);
		d->println("  Buttons:");
		d->println("Home  Click    End   Double");
		d->println("PgDn  Press    PgUp  Release");
		yadd(4);
		d->println("/,*   Wheel move horiz, vert");
		yadd(2);
		d->println("/,*,-   Adjust x, y, Set xy-");
		yadd(6);
		d->println("^,v   Adjust parameter");
		break;

	case 8:  //  demos  ------------------------
		d->setClr(26,26,29);
		d->println("<,>   Prev / Next Preset");
		d->println("^,v   Dec, Inc Speed");
		yadd(4);
		d->println("-     Fps show, mode");
		d->println("*     Params Info");

		yadd(8);
		d->setClr(22,22,25);
		d->println("PgUp,PgDn   Change");
		d->println("Home,End    Params");
		yadd(2);
		d->println("   Ctrl-  Others");
		d->println("   Shift-  Fine");
		break;

	case 9:  //  sixtis  ------------------------
		d->setClr(29,26,23);
		d->println("<,>      Move");
		yadd(6);
		d->println("^        Rotate clockwise");
		yadd(2);
		d->println("5 or /   Rotate ccw");
		yadd(6);
		d->println("v        Fall down");
		yadd(2);
		d->println("Ins      Drop");
		break;

	case 10:  //  sixtis cd
		d->setClr(29,25,20);
		d->println("+ or Bksp   Back");
		yadd(2);
		d->println("-        Pause");
		yadd(2);
		d->println("Enter    New Game");
		yadd(6);
		d->println("*        Options toggle");
		yadd(2);
		d->setClr(25,21,16);
		d->println("<,>      Dec, Inc Value");
		d->println("PgUp,PgDn   Page");
		break;
	}
}
