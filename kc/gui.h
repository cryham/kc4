#pragma once
#include <stdint.h>
#include "def.h"
#include "demos.h"
#include "keys_usb.h"
#ifdef GAME
#include "games.h"
#endif
#include "gui_bitmaps.h"


struct Gui
{
	Demos demos;
	ILI9341_t3n* d = nullptr;
#ifdef GAME
	Games game;
#endif

	//  main  ----
	Gui();
	void Init(ILI9341_t3n* tft);
	void Clear(void* buf), Draw(), DrawEnd();  bool Force1Buf();
	void KeyPress(), NextDemo();


	//  draw menus
	void DrawMapping(), DrawSequences(), DrawPickKey();  // edit
	void DrawTesting(), DrawSetup(), DrawMatrix();  // setup
	void DrawInfo(), DrawDisplay(), DrawClock(), DrawGraph(), DrawHelp();  // info

	//  draw util
	const static int PickR = 10;  // key pick rows
	void DrawPressed(bool far=false), DrawLayout(bool edit);
	void DrawSeq(int8_t seq, int8_t q, uint16_t bck);
	void StrSeqCmd(int cmd, uint8_t cpar, char* a, bool& inCmt, bool shrt);
	void DrawOperInfo();
	void DrawDispCur(int i, int16_t y), DrawClockCur(int i, int16_t y);

	void PrintR(const char* str, int16_t x, int16_t y);  // right align
	int16_t GetWidth(const char* s);
	void DrawBmp(const uint8_t* bmp, int16_t x, int16_t y, int16_t w, int16_t h, uint al=256);
	void DrawBmp(const Bmp20* bmp, int16_t x, int16_t y, uint al=256);

	//  draw menu
	void DrawTitleMain(uint16_t clr, const Bmp20* bmp);
	void DrawTitle(const char* str, uint16_t clr, const Bmp20* bmp);
	void DrawMenu(int cnt, const char** str, const Bmp20** bmp,
		EFadeClr ec, uint16_t curClr, uint16_t bckClr,
		int8_t nextCol=-1, int8_t numGap=-1, int8_t numGap2=-1);
	
	//  util
	void ClrPress(int pressPerMin), ClrTemp(int temp);
	void PrintInterval(uint32_t t);
	int TempFtoB(float t);  float TempBtoF(uint8_t b);


	//  keys
	int8_t KeysSeq();  void KeysMap();
	int PressKey(int8_t& var);
	void KeysSetup(int sp), KeysMatrix(int sp), KeysDisplay(int sp);
	void KeysInfo(int sp), KeysClock();

	//  start
	void SetScreen(int8_t start);
	const char* StrScreen(int8_t s);

	void FadeClr(EFadeClr ec, const uint8_t minRGB, const uint8_t mul, const uint8_t div, uint16_t bckClr);
	void FadeGrp(uint8_t g, const uint8_t minRGB, const uint8_t mul, const uint8_t div, uint16_t bckClr);
	uint16_t FadeClr(uint8_t r, uint8_t g, uint8_t b, uint8_t mul, uint8_t div, uint8_t mi, uint16_t bckClr);
	

	//  vars  ---------
	int8_t kbdSend = 0; // 1 send to usb  0 in menu
	int8_t mlevel = 0;  // 0 main, 1 level1, 2 level2

	int8_t ym = 0;      // 0 main y cursor
	int8_t ym1[M_All];  // 1 y cursor for all main menu entries
	int8_t yy = 0;      // = ym1[ym]  level1 y cursor
	void Check_ym1();

	//  time, key repeat
	uint32_t oldti=0, oldti_kr=0;
	int8_t kr(uint8_t sc, uint16_t dt);
	//  fps
	int16_t fps=0, fps_cnt=0;

	//  help
	int8_t hpage = 0;
	const static int8_t HAll = 11;
	#ifdef LED
	int8_t led = 0;
	#endif


	//  keys pressed, some +-1  _k_
	int8_t kRight=0, kUp=0,  kPgUp=0, kEnd=0,  kDnH=0,
	/*Add*/ kEsc=0, kAdd=0, kEnt=0,kEnt2=0,  kCtrl=0, kSh=0,  kMul=0, kSub=0, kDiv=0,
	/*seq*/	kBckSp=0, kIns=0, kDel=0,  kCopy=0, kPaste=0, kSwap=0, /*F4,5*/kLoad=0, kSave=0,
			kF1=0,kF2=0,kF3=0,kF6=0,kF7=0,kF8=0,kF9=0,kF10=0,kF11=0,kF12=0;

	//  Mapping  - - - -
	int16_t keyCode=0, scId=0, scIdCpy=0, drawId=-1, drawX=0,drawY=0;
	int8_t nLay=0, nLayCpy=0,
		pressKey=0, pickCode=K_Seq0, // edit operations
		keyGroup=grpMax-1, grpFilt=1;  // pick group filter

	//  level 2 y cursors  - - -
	int8_t ym2Lay = 0, ym2Keyb = 0, ym2Mouse = 0,  // Setup
		ym2Scan = 0, pressGui = 0;  // Matrix
	int8_t ym2Disp = 0, pgDisp = 0;  // Display
	int8_t ym2Clock = 0, pgClock = Cl_StatsExt;  // Clock


	//  const
	const uint8_t yTitle = 60, yTitleUp = 52;  // y after title
	const uint8_t yPosLay = 112;
	const static uint8_t
		DispPages[Di_All], ScanPages[S_All]; //, InfoPages[I_All];

	inline static uint8_t ClockVars(int pg)
	{	return pg == Cl_Adjust ? 6 : 0;  }

	//  util
	int16_t RangeAdd(int16_t val, int16_t add, int16_t vmin, int16_t vmax, int8_t cycle=0);
	void Save(), Load(int8_t reset);


	//  Sequences  - - - -
	int8_t edit = 0;   // seq 0 view / 1 edit
	int8_t edins = 1;  // 1 ins 0 overwrite
	int8_t copyId = -1;  // copy/swap from

	int8_t slot=0, page=0;  int16_t edpos=0;  // edit vars
	void slotMax(),slotMin(),pageInc(),pageDec();
	int seqId()
	{	return slot + page*iPage;  }

	int8_t tBlnk = 0;
	int16_t tInfo=0;  int8_t infType=0;  // info text vars
	const int8_t iPage = 10, cBlnk = 60;  // par-

	//  const from grp
	uint8_t grpStart[grpMax], grpEnd[grpMax];


	//  Temp 'C  ---
#ifdef TEMP1
	float fTemp = -90.f;  // cur value
	int8_t temp1 = 1;     // fist, init
	//  last time read'C, add to graph
	uint32_t msTemp = 0, msTempGr = 0;
	void GetTemp();

	uint8_t grTemp[W];    // graph array
	uint16_t grTpos = 0;  // write pos
	// auto range
	uint8_t grTempUpd = 1;  // update
	uint8_t grFmin = 17, grFmax = 35;  // temp 'C
	uint8_t grBmin = 0, grBmax = 255;  // val Byte
#endif

};

