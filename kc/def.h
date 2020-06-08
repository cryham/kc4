#pragma once
#include <stdint.h>
typedef unsigned long ulong;


//----  Setup  ----	  use:  ram B      flash
//  optional features, comment out to disable
#define DEMOS			//  28        9%
//#define DEMOS_OLD_PAR	//  280 <1%   16%  min
					// all  6680 10%  17%  max 41k


#define W 320  //  display dim
#define H 240

//  R F800  G 07E0  B 001F  R 32 G 64 B 32  565
#define RGB(r,g,b)  ( ((r)<<11)+ ((g)<<6) +(b))   // 31 31 31
#define RGB2(r,g,b) ( ((r)<<11)+ ((g)<<5) +(b))  // 31 63 31

#ifdef __cplusplus
class ILI9341_t3n;
#endif


#ifdef DEMOS
enum EDemo  //  Demos, level1
{
	D_Plasma,
	D_Wave, D_Fire,
	D_Hedrons,
	D_CK_Logo,
#ifdef DEMOS_OLD_PAR
	D_Fonts,
	D_Ngons,
	D_Space, D_Balls, D_Fountain,
#endif
	D_Rain,
	D_All,  D_Next = D_CK_Logo
};
//extern const char *strDemo[D_All];
#endif
