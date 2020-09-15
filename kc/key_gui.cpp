#include "gui.h"
#include "matrix.h"
#include "kbd_layout.h"
#include "keys_usb.h"
#include "kc_params.h"
#include "kc_data.h"


//  Key press
//....................................................................................
void Gui::KeyPress()
{
	if (Key(par.keyGui))  // toggle Gui **
	{
		kbdSend = 1 - kbdSend;
		kc.QuitSeq();
		kc.setBright = 1;
	}

	if (kbdSend)
		return;

	uint32_t ti = millis();
	uint16_t dt = ti - oldti_kr;
	oldti_kr = ti;

	//  update keys press  _k_
	kRight= kr(gRight,dt) - kr(gLeft,dt);
	kUp   = kr(gDown,dt) - kr(gUp,dt);  kDnH = KeyH(gDown);
	kPgUp = kr(gPgDn,dt) - kr(gPgUp,dt);
	kEnd  = kr(gEnd,dt) - kr(gHome,dt);

	kAdd = Key(gAdd);  kEnt = Key(gEnt);  kEnt2 = Key(gEnt2);
	kCtrl = KeyH(gCtrl); kSh  = KeyH(gSh);  kEsc = Key(gEsc);
	kMul  = Key(gMul);  kSub = Key(gSub);  kDiv = Key(gDiv);
	//  edit seq
	kIns = Key(gIns);   kDel = kr(gDel,dt);  kBckSp = kr(gBckSp,dt);
	kCopy = Key(gC);  kPaste = Key(gV);  kSwap = Key(gX);
	kLoad = Key(gLoad);  kSave = Key(gSave);

	kF1=Key(gF1); kF2=Key(gF2); kF3=Key(gF3); kF6=Key(gF6); kF7=Key(gF7);
	kF8=Key(gF8); kF9=Key(gF9); kF10=Key(gF10); kF11=Key(gF11); kF12=Key(gF12);


	//  quick access keys  * * *
	//----------------------------
	if (par.quickKeys && !(pressGui || pressKey ||
		(edit && mlevel == 1 && ym == M_Sequences) ||
		(mlevel == 2 && ym == M_Testing)))
	{
		if (kEsc)  SetScreen(ST_Main0);
		if (kF1)  SetScreen(ST_Map);
		if (kF2)  SetScreen(ST_Seqs);
		if (kF3)  SetScreen(ST_Test2 + (kSh ? T_Pressed : T_Layout));
		//  F4 save  F5 load
		if (kF6)  SetScreen(ST_Info2 + I_Use);
		if (kF7)  SetScreen(ST_Setup2 + S_Mouse);
		if (kF8)  SetScreen(ST_Setup2 + (kSh ? S_Keyboard : S_Layer));

		if (kF9)   SetScreen(ST_Demos2 +
			(kCtrl ? (kSh ? D_Rain : D_CK_Logo) :
					 (kSh ? D_Hedrons : D_Plasma)));

		if (kF10)  SetScreen(ST_Help);
		if (kF11)
		{	SetScreen(ST_Displ);
			// set page for clock graphs
			if (ym == M_Clock && (pgClock == Cl_StatsExt || pgClock == Cl_Graphs))
				pgDisp = Di_StatsGraph;
		}
		if (kF12)  SetScreen(ST_Clock +
			(kCtrl ? Cl_Graphs : kSh ? Cl_StatsExt : pgClock));
	}

	int sp = (kCtrl ? 10 : kSh ? 1 : 2);  // mul


	//  Keys
	if (mlevel == 2)
		switch (ym)
		{
		case M_Setup:  KeysSetup(sp);  break;
		case M_Matrix: KeysMatrix(sp);  break;
		case M_Info:   KeysInfo(sp);  break;
		}
	else if (mlevel == 1)
		switch (ym)
		{
		#ifdef GAME
		case M_Game:
			if (game.KeyPress(mlevel))
			{	// goto help
				ym = M_Help;  mlevel = 1;  hpage = HAll-2;
			}
			return;
		#endif
		case M_Display:   KeysDisplay(sp);  return;
		case M_Clock:     KeysClock();  return;
		case M_Mapping:   KeysMap();  return;
		case M_Sequences: KeysSeq();  return;
		}


	if (mlevel == 0)  //  main menu
	{
		if (kUp || kPgUp)
			ym = RangeAdd(ym, kUp + kPgUp*3, 0,M_All-1, 1);
		
		if (kRight > 0 || kEnt || kEnt2)
			mlevel = 1;  // enter>
		return;
	}


	//  Add+  <back global
	if ((kAdd || kBckSp) && mlevel > 0)
		--mlevel;


	//  Help
	if (ym == M_Help && mlevel == 1)
	{
		if (kUp || kPgUp)
			hpage = RangeAdd(hpage, kUp+kPgUp, 0,HAll-1, 1);
		if (kRight < 0 || kAdd || kBckSp)
			mlevel = 0;  // <back
		return;
	}


	if (mlevel == 1)  //  sub menu
	{
		//  navigate
		if (kRight < 0)  mlevel = 0;  // <back
		if (kRight > 0 || kEnt2)
		{
			if (ym != M_Display)
				mlevel = 2;  // enter>

			demos.bAuto = ym == M_Demos && yy == D_AutoAll;
		}

		if (kUp){  ym1[ym] += kUp;  Check_ym1();  }
		return;
	}


	//  Demos
	if (ym == M_Demos && mlevel == 2)
	{
		demos.KeyPress((EDemo)ym1[ym]);
		return;
	}
}


void Gui::NextDemo()
{
	auto& yy = ym1[M_Demos];
	++yy;  // next

	if (yy == D_Fonts
	#ifdef DEMOS_BITMAPS
	|| yy ==  D_Bitmaps
	#endif
	)  // skip
		++yy;

	if (yy >= D_All)
		yy = D_Plasma;  // first
}
