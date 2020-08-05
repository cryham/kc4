#pragma once
#include <stdint.h>
typedef unsigned long ulong;
typedef uint8_t Clr[3];


//  display  dim
const static int W = 320, H = 240;

//  R F800  G 07E0  B 001F  R 32 G 64 B 32  565
#define RGB(r,g,b)  ( ((r)<<11)+ ((g)<<6) +(b))  // 31 31 31
#define RGB2(r,g,b) ( ((r)<<11)+ ((g)<<5) +(b))  // 31 63 31

#ifdef __cplusplus
class ILI9341_t3n;
#endif


enum EMainMenu  //  main menu entries, level0
{
	M_Mapping, M_Sequences, M_Testing,
	M_Setup, M_Matrix, M_Info,
	M_Display, M_Clock, M_Help,
	#ifdef GAME
		M_Game,
	#endif
	M_Demos,
	M_All,  M_Next = M_Display  // 2nd column, -1 off
	// any change here needs updating 3 below arrays [M_All]
};

enum EDemo  //  Demos, level1
{
	D_AutoAll,
	D_Plasma,
	D_Wave, D_Fire,
	D_Hedrons,
	D_CK_Logo,
	D_Fonts,
#ifdef DEMOS_OLD
	D_Ngons,
	D_Space, D_Balls, D_Fountain,
#endif
	D_Rain,
	D_All,  D_Next = D_CK_Logo
};
extern const char *strDemo[D_All];


//  menus  --
enum ETesting  //  Testing kbd, level1
{
	T_Layout, T_Pressed, T_All
};
enum ESetup  //  Setup kbd, level1
{
	S_Layer, S_Keyboard, S_Mouse, S_All
};
enum EMatrix  //  Matrix #, level1
{
	X_Test, X_Scan, X_All
};

enum EInfo  //  Info use,ver
{
	I_Use, I_Version, I_All
};
enum EDisplay  //  Display options pages  --
{
	Di_Bright, Di_Key, Di_Stats, Di_Graph, Di_Debug, Di_All
};
enum EClock
{
	Cl_Adjust, Cl_Simple,
	Cl_StatsText, Cl_Stats, Cl_StatsExt,
	Cl_Graphs, Cl_All
};


//  string names for all above ^
extern const char
	*strMain[M_All], *strTest[T_All],
	*strSetup[S_All], *strMatrix[X_All],
	*strInfo[I_All], *strClock[Cl_All];

//  bitmaps
// 20 x 20 x 4B  (B,G,R,A 32bit  could be 16bit but meh)
typedef uint8_t Bmp20[1600];
extern const Bmp20* bmpMain[M_All];

//  sub page counts, inside each main menu entry
extern const uint8_t YM1[M_All];


//  time intervals  *0.1s
extern const uint16_t gIntervals[];
const static uint8_t gIntvMask = 0x1F;


enum EStartScr  //  start screen
{	ST_Main0 = 0,
	ST_Test2, ST_Test2Max = ST_Test2 +T_All,
	ST_Map = ST_Test2Max,
	ST_Seqs,
	ST_Displ,
	ST_Clock, ST_ClockMax = ST_Clock +Cl_All,
	ST_Help = ST_ClockMax,
	ST_Setup,
	ST_Matrix2,
	ST_Info2, ST_Info2Max = ST_Info2 +I_All,
	ST_Setup2 = ST_Info2Max, ST_Setup2Max = ST_Setup2 +S_All,
	ST_Demos = ST_Setup2Max,
	ST_Demos2,  ST_ALL = ST_Demos2 +D_All
};

//  fade color menu  ---
enum EFadeClr
{	C_Main=0, C_Demos, C_Test, C_Map, C_Seq,
	C_Setup, C_Disp, C_Clock, C_Setup2,
	C_Game, C_GameOpt, C_Info, C_Matrix, C_ALL  };
extern const uint8_t
	gMenuClr[C_ALL][2][3];
