#include "gui.h"
#include "def.h"


//  Main  ----
Gui::Gui() : d(0)
{
	Init(nullptr);

#ifdef TEMP1
	memset(grTemp, 0, sizeof(grTemp));
#endif
}

void Gui::Init(ILI9341_t3n* tft)
{
	d = tft;
	kbdSend = 0;  //-

	mlevel = 0;
	ym = 0;  yy = 0;
	for (int i=0; i < M_All; ++i)
		ym1[i]=0;


//	mlevel = 2;  //0
//	ym = M_Testing;
//	ym1[M_Testing] = T_Pressed;
//	SetScreen(ST_Main0);

	
	oldti=0;  oldti_kr=0;
	hpage = 0;

	demos.Init(d);
#ifdef GAME
	game.Init(this);
#endif

	ym2Lay=0; ym2Scan=0; ym2Keyb=0; ym2Mouse=0; pressGui=0;
	ym2Disp=0; pgDisp=0;

	nLay=0;  scId=/*NO*/255;  drawId=54;  drawX=0; drawY=0;
	nLayCpy=0;  scIdCpy=0;  
	keyCode=0;  pressKey=0;  pickCode=0;

	edit = 0;  edins = 1;  copyId = -1;
	slot=0; page=0; edpos=0;
	tBlnk=0; tInfo=0;  infType=0;


	//  init Group Starts and Ends
	for (int g=0; g < grpMax; ++g)
	{
		grpStart[g] = 0;  // all
		grpEnd[g] = KEYS_ALL_EXT;

		//  find first and last key, for this group
		for (int i=0; i < KEYS_ALL_EXT; ++i)
			if (cKeyGrp[i] == g)
			{
				if (grpStart[g] == 0)  grpStart[g] = i;
				grpEnd[g] = i;
			}
	}
}

//  start screen  ---
void Gui::SetScreen(int8_t s)
{
	if (s == ST_Main0)
	{	mlevel = 0;  ym = 0;  return;  }
	else  mlevel = 1;

	switch (s)
	{
	case ST_Map:   ym = M_Mapping;  break;
	case ST_Seqs:  ym = M_Sequences;  break;

	case ST_Setup: ym = M_Setup;  break;
	case ST_Matrix2:ym = M_Matrix;  mlevel = 2;  ym1[ym] = s - ST_Matrix2;  break;

	case ST_Displ: ym = M_Display;  break;
	case ST_Help:  ym = M_Help;  break;

	case ST_Demos: ym = M_Demos;  break;
	}
	if (s >= ST_Demos2){  mlevel = 2;  ym = M_Demos;  ym1[ym] = s - ST_Demos2;  }  else
	
	// level2
	if (s >= ST_Setup2 && s < ST_Setup2Max){  mlevel = 2;  ym = M_Setup;    ym1[ym] = s - ST_Setup2;  }  else
	if (s >= ST_Test2 && s < ST_Test2Max){    mlevel = 2;  ym = M_Testing;  ym1[ym] = s - ST_Test2;  }  else
	if (s >= ST_Info2 && s < ST_Info2Max){    mlevel = 2;  ym = M_Info;     ym1[ym] = s - ST_Info2;  }  else
	if (s >= ST_Clock && s < ST_ClockMax){    ym = M_Clock;  pgClock = s - ST_Clock;  }
}

const char* Gui::StrScreen(int8_t s)
{
	switch (s)
	{
	case ST_Main0:  return "Main Menu";
	case ST_Map:    return strMain[M_Mapping];
	case ST_Seqs:   return strMain[M_Sequences];

	case ST_Setup:  return strMain[M_Setup];

	case ST_Displ:  return strMain[M_Display];
	case ST_Help:   return strMain[M_Help];

	case ST_Demos:  return strMain[M_Demos];
	case ST_Matrix2:  return strMain[M_Matrix];
	}
	if (s >= ST_Demos2)  return strDemo[s - ST_Demos2];
	
	// level2
	if (s >= ST_Setup2 && s < ST_Setup2Max)	return strSetup[s - ST_Setup2];  else
	if (s >= ST_Test2 && s < ST_Test2Max)	return strTest[s - ST_Test2];  else
	if (s >= ST_Info2 && s < ST_Info2Max)	return strInfo[s - ST_Info2];  else
	if (s >= ST_Clock && s < ST_ClockMax)	return strClock[s - ST_Clock];
	return "";
}

