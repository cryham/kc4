#include "gui.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

#include "matrix.h"
#include "kc_data.h"
#include "kbd_layout.h"

//  extern
extern uint scan_freq;  // scan counter, freq
extern uint32_t us_scan;


//  Matrix
//....................................................................................
void Gui::DrawMatrix()
{
	//  menu
	if (mlevel == 1)
	{
		DrawTitleMain(RGB(20,20,26), &bmpMATRIX);
		DrawMenu(X_All,strMatrix,0, C_Matrix,RGB(22,22,31),RGB(5,5,9));
		return;
	}

	//  title
	DrawTitle(strMatrix[yy], RGB(20,20,28), &bmpMATRIX);
	d->setFont(OpenSans12);
	d->setClr(24,24,31);


	char a[64],b[64];
	int16_t y = yTitle, x0 = W / 2, x1 = x0 + 14;
	auto yadd = [&y](int16_t h){  y += h*2;  };

	uint16_t bck = RGB(4,4,8);

	auto DrawCursor = [&](auto clr)
	{
		d->setColor(clr, bck);
		d->fillRect(0, y-3, W, 21, bck);
		d->setCursor(4,y);
		d->print(">");
	};

	switch (yy)
	{
	//-----------------------------------------------------
	case X_Test:
	{
		const uint x1 = 13, y1 = 122, w = 14, h = 14;
		uint c,r, hc=99,hr=99;  // held col,row
		uint16_t clr;

		//  grid #
		const uint16_t cl[3] = {RGB(9,10,10), RGB(13,14,14), RGB(16,20,20)};
		uint ww = min(NumCols*w, W-1-x1);
		uint hh = min(NumRows*h, H-1-y1);
			
		for (r=0; r < NumRows; ++r)
			if (r % 2 == 1)
				d->drawFastHLine(x1, y1 + r*h +4, ww, cl[r % 4 == 3 ? 1 : 0]);
		for (c=0; c < NumCols; ++c)
			if (c % 2 == 1)
				d->drawFastVLine(x1 + c*w +3, y1, hh-3, cl[c%8==7 ? 2 : c%4==3 ? 1 : 0]);
		d->setFont(0);

		//  matrix  :::  *
		bool ghost = false;
		for (c=0; c < NumCols; ++c)
		for (r=0; r < NumRows; ++r)
		{
			d->setCursor(x1 + c*w, y1 + r*h);
			int sc = NumCols * r + c;
			const KeyState& k = Matrix_scanArray[sc];

			//  color from ghost, use
			#define CGh(gh, u)  clr = gh ? RGB(31,26,12) : \
				RGB( min(31,7+u*5), min(31, 14+u*8), max(4, 24-u*2) ); \
				d->setColor(clr, clr)

			a[1]=0;
			if (k.state == KeyState_Off)
			{	//sprintf(a,".");
				a[0]=2;
				bool gh = col_ghost[c] || row_ghost[r];
				int u = max(col_use[c], row_use[r]);
				CGh(gh, u);
				ghost |= gh;
			}else{
				hc=c; hr=r;  // held
				a[0]='*';
				//sprintf(a,"%d", k.state);
				d->setClr(29,30,31);
			}
			if (!kc.matrixVis[sc])
				a[0]='X';  // unavailable scan id
			d->print(a);
		}

		//  col| row- use
		d->setFont(OpenSans12);
		for (c=0; c < NumCols; ++c)
		{
			CGh(col_ghost[c], col_use[c]);
			d->setCursor(x1 + c*w - 1, y1 - x1 - 6);
			d->print(col_use[c]);
		}
		for (r=0; r < NumRows; ++r)
		{
			CGh(row_ghost[r], row_use[r]);
			d->setCursor(0, y1 + r*h - 3);
			d->print(row_use[r]);
		}

		//  held  ---
		y = yTitleUp;
		d->setCursor(0, y);
		d->setClr(24,24,31);
		sprintf(a,"Held:  %d   press:  %d ", cnt_press-cnt_rel, cnt_press);
		d->print(a);
		if  (hc<99 && hr<99)
		{
			d->setCursor(W*2/3, y);
			sprintf(a,"R%d  C%d", hr+1, hc+1);
			d->print(a);
		}

		//  ghosting  ---
		d->setCursor(0, y+20);
		d->setColor(ghost ? RGB(31,26,12) : RGB(16,21,26));
		sprintf(a,"Ghost   col %d  row %d", ghost_cols, ghost_rows);
		d->print(a);

	}	break;


	//-----------------------------------------------------
	case X_Scan:
	{
		for (int i=0; i <= 2; ++i)
		{
			int c = abs(i - ym2Scan);
			if (!c)
				DrawCursor(RGB(24,22,28));
			d->setCursor(20,y);

			FadeClr(C_Matrix, 4, c, 1, !c ? bck : 0);
			switch(i)
			{
			case 0:
				strcpy(a,"Frequency:");
				sprintf(b,"%u Hz", par.scanFreq * 20);  break;
			case 1:
				strcpy(a,"Strobe delay:");
				sprintf(b,"  %d us", par.strobe_delay);  break;
			case 2:
				strcpy(a,"Debounce:");
				sprintf(b,"  %d ms", par.debounce);  break;
			}
			PrintR(a, x0, y);
			d->setCursor(x1, y);  d->print(b);  yadd(8+4);
		}

		d->setClr(23,22,25);
		d->setCursor(W-1-44,H-32);
		d->print("Fps");

		y = H-100;
		sprintf(a,"Time:  %lu us", us_scan);  PrintR(a, x0, y);
		sprintf(a,"%u Hz", scan_freq);  d->setCursor(x1, y);  d->print(a);

		y = H-20;  x0 = W / 3;  x1 = x0 + 14;
		d->setClr(22,20,26);
		sprintf(a,"Matrix keys:");  PrintR(a, x0, y);
		sprintf(a,"%d = %d x %d", ScanKeys, NumCols, NumRows);  d->setCursor(x1, y);  d->print(a);

		y = H-44;
		sprintf(a,"Layout keys:");  PrintR(a, x0, y);
		sprintf(a,"%d  %s", nDrawKeys, CKname);  d->setCursor(x1, y);  d->print(a);

	}	break;
	}
}
