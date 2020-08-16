#include "WProgram.h"
#include "kc_data.h"
#include "kbd_layout.h"
//extern KC_Params par;

//  load, save in eeprom
//.............................................
#define ESize 1080  //  max eeprom size to use
#define Erd(a)    eeprom_read_byte((uint8_t*)a);      ++a;  memSize = a;  if (a >= ESize) {  err=E_size;  return;  }
#define Ewr(a,b)  eeprom_write_byte((uint8_t*)a, b);  ++a;  memSize = a;  if (a >= ESize) {  err=E_size;  return;  }


//  default params  ----
void ParInit()
{
	par.debounce = 2;  // ms?
	par.strobe_delay = 8;
	par.scanFreq = 50;  // * 20 Hz

	par.brightness = 80;  par.brightOff = 60;
	par.startScreen = 0;

	par.verCounter = 0;
	par.krDelay = 250/5;  par.krRepeat = 80/5;  // ms
	par.mkSpeed = 100;  par.mkAccel = 100;
	par.mkWhSpeed = 100;  par.mkWhAccel = 100;
	par.quickKeys = 1;

	par.dtSeqDef = 20;
	par.defLayer = 0;  par.editLayer = 2;
	par.keyGui = gGui;

	par.msLLTapMax = 10;  // 100ms
	par.msLLHoldMin = 30;  // 3s

	par.rtcCompensate = 0;
	par.minInactive = 5;
	par.tempOfs = int8_t(-0.6/*'C*/ / 0.03);  //-20

	par.time1min = 4;  // 1m
	par.timeTemp = 10;   // 8s
	par.timeTgraph = 15; // 1m  gIntervals
	par.minTemp = 17;  par.maxTemp = 35;
	par.xCur = W-1;
}

//  errors
const char* KCerrStr[E_max] = {
	"ok", "> Max size",  "Hdr1 !k", "Hdr2 !c", "Hdr3 ver <",
	"Scan rows >8", "Scan cols >18",  "Seq slots >60", "Key lay >8",
	"r * c != sc", "nkeys != sc",
};

//  seq commands colors ___ underline
const Clr cCmdClr[CMD_ALL]={
	// CMD_SetDelay, CMD_Wait,  CMD_Comment, CMD_Hide,
	{31,31,12},{31,31,20},  {6,28,30}, {12,22,31},
	// CM_x,CM_y, CM_BtnOn,CM_BtnOff,
	{6,31,6},{16,31,6}, {18,26,18},{14,26,22},
	// CM_Btn,CM_Btn2x, CM_WhX,CM_WhY,
	{6,31,6},{16,31,6}, {6,31,16},{16,31,16},
	{31,16,16}, // CMD_RunSeq
	//  CM_xbig,CM_ybig, CM_xset,CM_yset,  CM_mset,
	{6,31,6},{16,31,6}, {16,24,16},{19,24,16},  {21,26,21},
	{31,21,6} }; // CMD_Repeat



//  get ram allocated
uint16_t KC_Main::GetSize()
{
	uint16_t s=0;
	s += sizeof(KC_Main);
	s += sizeof(KC_Setup);
	int i;
	s += set.nkeys();  // 1B per key

	for (i=0; i < set.nseqs(); ++i)
	{
		s += sizeof(KC_Sequence);
		s += set.seqs[i].data.capacity();
	}
	return s;
}


//  Load
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
void KC_Main::Load()
{
	err = E_ok;
	set.Clear();

	//  var adr,  // todo: ofs = slot * ESize 5000, ext spi eeprom ..
	int a = 0, i, n;  uint8_t b;
	//  header
	set.h1 = Erd(a);  if (set.h1 != 'k') {  err=E_H1;  return;  }
	set.h2 = Erd(a);  if (set.h2 != 'c') {  err=E_H2;  return;  }
	set.ver = Erd(a);  if (set.ver > 9) {  err=E_ver;  return;  }

	//  matrix
	set.rows = Erd(a);  if (set.rows > KC_MaxRows) {  err=E_rows;  return;  }
	set.cols = Erd(a);  if (set.cols > KC_MaxCols) {  err=E_cols;  return;  }
	set.scanKeys = set.rows * set.cols;
	set.seqSlots = Erd(a);  // now less than in ee
	if (set.seqSlots > KC_MaxSeqs) {  err=E_slots;  set.seqSlots = KC_MaxSeqs;  }


	//  params  ----
	ParInit();  // defaults

	n = Erd(a);  // size
	eeprom_read_block((void*)&par, (void*)a, n);  a+=n;
	if (a >= ESize) {  err=E_size;  return;  }

	if (par.startScreen >= ST_ALL)
		par.startScreen = ST_ALL-1;
	setBright = 1;  // upd
	
	//  old versions  --

	//  zero
	memset(set.key, 0, sizeof(set.key));

	//  Keys  ---
	for (i=0; i < set.scanKeys; ++i)
	{
		uint8_t len = Erd(a);
		if (len > KC_MaxLayers)
		{	err=E_lay;  return;  }

		for (n=0; n < len; ++n)
		{
			b = Erd(a);
			set.key[n][i] = b;
		}
	}

	//  Seqs  ---
	for (i=0; i < set.seqSlots; ++i)
	{
		uint8_t len = Erd(a);

		//KC_Sequence s;
		set.seqs[i].clear();
		for (n=0; n < len; ++n)
		{
			b = Erd(a);
			set.seqs[i].add(b);
		}
	}

	if (set.seqSlots < KC_MaxSeqs)
		set.seqSlots = KC_MaxSeqs;  // now more than in ee

	memSize = a;
}

//  Save
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
void KC_Main::Save()
{
	err = E_ok;
	//  sth very wrong
	if (set.rows * set.cols != set.scanKeys)
	{	err=E_rcEq;  return;  }

	#if !(defined(CK1) || defined(CK9))
	if (set.nkeys() != int(set.scanKeys))
	{	err=E_nkeys;  return;  }
	#endif

	int a = 0 /*slot * ESize*/, i, n;

	//  header
	set.h1 = 'k';  set.h2 = 'c';  set.ver = 5;  // cur
	Ewr(a, set.h1);  Ewr(a, set.h2);  Ewr(a, set.ver);

	//  matrix
	Ewr(a, set.rows);  Ewr(a, set.cols);  Ewr(a, set.seqSlots);


	//  params  ----
	++par.verCounter;  // inc ver

	n = sizeof(par);
	Ewr(a, n);  // size
	eeprom_write_block((void*)&par, (void*)a, n);  a+=n;
	if (a >= ESize) {  err=E_size;  return;  }


	//  Keys  ---
	for (i=0; i < set.scanKeys; ++i)
	{
		//  get len, find last non zero
		uint8_t len = KC_MaxLayers;
		for (int l = KC_MaxLayers-1; l >= 0; --l)
			if (set.key[l][i] == KEY_NONE)  len = l+1;
			else  break;
		Ewr(a, len);

		for (n=0; n < len; ++n)
		{	Ewr(a, set.key[n][i]);  }
	}

	//  Seqs  ---
	for (i=0; i < set.seqSlots; ++i)
	{
		const KC_Sequence& s = set.seqs[i];
		uint8_t len = s.len();
		Ewr(a, len);

		for (n=0; n < len; ++n)
		{	Ewr(a, s.data[n]);  }
	}

	memSize = a;
}
