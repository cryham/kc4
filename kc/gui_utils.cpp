#include "gui.h"
#include "matrix.h"
#include "kc_data.h"

#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  time
void Gui::PrintInterval(uint32_t t)
{
	char a[32];
	int ms = t/100%10;
	int s = t/1000;  t=s;  s%=60;
	int m = t/60;  t=m;  m%=60;
	int h = t/60;
	if (h)
		sprintf(a,"%dh%02dm", h, m);
	else if (m)
		sprintf(a,"%dm%02ds", m, s);
	else //if (s)
		sprintf(a,"%d.%ds", s, ms);
	d->print(a);
}


//  key utils
//....................................................................................
void Gui::Check_ym1()
{
	if (ym1[ym] >= YM1[ym])  ym1[ym] = 0;
	if (ym1[ym] < 0)  ym1[ym] = YM1[ym]-1;
}

//  returns pressed scan id or -1
int Gui::PressKey(int8_t& pressKey)
{
	if (!pressKey)  return -1;

	int c = 0, ii = -1;
	for (uint i=0; i < ScanKeys; ++i)
	{	if (Matrix_scanArray[i].state != KeyState_Off)  ++c;
		if (Matrix_scanArray[i].state == KeyState_Press)  ii = i;
	}
	if (pressKey == 1)
	{	//  wait until all keys are off
		if (c == 0)
			pressKey = 2;
		return -1;
	}
	else if (pressKey == 2 && ii != -1)
		return ii;

	return -1;
}

//  key auto repeat,  row, col, ms
int8_t Gui::kr(uint8_t sc, uint16_t dt)
{
	KeyPosition st = Matrix_scanArray[sc].state;
	int16_t& m = Matrix_scanArray[sc].autoRepeat;

	if (st == KeyState_Press)
	{
		m = -1;  // start delay
		return 1;
	}
	else if (st == KeyState_Hold)
	{
		if (m < 0)
		{	m -= dt;
			if (m < -par.krDelay*5)  // delay ms
			{	m = 0;  return 1;  }
		}else
		{	m += dt;
			if (m > par.krRepeat*5)  // repeat freq
			{	m = 0;  return 1;  }
		}
	}
	return 0;
}

//  value add, range
int16_t Gui::RangeAdd(int16_t val, int16_t add, int16_t vmin, int16_t vmax, int8_t cycle)
{
	int16_t v = val;
	if (cycle)
	{	int16_t r = vmax - vmin + 1;
		v = (v + add + r) % r;
	}else
	{	v += add;
		v = min(vmax, max(vmin, v));
	}
	return v;
}


//  load save KC  -------------------
void Gui::Save()
{
	kc.Save();  infType = 2;  tInfo = -1;
}
//  load
void Gui::Load(int8_t reset)
{
	if (reset)
	{	kc.set.InitCK();  infType = 0;  }
	else
	{	kc.Load();  infType = 1;  }
	tInfo = -1;
}

void Gui::KeysLoadSave()
{
	if (kSave)  eLoadSave = ee_Save;
	if (kLoad)  eLoadSave = ee_Load;
}
void Gui::ExecLoadSave()
{
	if (eLoadSave == ee_Save)  Save();
	if (eLoadSave == ee_Load)  Load(kCtrl);
	eLoadSave = ee_None;
}
