#include "demos.h"
#include "ILI9341_t3n.h"
#include "gui.h"  // keys


//  Main
Demos::Demos()
{
	Init(nullptr);
}
//....................................................................................
void Demos::Init(ILI9341_t3n* tft)
{
	d = tft;  if (d)  data = d->getFrameBuffer();
	ti = 0;  oti = 0;
	
#ifdef DEMOS  // params
	iFps = 0;  iInfo = 0;
	fntCur = 0;

#ifdef DEMOS_OLD_PAR
	einit = INone;
	sCnt = 3*sMax/4;  sVel = 17;  // stars
	bCnt = min(200,bMax);  bSpd = 120;  bSpRnd = 1;  bRad = 3;  // balls
	fInt = 3;  fWave = 1;  // fountain

	ngtOn = 1;  // ngons
	ngt = 0;  ngCur = 12;  ngRot = 0;
#endif

	r1Int = 2;  r1Size = 2;  // rain
	r2Int = 4;  r2Size = 3;  rCur = 1;
	
	hdtOn = 1;  // hedrons
	hdt = 0;  hdCur = 9;  hdRot = 0;
	hdSpd = 1;  hdDiag = 1;

	ckCur = 1;  ckSpeed = 8;  // logo

	plasma = 2;  t = 13210;  // plasma
	tadd[0]=5; tadd[1]=7; tadd[2]=6; tadd[3]=8; tadd[4]=7; tadd[5]=7;
	tadd[6]=3; tadd[7]=3; tadd[8]=2; tadd[9]=5;

	waveSpd = 8;  // wave
	fire = 1;
	fireSpd[0]=19; fireSpd[1]=17;  // fire
#endif
}


#if 0  // todo: def DEMOS

//  Key Press  demo params
//....................................................................................
void Demos::KeyPress(EDemo demo, Gui* gui)
{
	//  global  //  fps, info txt
	if (gui->kMul){  iInfo = 1 - iInfo;  return;  }
	if (gui->kSub){  iFps = (iFps + 1) % 3;   return;  }

	int8_t k = gui->kRight, u = -gui->kUp,
		pgup = gui->kPgUp, end = gui->kEnd, ct = gui->kCtrl;
	#ifdef DEMOS_OLD_PAR
	int sp = gui->kSh ? 2 : 10;
	#endif

	if (k || u || pgup || end)
	{	//iInfo = -1;
		switch (demo)
		{
	#ifdef DEMOS_PLASMA
		//  full  --------
		case D_Plasma:
			if (k)  plasma = (plasma + k + num_plasma) % num_plasma;
			if (u)  PlasmaT(u);
			break;
	#endif

	#ifdef DEMOS_3D
		case D_Hedrons:  // 3d
			if (pgup)
				hdDiag = (hdDiag + pgup + hdDiagMax) % hdDiagMax;
			if (end)
				hdSpd += end;
			if (k)
			{	hdtOn = 0;  // manual
				hdCur = (hdCur + k + hdA) % hdA;  }
			if (u)
				hdRot = (hdRot + u + hdRotMax) % hdRotMax;
			break;
	#endif

		case D_Fire:
			if (k)  fire = (fire + k + num_fire) % num_fire;
			if (u)  fireSpd[fire] += u;
			break;

		case D_Wave:
			if (k)  waveSpd += k;
			if (u)  waveSpd += u*10;
			break;

		//  txt	 --------
		case D_CK_Logo:
			if (k)  ckCur = (ckCur + k + ckMax) % ckMax;
			if (u)  ckSpeed += u;
			break;

		//  old  --------
	#ifdef DEMOS_OLD_PAR
		case D_Ngons:
			if (k)
			{	ngtOn = 0;  // manual
				ngCur += k;
				if (ngCur < ngMin)  ngCur = ngMax;
				if (ngCur > ngMax)  ngCur = ngMin;  }
			if (u)
			{	++ngRot;
				if (ngRot >= ngRotMax)  ngRot = 0;  }
			break;

		case D_Space:
			if (k){  sCnt += k*sp;  sCnt = max(0, min(sMax, sCnt));  einit = INone;  }
			if (u){  sVel += u;     sVel = max(0, min(60, sVel));  einit = INone;  }
			break;
			
		case D_Balls:
			if (ct)
			{	if (k){  bRad += k;      bRad = max(1, min(8, bRad));  einit = INone;  }
				if (u){  bSpRnd += u;  bSpRnd = max(0, min(bSpRMax, bSpRnd));  einit = INone;  }
			}else
			{	if (k){  bCnt += k*sp;    bCnt = max(0, min(bMax, bCnt));  einit = INone;  }
				if (u){  bSpd += u*sp*2;  bSpd = max(0, min(400, bSpd));  einit = INone;  }
			}break;

		case D_Fountain:
			if (k)  fInt = (fInt + k + 12) % 12;
			if (u)  fWave += u;
			break;

		case D_Fonts:
			if (k)  fntCur = (fntCur + k + fntMax) % fntMax;
			break;
	#endif
		case D_Rain:
			if (!ct){  if (!rCur)
			{	if (k){  r1Int += k;  if (r1Int < -6)  r1Int = -6;  }
				if (u)   r1Size += u;  r1Size = max(0, min(4, r1Size));
			}else{
				if (k){  r2Int += k;  if (r2Int < -2)  r2Int = -2;  }
				if (u)   r2Size += u;  r2Size = max(0, min(4, r2Size));
			}	}
			if (ct && k)  rCur = 1-rCur;
			break;

		default:
			break;
		}
	}
}
#endif
