#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kc_data.h"


//  Testing
//....................................................................................
void Gui::DrawTesting()
{
	//  menu
	if (mlevel == 1)
	{
		DrawTitleMain(RGB(16,26,16), &bmpZOOM);
		DrawMenu(Ts_All,strTest,0, c_Test,RGB(22,31,14),RGB(5,9,6));
		return;
	}
	char a[64];

	//  title
	DrawTitle(strTest[yy], RGB(12,20,28), &bmpZOOM);
	d->setFont(OpenSans12);
	d->setClr(21,26,31);

	switch (yy)
	{
	//-----------------------------------------------------
	case Ts_Pressed:
	{
		d->setCursor(0,yTitle);
		d->setFont(OpenSans14);
		//  layer  -
		d->setClr(20,23,31);
		sprintf(a,"Layer:  %d %s", kc.nLayer,
				   kc.nLayerLock >= 0 ? "Lock" : "");
		d->setClr(24,27,31);
		d->print(a);
		d->println("");  d->moveCursor(0,8);

		//  todo- locks  -
		//d->setClr(18,21,24);
		//d->print("Locks: ");  // Num Caps Scrl
		//d->println("");  d->moveCursor(0,2);

		//  modifiers  -
		d->setClr(18,21,24);
		d->print("Modif:  ");
		d->setClr(20,25,28);
		for (uint i = 0; i < ScanKeys; ++i)
		{
			const KeyState& k = Matrix_scanArray[i];
			if (kc.set.nkeys() >= int(ScanKeys))
			if (k.state == KeyState_Hold)
			{
				uint8_t code = kc.set.key[kc.nLayer][i];
				if (code > KEY_NONE && code <= K_ModLast)
				{	sprintf(a," %s",cKeyStr[code]);
					d->print(a);
		}	}	}
		d->println("");  d->moveCursor(0,8);


		//  scan codes  - - -
		d->setClr(17,23,26);
		d->print("Scan:  ");
		d->setClr(15,20,25);
		int c = 0;
		for (uint i = 0; i < ScanKeys; ++i)
		{
			const KeyState& k = Matrix_scanArray[i];
			if (k.state == KeyState_Hold)
			{
				sprintf(a," %d",i);  // scan code
				d->print(a);  ++c;
		}	}
		d->println("");  d->moveCursor(0,10);


		//  keys  - - - -
		DrawPressed(true);

		//  held count
		d->setFont(OpenSans14);
		d->setCursor(0, H-1-20);
		d->setClr(16+c, min(31,24+c), 31);
		sprintf(a,"Held:  %d", c);
		d->print(a);

		if (ghost_cols)  // ghost
		{	d->setCursor(90, H-1-40);
			d->setClr(min(31,24+c), 18+c, 31);
			sprintf(a,"Ghost:  %d %d", ghost_cols, ghost_rows);
			d->print(a);
		}
		//  press count -
		d->setCursor(90, H-1-20);
		d->setClr(21,21,27);
		sprintf(a,"Press:  %d", cnt_press); //, cnt_hold % 1000);
		d->print(a);

	}	break;


	//-----------------------------------------------------
	case Ts_Layout:
	{
		d->setFont(OpenSans12);
		d->setCursor(0, yTitleUp);
		
		//  layer  -
		d->setClr(20,23,31);
		sprintf(a,"Layer:  %d %s", kc.nLayer,
				   kc.nLayerLock >= 0 ? "Lock" : "");
		d->print(a);
		d->setCursor(0, d->getCursorY() + 20);

		//  keys  - - - -
		DrawPressed();

		DrawLayout(false);
	}	break;
	}
}


//  pressed keys list
//....................................................................................
void Gui::DrawPressed(bool far)
{
	int8_t seq=-1, fun=-1;
	d->setClr(20,25,28);
	d->print("Keys:  ");
	d->setFont(OpenSans12);

	if (kc.set.nkeys() >= int(ScanKeys))
	for (uint i = 0; i < ScanKeys; ++i)
	{
		const KeyState& ks = Matrix_scanArray[i];
		if (ks.state == KeyState_Hold)
		{
			uint8_t k = kc.set.key[kc.nLayer][i];
			if (k > K_ModLast && k < KEYS_ALL_EXT/*KEYS_ALL*/)
			{
				const uint8_t* c = &cGrpRgb[cKeyGrp[k]][0][0];
				d->setClr(c[0],c[1],c[2]);
				d->print(" ");  d->print(cKeyStr[k]);

				if (k >= K_Seq0 && k <= K_SeqLast)  seq = k - K_Seq0;  else
				if (k >= K_Fun0 && k <= K_FunLast)  fun = k - K_Fun0;
	}	}	}

	d->setCursor(20, d->getCursorY() + (far ? 30 : 16));

	if (seq >= 0)  //  seq preview  ---
		DrawSeq(seq, 2, 0);
	else
	if (fun >= 0)
		d->print(cFunStr[fun]);
}
