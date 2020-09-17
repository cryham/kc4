#include "gui.h"
#include "kc_params.h"
#include "kc_data.h"

#include "TimeLib.h"
extern KC_Main kc;


//  Clock
//....................................................................................
void Gui::KeysClock()
{
	if (kUp)  // y
	{	ym2Clock = RangeAdd(ym2Clock, kUp, 0, ClockVars(pgClock), 1);  }
	else
	if (kPgUp)  // pg
	{	pgClock = RangeAdd(pgClock, kPgUp, 0, Cl_All-1, 1);
		ym2Clock = RangeAdd(ym2Clock, 0, 0, ClockVars(pgClock), 1);
	}
	else
	//  adjust time  ---
	if (kRight && pgClock == Cl_Adjust)
	{
		TimeElements t;
		breakTime(rtc_get(), t);

		int a = kRight * (kCtrl ? 10 : 1);
		auto add = [&](uint8_t& u, uint8_t vmin, uint8_t vmax)
		{
			if ((a > 0 && u+a <= vmax) ||
				(a < 0 && u+a >= vmin))
			u += a;
		};
		if (a)
		{
			switch (ym2Clock)
			{
			case 0:  add(t.Hour, 0, 23);  break;
			case 1:  add(t.Minute, 0, 59);  break;
			case 2:  add(t.Second, 0, 59);  break;

			case 3:  add(t.Day, 1, 31);  break;
			case 4:  add(t.Month, 1, 12);  break;
			case 5:  t.Year += a;  break;

			case 6:  par.rtcCompensate += a;
					 rtc_compensate(par.rtcCompensate);  break;
			}
			time_t tm = makeTime(t);
			setTime(tm);
			rtc_set(tm);

			// if (kc.tm_on + td >= 0)  uptime diff..
			// 	kc.tm_on += td;
			kc.ResetStats(false);
		}
	}
	//  graphs  cursor move  ---
	else if (pgClock == Cl_Graphs || pgClock == Cl_GraphsDaily)
	{
		if (kEnd)
			par.xCur = W-1;  // to end
		if (kMul)
		{	if (par.xCur == W)  par.xCur = W-1;  // show
			else  par.xCur = W;  // hide
		}
		int a = kRight * (kCtrl ? 30 : kSh ? 1 : 10);
		if (a)
			par.xCur = RangeAdd(par.xCur, a, 0, W-1, 1);
	}

	if (kAdd || kBckSp)  --mlevel;

	if (pgClock == Cl_Adjust || pgClock == Cl_Graphs)
		KeysLoadSave();
}
