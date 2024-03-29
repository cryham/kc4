#include "gui.h"
#include "matrix.h"
#include "kbd_layout.h"
#include "keys_usb.h"
#include "kc_params.h"
#include "kc_data.h"

#include "IntervalTimer.h"
extern IntervalTimer tim;

#ifdef REMOTE
	#include <RH_ASK.h>
	#include <SPI.h>
	extern RH_ASK remote;
#endif


const uint8_t Gui::DispPages[Di_All] = {3,3,4,3};
const uint8_t Gui::ScanPages[St_All] = {3,1,4,1};
const uint8_t Gui::ConfigPages[Cf_All] = {4,0,0};


//  Config
//....................................................................................
void Gui::KeysConfig(int sp)
{
	int16_t yip = ConfigPages[yy];
	switch (yy)
	{
	case Cf_Storage:
		if (kUp)
		{	ym2Storage  = RangeAdd(ym2Storage, kUp, 0, yip, 1);  }
		else
		if (kRight)
		switch (ym2Storage)
		{
		case 0:  par.verCounter = RangeAdd(par.verCounter, kRight * sp, 0, 255, 1);  break;
		case 1:  kc.eSlot = RangeAdd(kc.eSlot, kRight, 0, ESlots-1, 1);  break;
		case 2:  kc.loadExt = RangeAdd(kc.loadExt, kRight, 0, 1, 1);  break;
		case 3:  kc.saveExt = RangeAdd(kc.saveExt, kRight, 0, 1, 1);  break;
		case 4:  memVisAdr = RangeAdd(memVisAdr, kRight, -1, ESlotSize/16/10-1);  break;
		}	break;
	}
	KeysLoadSave();
}


//  Setup
//....................................................................................
void Gui::KeysSetup(int sp)
{
	int16_t ysp = ScanPages[yy];
	switch (yy)
	{
	case St_Layer:
		if (kUp)
		{	ym2Lay  = RangeAdd(ym2Lay, kUp, 0, ysp, 1);  }
		else
		if (kRight)
		switch (ym2Lay)
		{
		case 0:  par.defLayer = RangeAdd(par.defLayer, kRight, 0, KC_MaxLayers-1);  break;
		case 1:  par.editLayer = RangeAdd(par.editLayer, kRight, 0, KC_MaxLayers-1);  break;
		case 2:  par.msLLTapMax = RangeAdd(par.msLLTapMax, kRight, 0, 250);  break;
		case 3:  par.msLLHoldMin = RangeAdd(par.msLLHoldMin, kRight, 0, 90);  break;
		}	break;

	case St_Keyboard:
		if (pressGui)
		{
			int ii = PressKey(pressGui);
			if (ii != -1)
			switch (ym2Keyb)
			{
			case 3:  par.keyGui = ii;  pressGui = 0;  break;
			}
		}
		else if (kUp)
		{	ym2Keyb  = RangeAdd(ym2Keyb, kUp, 0, ysp, 1);  }
		else
		if (kRight)
		switch (ym2Keyb)
		{
		case 0:  par.dtSeqDef = RangeAdd(par.dtSeqDef, kRight * sp, 0, 250);  break;
		case 1:  pressGui = 1;  break;
		}	break;

	case St_Mouse:
		if (pressGui)
		{
			int ii = PressKey(pressGui);
			if (ii != -1)
			switch (ym2Mouse)
			{
			case 2:  par.keyMouseSlow = ii;  pressGui = 0;  break;
			}
		}
		else if (kUp)
		{	ym2Mouse = RangeAdd(ym2Mouse, kUp, 0, ysp, 1);  }
		else
		if (kRight)
		switch (ym2Mouse)
		{
		case 0:  par.mkSpeed = RangeAdd(par.mkSpeed, kRight * sp, 0, 250);  break;
		case 1:  par.mkAccel = RangeAdd(par.mkAccel, kRight * sp, 0, 250);  break;
		case 2:  pressGui = 1;  break;
		case 3:  par.mkWhSpeed = RangeAdd(par.mkWhSpeed, kRight * sp, 0, 250);  break;
		case 4:  par.mkWhAccel = RangeAdd(par.mkWhAccel, kRight * sp, 0, 250);  break;
		}	break;

	case St_Remote:
		if (kUp)
		{	ym2Remote  = RangeAdd(ym2Remote, kUp, 0, ysp, 1);  }
		else
		if (kRight)
		switch (ym2Remote)
		{
	#ifdef REMOTE_SEND
		case 0:  // send
		{	const uint8_t s = 64;
			uint8_t a[s] = {remoteId, remoteId, 0xAA, 0x11, 0x33};
			// for (int i=0; i < s; ++i)
			// 	a[i] = remoteId;
			remote.send(a, 4);
			//remote.waitPacketSent();
			delay(100);
			++remoteId;
		}	break;
	#endif

	#ifdef REMOTE_RECV
		/*case 1:  // receive  in main
		{	uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
			uint8_t buflen = sizeof(buf);

			if (remote.recv(buf, &buflen)) // Non-blocking
			{
				for (int i=0; i < buflen; ++i)
					remoteData[i] = buf[i];
				++remoteId;
			}
		}	break;*/
	#endif
		}	break;
	}
	KeysLoadSave();
}


//  Matrix Scan
//....................................................................................
void Gui::KeysMatrix(int sp)
{
	int16_t ysp = 2;  //ScanPages[yy];
	switch (yy)
	{
	case Mx_Scan:
		if (kUp)
		{	ym2Scan = RangeAdd(ym2Scan, kUp, 0, ysp, 1);  }
		else
		if (kRight)
		switch (ym2Scan)
		{
		case 0:  par.scanFreq = RangeAdd(par.scanFreq, kRight * (kSh ? 1 : 4), 2, 250);
				 tim.update( 1000000 / (par.scanFreq * 20) );  break;  // upd
		case 1:  par.strobe_delay = RangeAdd(par.strobe_delay, kRight, 0, 50);  break;
		case 2:  par.debounce = RangeAdd(par.debounce, kRight, 0, 50);  break;
		}	break;
	}
	KeysLoadSave();
}


//  Display
//....................................................................................
void Gui::KeysDisplay(int sp)
{
	if (kUp)  // y
	{	ym2Disp = RangeAdd(ym2Disp, kUp, 0, DispPages[pgDisp], 1);  }
	else
	if (kPgUp)  // pg
	{	pgDisp = RangeAdd(pgDisp, kPgUp, 0, Di_All-1, 1);
		ym2Disp = RangeAdd(ym2Disp, 0, 0, DispPages[pgDisp], 1);
	}else
	if (kRight)  // adjust values
	switch (pgDisp)
	{
	case Di_Bright:
		switch (ym2Disp)
		{
		case 0:  par.brightness = RangeAdd(par.brightness, kRight * sp, 0, 100);
				 kc.setBright = 1;  break;
		case 1:  par.brightOff = RangeAdd(par.brightOff, kRight * sp, 0, 100);  break;
		#ifdef LED_LAMP
		case 2:  kc.ledOn = kRight > 0;  kc.LedUpdate();  break;
		case 3:  par.ledBright = RangeAdd(par.ledBright, kRight * sp, 0, 64);
				 kc.LedUpdate();  break;
		#endif
		}	break;

	case Di_Keys:
		switch (ym2Disp)
		{
		case 0:  par.startScreen = RangeAdd(par.startScreen, kRight, 0, ST_ALL-1);  break;
		case 1:  par.krDelay = RangeAdd(par.krDelay, kRight, 0,255);  break;
		case 2:  par.krRepeat = RangeAdd(par.krRepeat, kRight, 0,255);  break;
		case 3:  par.quickKeys = RangeAdd(par.quickKeys, kRight, 0, 1);  break;
		}	break;

	case Di_StatsGraph:
		switch (ym2Disp)
		{
		case 0:  par.time1min = RangeAdd(par.time1min, kRight * sp/2, 0, gIntvMask, 1);  break;
		case 1:  par.minInactive = RangeAdd(par.minInactive, kRight *sp/2, 0, 60, 1);  break;
		case 2:  par.timeTemp = RangeAdd(par.timeTemp, kRight * sp/2, 0, gIntvMask, 1);  break;
		case 3:  par.timeTgraph = RangeAdd(par.timeTgraph, kRight * sp/2, 0, gIntvMask, 1);  break;
		case 4:  par.dailyHours = RangeAdd(par.dailyHours, kRight * sp/2, 0, gIntvMask, 1);  break;
		}	break;

	case Di_AdjustTemp:
		switch (ym2Disp)
		{
		case 0:  demos.iFps = RangeAdd(demos.iFps, kRight, 0, 2);  break;
		case 1:  par.tempOfs = RangeAdd(par.tempOfs, kRight * sp, -128, 127, 1);  break;
		case 2:  par.minTemp = RangeAdd(par.minTemp, kRight * sp/2, 0, 40, 1);  break;
		case 3:  par.maxTemp = RangeAdd(par.maxTemp, kRight * sp/2, 0, 40, 1);  break;
		}	break;
	}
	if (kAdd || kBckSp)  --mlevel;

	KeysLoadSave();
}
