#pragma once
#include <stdint.h>
typedef unsigned long ulong;
typedef uint8_t Clr[3];

extern const char* strVersion;


//  display  dim
const static int W = 320, H = 240;

//  R F800  G 07E0  B 001F  R 32 G 64 B 32  565
#define RGB(r,g,b)  ( ((r)<<11)+ ((g)<<6) +(b))  // 31 31 31
#define RGB2(r,g,b) ( ((r)<<11)+ ((g)<<5) +(b))  // 31 63 31

#ifdef __cplusplus
class ILI9341_t3n;
#endif


//  main menu entries, level0
//-----------------------------------------------
enum EMainMenu
{
	M_Mapping, M_Sequences, M_Testing,
	M_Setup, M_Matrix,
#ifdef GSM
	M_GSM,
#endif
	M_Config,
	M_Display, M_Clock,
	M_Help,  //new?, Graphs, Calendar, Alarms
#ifdef GAME
	M_Game,
#endif
	M_Demos,
	
	M_All,	// NOTE: any change here needs updating
			// all 3 arrays [M_All] below,  search for: /* MAIN
	M_Next = M_Config  // 2nd column, -1 off
};


//  sub menus  level1
//-----------------------------------------------
enum ETesting  //  Testing kbd
{
	Ts_Layout, Ts_Pressed, Ts_All
};
enum ESetup  //  Setup kbd
{
	St_Layer, St_Keyboard, St_Mouse, St_Remote, St_All
};
enum EMatrix  //  Matrix #
{
	Mx_Test, Mx_Scan, Mx_All
};

enum EConfig  //  Config use,ver
{
	Cf_Storage, Cf_Use, Cf_Version, Cf_All
};
enum EDisplay  //  Display options pages  --
{
	Di_Bright, Di_Keys, Di_StatsGraph, Di_AdjustTemp, Di_All
};
enum EClock
{
	Cl_Adjust, Cl_Simple,//big
	Cl_Stats, Cl_StatsExt,
	Cl_Graphs, Cl_GraphsDaily, Cl_GraphLight, Cl_All
};

//-----------------------------------------------
//  string names for all above ^
extern const char
	*strMain[M_All], *strTest[Ts_All],
	*strSetup[St_All], *strMatrix[Mx_All],
	*strConfig[Cf_All], *strClock[Cl_All];


//-----------------------------------------------
enum EDemo  //  Demos, level1
{
	D_AutoAll,
	D_Fractal, D_Plasma,
	D_Wave, D_Fire,

	D_Hedrons,
	D_CK_Logo,
	D_Fonts,
#ifdef DEMOS_BITMAPS
	D_Bitmaps,
#endif
#ifdef DEMOS_OLD
	D_Ngons,
	D_Space, D_Balls, D_Fountain,
#endif
	D_Rain,
	D_All,  D_Next = D_Hedrons,
	D_First = D_Fractal,
};
extern const char *strDemo[D_All];


//-----------------------------------------------
//  menu icons, bitmaps
// 20 x 20 x 4B  (B,G,R,A 32bit  could be 16bit but meh)
typedef uint8_t Bmp20[1600];
extern const Bmp20* bmpMain[M_All];
typedef uint8_t Bmp320[320*240*4];

//  sub page counts, inside each main menu entry
extern const uint8_t YM1[M_All];


//  time intervals  *0.1s
extern const uint16_t gIntervals[];
const static uint8_t gIntvMask = 0x1F;


enum EStartScr  //  start screen
{	ST_Main0 = 0,
	ST_Test2, ST_Test2Max = ST_Test2 +Ts_All,
	ST_Map = ST_Test2Max,
	ST_Seqs,
	ST_Displ,
	ST_Clock, ST_ClockMax = ST_Clock +Cl_All,
	ST_Help = ST_ClockMax,
	ST_Setup,
	ST_Matrix2,
	ST_GSM,
	ST_Config2, ST_Config2Max = ST_Config2 + Cf_All,
	ST_Setup2 = ST_Config2Max, ST_Setup2Max = ST_Setup2 + St_All,
	ST_Demos = ST_Setup2Max,
	ST_Demos2,  ST_ALL = ST_Demos2 + D_All
};

//  fade color menu  ---
enum EFadeClr
{	c_Main=0, c_Demos, c_Test, c_Map, c_Seq,
	c_Setup, c_Disp, c_Clock, c_Setup2,
	c_Game, c_GameOpt, c_Config, c_Matrix, c_ALL  };
extern const uint8_t
	gMenuClr[c_ALL][2][3];
