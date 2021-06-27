#include "demos.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

// FractalColors, Created by tholzer in 2016-02-11			// https://www.shadertoy.com/view/MsK3zG
// original created by Robert Schütze (trirop) 07.12.2015	// http://glslsandbox.com/e#29611.0 
// modified by CryHam


const char* sFractalName[Demos::FR_ALL] =
{	//  info names
	"1 basic 2 fastest",
	"2 simple 3 fast",
	"3 medium 4",
	"4 twist 4",
	"5 bubbles 6 slow",
	"5 medium 6 slow",
	"6 fractal 8 slow ",  // 7Fps 600, 12Fps 960 OC
	"7 varying 8 slow",
};
const Demos::Fractal Demos::FrSets[Demos::FR_ALL] = {
// iter   x0,y0,z0     xD,yD,zD          Amp,Spd z,y,x                        xM,yM,zM
{2,  0.0f,0.0f,0.24f, 1.00f,0.88f,0.50f, 0.70f,0.051f, 0.0f,0.072f, 0.0f,0.051f, 1.f,1.f,1.f},
{3,  0.0f,0.0f,0.30f, 1.00f,0.88f,0.50f, 0.70f,0.021f, 0.0f,0.063f, 0.0f,0.052f, 1.1f,1.1f,0.8f},
{4,  0.1f,0.1f,0.01f, 0.90f,0.78f,0.50f, 0.30f,0.052f, 0.0f,0.044f, 0.0f,0.076f, 1.f,1.f,1.f},
{4,  0.0f,0.0f,0.18f, 0.90f,0.20f,0.10f, 0.43f,0.041f, 0.4f,0.055f, 0.4f,0.065f, 1.2f,0.7f,0.4f},
{6,  0.1f,0.0f,0.22f, 0.80f,1.10f,0.40f, 0.80f,0.021f, 0.0f,0.046f, 0.0f,0.070f, 1.f,1.f,1.f},
{6,  0.1f,0.0f,0.22f, 0.80f,0.70f,0.70f, 0.30f,0.012f, 0.4f,0.027f, 0.5f,0.022f, 0.9f,0.9f,0.9f},
{8, 0.1f,0.1f,-0.12f, 0.91f,0.70f,0.80f, 0.30f,0.043f,-0.3f,0.048f, 0.2f,0.061f, 0.6f,0.7f,0.7f},
{8,  0.1f,0.0f,0.31f, 0.80f,0.48f,0.50f, 0.40f,0.031f, 0.6f,0.013f, 0.3f,0.022f, 1.f,1.f,1.f}
};


void Demos::FractalDraw()
//....................................................................................
{
	if (!bFrPause)
	{
		tFrZ += fpar.zSpd * frSpd;
		tFrY += fpar.ySpd * frSpd;
		tFrX += fpar.xSpd * frSpd;
	}
	const float zT = fpar.zAmp * sinf(tFrZ);
	const float yT = fpar.yAmp * sinf(tFrY);
	const float xT = fpar.xAmp * sinf(tFrX);

	register uint a = 0;
	register float px,py,pz;
	for (uint y=0; y<H; ++y)
	{
		for (register uint x=0; x<W; ++x, ++a)
		{
			py = float(H-1-y) / (H-1) + fpar.y0;  pz = fpar.z0;
			px = float(x) / (W-1) + fpar.x0;

  			for (register int i=0; i < fpar.iter; ++i)
			{
				const float l = px*px + py*py + pz*pz;  // dot(p,p)
				const float dx = (px / l);
				const float dy = (py / l);
				const float dz = (pz / l);
				px = fpar.xM * fabs(dx - fpar.xD + xT);  // xzy yzx xyz
				pz = fpar.yM * fabs(dy - fpar.yD + yT);
				py = fpar.zM * fabs(dz - fpar.zD + zT);
				px = min(px, 1.f);
				py = min(py, 1.f);
				pz = min(pz, 1.f);
			}
			const int r = int(px*31.f);
			const int g = int(py*63.f);
			const int b = int(pz*31.f);
			data[a] = RGB2(r,g,b);
		}
	}

	if (!iInfo || bFrPause)  return;  // txt

	d->setFont(OpenSans12);
	d->setCursor(0, 1);
	uint16_t c = RGB(28,30,31);
	d->setColor(c,0);
	d->print(yFrPar);  d->print("  ");

	#define Par(par)  d->print(#par);  d->print("  ");  d->println(fpar.par)
	switch (yFrPar)
	{
	case -1:  d->print("Speed ");  d->println(frSpd);  break;
	case 0:  Par(iter);  break;
	case 1:  Par(xD);  break;    case 2:  Par(yD);  break;    case 3:  Par(zD);  break;
	case 4:  Par(z0);  break;    case 5:  Par(x0);  break;    case 6:  Par(y0);  break;
	case 7:  Par(zAmp);  break;  case 8:  Par(zSpd);  break;
	case 9:  Par(yAmp);  break;  case 10: Par(ySpd);  break;
	case 11: Par(xAmp);  break;  case 12: Par(xSpd);  break;
	case 13: Par(xM);  break;  case 14: Par(yM);  break;  case 15: Par(zM);  break;
	}
	#undef Par

	//d->setColor(c,0);
	d->setCursor(0, H-1 - 15);
	d->print(sFractalName[fractal]);
}
