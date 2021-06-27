#pragma once
#include <WProgram.h>
#include "def.h"


//  sinus table  ----
// for: plasma, wave, ck_logo
#define SX  8192   // 8192  mem size 16kB
#define SY  16384  // 16384  y quality
#define Cos(x)  sint[(SX/2+(x))%SX]  // %SX
#define Sin(x)  sint[      (x) %SX]

extern const int16_t sint[SX];

struct Gui;

struct Demos
{
	//  main  ----
	Demos();
	void Init(ILI9341_t3n* tft, Gui* gui);

	ILI9341_t3n* d = nullptr;
	uint16_t* data = nullptr;  // scren buffer
	Gui* g = nullptr;

	int8_t iFps = 0;  // show frames per second, 1 on, 0 off
	uint32_t ti = 0, oti = 0;  // fps: time ms, old

	
	//  Fonts
	void Fonts();  // chars
	void Version();  // ver, date
#ifdef DEMOS_BITMAPS
	void Bitmaps();
	const static int8_t bmMax = 2;
	int8_t bmCur = 0;
#endif


	int8_t bAuto = 0;  // auto next demo
	int8_t iInfo = 0;  // show demo params

	bool Draw(int num);  // returns next for auto all
	void KeyPress(EDemo demo);

	uint t = 0, cnt = 0, next = 0;  // frame counter


	//  Plasma  ~~~~
	enum EPlasma  {  // sets
		Pls_SlowClrRings, Pls_ClrLines, Pls_OrnPinkCy, Pls_OrnViolBlue,
		Pls_SmallWhiteCyVi, Pls_PAbsCyanRed, Pls_PAbsCyanOlive, Pls_PAbsViolDarkCyanEl,
		Pls_FastOrnCy, Pls_FastBlueYellow, Pls_FastDiamCyBlR, Pls_PinkSlime, Pls_WhitePaint,
		Plasma_All  };
	int8_t plasma = 0, tadd[Plasma_All];  // cur mode, speeds

	void PlasmaDraw(), PlasmaInit();
	void PSlowClrRings(), PClrLines(), POrnPinkCy(), POrnViolBlue(),
		PSmallWhiteCyVi(), PAbsCyanRed(), PAbsCyanOlive(), PAbsViolDarkCyanEl(),
		PFastOrnCy(), PFastBlueYellow(), PFastDiamCyBlR(), PPinkSlime(), PWhitePaint();

	//  Wave  ~
	int8_t waveSpd = 8;
	void Wave();

	//  Fire  ~~
	const static int8_t num_fire = 2;
	int8_t fire = 0, fireSpd[num_fire];
	void Fire(), Fire1(),Fire2();


	//  CK logo, 2D lines
	const static int8_t ckMax = 4;
	int8_t ckCur = 0, ckSpeed = 8;
	void CK_logo();


#ifdef DEMOS_OLD
	//  Balls  --------
	const static int  // _param_  none 49472
	//bMax = 100,  // 1356 B  ram size
	bMax = 600,  //
	dMax = bMax, sMax = bMax *12/18;

	int sCnt, sVel;  // stars: count, velocity
	int bCnt, bSpd, bSpRnd, bRad;  // balls: count, speed, radius max
	const static int bSpRMax = 8;
	#define bDet  64  // move detail

	enum EInit {  INone=0, IBalls, ISpace, IDrops  } einit;
	struct Star {  int x,y,z, v;  uint16_t c;  };  // 18 B
	struct Ball {  int16_t x,y, vx,vy, r;  uint16_t c;  };  // 12 B
	struct Drop {  int16_t x,y, vx,vy, t;  uint16_t c;  };
	union
	{	//  common data
		Star star[sMax];
		Ball ball[bMax];
		Drop drop[dMax];
	};
	void BallsInit();
	void Balls();


	//  Space
	void SpaceInit();
	void StarNew(int i);  // new
	void Space();

	//  Fountain, drops  ----
	int fInt, fWave;
	void FountainInit();
	void Fountain();

	//  Ngons 2D
	int16_t ngt;  int8_t ngCur,  ngtOn, ngRot;
	const static int8_t ngMin = 7, ngMax = 33, ngRotMax = 4;  // all n
	void Ngons();
#endif

	//  Rain  ----
	int8_t rCur, r1Int,r1Size, r2Int,r2Size;
	void Rain();
	

	//  Polyhedrons 3D  ----
	const static int8_t hdA = 11, hdRotMax = 4, hdDiagMax = 7;  // all presets
	const static int16_t hdtMax = 200;  // cycle time

	int16_t hdt;  int8_t hdCur,  hdtOn, hdRot,  hdSpd, hdDiag;
	void Hedrons();


	//  Fractal  ::::
	int8_t yFrPar = -1;  // cur
	float  tFrX=0.f, tFrY=0.f, tFrZ=0.f, frSpd = 0.80f;
	struct Fractal
	{
		int iter;
		float x0, y0, z0;  // start
		float xD, yD, zD;  // diff each iter
		float zAmp, zSpd;  // time ampl and speed
		float yAmp, ySpd;
		float xAmp, xSpd;
		float xM, yM, zM;  // mult
	};
	const static int8_t FR_ALL = 8;  //  presets
	static Fractal FrSets[FR_ALL];
	void FractalDraw();
	void KeyFractal(int k, const float fa);

	int8_t fractal = 3;  // cur
	bool bFrPause = false;
};
