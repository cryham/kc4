#pragma once
#include <stdint.h>
#include "def.h"
#include "demos.h"


struct Gui
{
	Demos demos;
	ILI9341_t3n* d = nullptr;

	//  main  ----
	Gui();
	void Init(ILI9341_t3n* tft);
	void Draw();

	int yy = 0;
	void NextDemo();


	//  fade color menu  ---
	enum EFadeClr
	{	C_Main=0, C_Demos, C_Test, C_Map, C_Seq,
		C_Setup, C_Disp, C_Clock, C_Setup2,
		C_Game, C_GameOpt, C_Info, C_ALL  };
	const static uint8_t
		Mclr[C_ALL][2][3];

	void FadeClr(EFadeClr ec, const uint8_t minRGB, const uint8_t mul, const uint8_t div);
	void FadeGrp(uint8_t g, const uint8_t minRGB, const uint8_t mul, const uint8_t div);
	void DrawMenu(int cnt, const char** str, EFadeClr ec, uint16_t curClr,
		uint16_t bckClr, int16_t yadd=10, int16_t nextCol=-1, int16_t numGap=-1);


	//  vars  ---

};
