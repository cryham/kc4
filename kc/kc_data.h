#pragma once
#include <stdint.h>
#include <vector>
#include "keys_usb.h"
#include "kc_params.h"
#include "def.h"


struct KC_Sequence
{
	//  var length
	std::vector<uint8_t> data;
	const int len() const {  return int(data.size());  }

	void add(uint8_t b) {  data.push_back(b);  }
	void clear() {  data.clear();  }
};

//  seq commands
enum ESeqCmd {
	CMD_SetDelay=0, CMD_Wait, CMD_Comment, CMD_Hide,
	//  mouse move: x,y, buttons: press,release, click,double, wheels: x,y
	CM_x,CM_y, CM_BtnOn,CM_BtnOff, CM_Btn,CM_Btn2x, CM_WhX,CM_WhY,
	CMD_RunSeq, CM_xbig,CM_ybig, CM_xset,CM_yset,CM_mset,
	CMD_Repeat,
	CMD_ALL  // 19
};	// note: need at least so many sequence ids as commands

extern const Clr cCmdClr[CMD_ALL];  // underline colors


struct KC_Setup
{
	//  const
	uint8_t h1,h2, ver;  // header

	//  rows * cols = scanKeys,  matrix setup
	uint8_t rows, cols, scanKeys, seqSlots;  // max seqs

	//  maps scan to byte codes
	//  const size: layers * scanKeys
	uint8_t key[KC_MaxLayers][KC_MaxRows * KC_MaxCols];

	KC_Sequence seqs[KC_MaxSeqs];
	KC_Sequence copy;  // for paste, swap

	const int nkeys() const {  return KC_MaxRows * KC_MaxCols;  }
	const int nseqs() const {  return KC_MaxSeqs;  }

	KC_Setup();
	void Clear(), InitCK();

	//int8_t minLay, layUsed;  // stats
};


//  errors load, save
enum KC_Err {
	E_ok=0, E_size,  E_H1, E_H2, E_ver,
	E_rows, E_cols,  E_slots, E_lay,
	E_rcEq, E_nkeys,  E_max
};
extern const char* KCerrStr[E_max];


//  main, state
//--------------------------------------
struct KC_Main
{
	//  toggles
	int8_t kbdSend = 0;  // 1 send keys to usb  0 in menu
	bool forceClock = false;  // force big clock show

	int8_t setBright = 1;  // update brightness
	bool offBright = false;  // off display for night


	//  current layer, set by keys
	int8_t nLayer = 0, nLayerHeld = -1,  // final, held layer key (or set)
		   nLayerLock = -1;  // tap layer key to lock/unlock
	bool bLxOffSkip = false;

	//  sequence running vars  0 norm, 1 from seq
	int8_t inSeq[2] = {-1,-1},	// id run, -1 none
		seqRel[2] = {0,0},		// pressed / released
		seqWait = 0;			// one time, restore dtSeq
	int16_t seqPos[2] = {0,0};	// cur index in seq data
	uint16_t dtSeq = 20;		// var delay  param
	uint32_t tiSeq = 0, tiFun = 0;  // ms time delay
	int8_t visSeq = 0;          // vis start only

	int8_t seqMod[K_ModLast+1];  // modifiers state
	void SeqModClear();
	void QuitSeq(int8_t chk=1);
	uint16_t xm=0, ym=0;  // abs mouse pos

	
	//  main  ----
	KC_Setup set;

	KC_Main();

	void UpdLay(uint32_t ms), UpdL();  // update
	void Send(uint32_t ms);  // send usb

	void StartSeq(int sq, uint32_t ms);
	bool SeqEnd(int lev, const KC_Sequence& sq);


	//  eeprom  ----
	uint16_t eSlot = 0;
	KC_Err err = E_ok;
	void Load(), Save();
	int FillConfig(uint8_t* buffer);  // for save or vis

	uint16_t memSize = 0;  // result B
	uint16_t GetSize();  // mem

	int8_t loadExt = 1, saveExt = 1;  // 0 forces internal eeprom use
	uint8_t ERead(int& a);


	//  inactive time(s) for stats
	uint32_t
		tm_on = 0,  // uptime   tm_= rtc, ms_= milis
		tm_key = 0, tm_keyOld = 0,  // last key press time, previous
		tm_keyAct = 0,
		msKeyLay = 0,  // layer key press, for lock times
		msMin1 = 0,    // 1 minute, last time
		msDaily1 = 0;  // daily press/min, last time
	uint16_t  // counts
		tInact1 = 0, tInact2 = 0,  // previous 2 inact mins in minutes
		tInactSum = 0,  // sum inactive times
		min1_Keys = 0;  // 1 minute keys press, show
	void ResetStats(bool rtc=false);

	//--inact-----|-active--|----------|---|----------------------
	//   0   1   2* *3 * 4  *5   6   7 * 8 * 9  10 now    * key press

	uint8_t grPMin[2][W];  // press/1min graph array  [1] is daily
	uint16_t grPpos[2] = {0,0};  // write pos

	uint8_t matrixVis[KC_MaxRows * KC_MaxCols];


	//  led lamp  ----
	#ifdef LED_LAMP
	bool ledOn = false;
	void LedUpdate();
	#endif
};

extern KC_Main kc;
