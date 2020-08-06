#include "def.h"
#include "gui.h"
#include "gui_bitmaps.h"


//  time intervals *0.1s
const uint16_t gIntervals[gIntvMask+1] =
{
	0, 1 /*100ms 1*/, 2, 4, 6, 8,
	10   /*1s 6*/   ,20,40,60,80,
	100  /*10s 11*/ ,200,300,450,
	600  /*1m 15*/  ,720,900,
	1200 /*2m 18*/  ,2100,3000,4500,
	6000 /*10m 22*/ ,7200,9000,
	12000/*20m 25*/ ,15000,18000,24000,
	36000/*1h 29*/  ,45000,
	54000/*1.5h 31*/
};

//  menu colors
const uint8_t gMenuClr[C_ALL][2][3] =
{
	{{20,26,31},{1,3,2}},  // 0 main 1,3,2
	{{26,25,31},{2,3,1}},  // 1 demos
	{{20,30,26},{6,6,8}},  // 2 test
	{{24,24,28},{3,3,3}},  //  3 map
	{{17,31,31},{5,4,3}},  //  4 seqs
	{{24,27,27},{6,5,4}},  // 5 setup
	{{31,31,16},{3,5,5}},  //  6 display
	{{30,30,30},{5,3,1}},  //  7 clock
	{{26,28,28},{4,3,3}},  // 8 setup2
	{{31,24,10},{2,5,5}},  //  9 game
	{{31,26,22},{1,6,8}},  //  10 game opt
	{{26,26,28},{9,9,8}},  // 11 info
	{{24,24,31},{8,6,6}},  // 12 matrix
};

//  Main Menu
const char* strMain[M_All] =
{
	"Mapping", "Sequences",	"Testing",
	"Setup", "Matrix", "Info",
	"Display", "Clock", "Help",
#ifdef GAME
	"Game",
#endif
	"Demos",
};

const Bmp20* bmpMain[M_All] =
{
	&bmpKEY, &bmpDOTS3, &bmpZOOM,
	&bmpSETUP, &bmpMATRIX, &bmpINFO,
	&bmpSCREEN, &bmpCLOCK, &bmpHELP,
#ifdef GAME
	&bmpGAME,
#endif
	&bmpDEMO,
};

//  Testing kbd
const char* strTest[T_All] =
{
	"Layout", "Pressed",
};
//  Setup kbd
const char* strSetup[S_All] =
{
	"Layer", "Keyboard", "Mouse",
};
//  Matrix
const char* strMatrix[X_All] =
{
	"Test matrix", "Scan setup",
};

//  Info use, ver
const char* strInfo[I_All] =
{
	"Use", "Version",
};
//  Clock pages
const char* strClock[Cl_All] =
{
	"Adjust", "Clock", "Stats", "Stats Extd", "Stats+Graph", "Graphs~"
};


//  Demos
const char* strDemo[D_All] =
{
	"All",
	"Plasma",
	"Wave", "Fire",
	"Hedrons",
	"CK Logo",
	"Fonts",
#ifdef DEMOS_OLD
	"N-gons",
	"Space", "Balls", "Fountain",
#endif
	"Rain",
};


//  sub pages  menu level1
const uint8_t YM1[M_All] =
{
	4,  // M_Mapping
	1,  // M_Sequences
	T_All,  // M_Testing
	
	S_All,  // M_Setup
	X_All,  // M_Matrix
	I_All,  // M_Info
	
	1,  // M_Display
	1,  // M_Clock  Cl_All
	1,  // M_Help
#ifdef GAME
	1,  // M_Game
#endif
	D_All,  // M_Demos
};
