#include <WProgram.h>
#include "kc_data.h"
#include "kbd_layout.h"
#include "eeprom_spi.h"


//  load, save in eeprom
//.............................................

uint8_t KC_Main::ERead(int& a)
{
	uint8_t b =
#ifdef EEPROM_CS
		loadExt ? EE_SPI_Read(a) :
#endif  // no ext, or force load int
		eeprom_read_byte((uint8_t*)a);
	
	++a;  memSize = a;
	return b;
}


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

	par.time1min = 15;  // 1m
	par.timeTemp = 10;   // 8s
	par.timeTgraph = 15; // 1m  gIntervals
	par.minTemp = 17;  par.maxTemp = 35;
	par.xCur = W-1;
	
	par.ledBright = 64;  // max for 4095
	par.dailyHours = 12;
}

//  errors
const char* KCerrStr[E_max] = {
	"ok", "> Max size",  "Hdr1 !k", "Hdr2 !c", "Hdr3 ver <",
	"Scan rows >", "Scan cols >",  "Seq slots >", "Key lay >",
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

	//  var adr
	int a,a0, i, n;  uint8_t b;
	bool ext = 
	#ifdef EEPROM_CS
		loadExt > 0 ? true :
	#else
		false;
	#endif
	a = ext ? int(eSlot) * int(ESlotSize) : 0;  a0 = a;  // todo: not working..
	#define Ser(a)  //if (a >= a0 + ESlotSize) {  err=E_size;  return;  }  // todo: check out of slot
	if (ext)
		EE_SPI_Start();
	//  reading 1 byte each time, since serial eeprom can't otherwise

	//  header
	set.h1 = ERead(a);  if (set.h1 != 'k') {  err=E_H1;  return;  }  Ser(a)
	set.h2 = ERead(a);  if (set.h2 != 'c') {  err=E_H2;  return;  }  Ser(a)
	set.ver = ERead(a);  if (set.ver > 9) {  err=E_ver;  return;  }  Ser(a)

	//  matrix
	set.rows = ERead(a);  if (set.rows > KC_MaxRows) {  err=E_rows;  return;  }  Ser(a)
	set.cols = ERead(a);  if (set.cols > KC_MaxCols) {  err=E_cols;  return;  }  Ser(a)
	set.scanKeys = set.rows * set.cols;
	set.seqSlots = ERead(a);  // now less than in ee
	if (set.seqSlots > KC_MaxSeqs) {  err=E_slots;  set.seqSlots = KC_MaxSeqs;  }
	Ser(a)

	//  params  ----
	ParInit();  // defaults

	n = ERead(a);  Ser(a)  // size
	if (!ext)
	{	eeprom_read_block((void*)&par, (void*)a, n);  a+=n;  }
	else
	{	uint8_t* up = (uint8_t*)&par;
		for (i=0; i < n; ++i)
			up[i] = ERead(a);
	}

	if (par.startScreen >= ST_ALL)
		par.startScreen = ST_ALL-1;
	setBright = 1;  // upd
	
	//  old versions  --

	//  zero
	memset(set.key, 0, sizeof(set.key));

	//  Keys  ---
	for (i=0; i < set.scanKeys; ++i)
	{
		uint8_t len = ERead(a);
		if (len > KC_MaxLayers)
		{	err=E_lay;  return;  }
		Ser(a)

		for (n=0; n < len; ++n)
		{
			b = ERead(a);  Ser(a)
			set.key[n][i] = b;
		}
	}

	//  Seqs  ---
	for (i=0; i < set.seqSlots; ++i)
	{
		uint8_t len = ERead(a);

		set.seqs[i].clear();
		for (n=0; n < len; ++n)
		{
			b = ERead(a);  Ser(a)
			set.seqs[i].add(b);
		}
	}

	if (set.seqSlots < KC_MaxSeqs)
		set.seqSlots = KC_MaxSeqs;  // now more than in ee

	memSize = a;
	if (ext)
		EE_SPI_End();
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

	int a,a0, i, size;
    //  buffered
	uint8_t buf[ESlotSize]={0};

	//  header
	set.h1 = 'k';  set.h2 = 'c';  set.ver = 6;  // cur
	++par.verCounter;  // inc ver
	a = FillConfig(buf);
	
	memSize = a;
	if (memSize >= ESlotSize)
		err=E_size;
	size = min(memSize, ESlotSize);  // dont write more

#ifdef EEPROM_CS
	if (saveExt)
	{
		a0 = int(eSlot) * int(ESlotSize);  a = 0;
		EE_SPI_Start();

		//  wite in EEPage size pages
		int numPages = size / EEPage;
		int restBytes = size % EEPage;

		for (i=0; i < numPages; ++i, a+=EEPage)
			EE_SPI_Write(a0 + a, &buf[a], EEPage);
		if (restBytes > 0)
			EE_SPI_Write(a0 + a, &buf[a], restBytes);

		EE_SPI_End();
	}else
#endif
	{	a = 0;
		for (i=0; i < size; ++i, ++a)
			eeprom_write_byte((uint8_t*)a, buf[i]);
	}
}


//  Fill write buffer
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
int KC_Main::FillConfig(uint8_t* buf)
{
	//auto Write = [&](uint8_t b) {  if (a < ESlotSize)  buf[a++] = b;  };
	#define Write(b)  buf[a++] = b

	int a = 0, i, n;
	Write(set.h1);
	Write(set.h2);
	Write(set.ver);

	//  matrix
	Write(set.rows);
	Write(set.cols);
	Write(set.seqSlots);


	//  params  ----
	n = sizeof(par);
	Write(n);  // size

	const uint8_t* up = (const uint8_t*)&par;
	for (i=0; i < n; ++i)
		Write(up[i]);


	//  Keys  ---
	for (i=0; i < set.scanKeys; ++i)
	{
		//  get len, find last non zero
		uint8_t len = KC_MaxLayers;
		for (int l = KC_MaxLayers-1; l >= 0; --l)
			if (set.key[l][i] == KEY_NONE)  len = l+1;
			else  break;

		Write(len);
		for (n=0; n < len; ++n)
		{	Write(set.key[n][i]);  }
	}

	//  Seqs  ---
	for (i=0; i < set.seqSlots; ++i)
	{
		const KC_Sequence& s = set.seqs[i];
		uint8_t len = s.len();
		Write(len);

		for (n=0; n < len; ++n)
		{	Write(s.data[n]);  }
	}
	return a;
}
