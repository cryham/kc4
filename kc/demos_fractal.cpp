#include "demos.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"

//  FractalColors created by tholzer  11.02.2016  https://www.shadertoy.com/view/MsK3zG
//  Original by Robert Schütze trirop 07.12.2015  http://glslsandbox.com/e#29611.0
//  Modified and adapted by CryHam


const char* sFractalNames[Demos::FR_ALL] =  //  presets
{	//  info names     Fps @600  @960 OC
	"1 Basic 2 fastest",  // 27  43
	"2 Simple 3 fast",    // 19  31
	"3 Medium 4",         // 15  24
	"4 Twist 4",          // 14  24
	"5 Bubbles 6 slow",   // 10  17
	"5 Medium 6 slow",    // 10  16
	"6 Fractal 8 slow ",  // 7   12
	"7 Varying 8 slow",   // 7   12
};

Demos::Fractal Demos::sFractalSets[Demos::FR_ALL] = {
// iter   x0,y0,z0     xD,yD,zD          Amp,Spd z,y,x                           xM,yM,zM
{2,  0.0f,0.0f,0.24f, 1.00f,0.88f,0.50f, 0.70f,0.051f, 0.0f,0.072f, 0.0f,0.042f, 1.f, 1.f, 1.f},
{3,  0.0f,0.0f,0.30f, 1.00f,0.88f,0.50f, 0.70f,0.041f, 0.0f,0.063f, 0.0f,0.052f, 1.1f,1.1f,0.8f},
{4,  0.1f,0.1f,0.01f, 0.90f,0.78f,0.50f, 0.30f,0.052f, 0.0f,0.044f, 0.0f,0.076f, 1.f, 1.f, 1.f},
{4,  0.0f,0.0f,0.18f, 0.90f,0.20f,0.10f, 0.43f,0.041f, 0.4f,0.055f, 0.4f,0.065f, 1.2f,0.7f,0.4f},
{6,  0.1f,0.0f,0.22f, 0.80f,1.10f,0.40f, 0.80f,0.021f, 0.0f,0.046f, 0.0f,0.070f, 1.f, 1.f, 1.f},
{6,  0.1f,0.0f,0.22f, 0.80f,0.70f,0.70f, 0.30f,0.012f, 0.4f,0.027f, 0.5f,0.022f, 0.9f,0.9f,0.9f},
{8, 0.1f,0.1f,-0.12f, 0.91f,0.70f,0.80f, 0.30f,0.043f,-0.3f,0.048f, 0.2f,0.061f, 0.6f,0.7f,0.7f},
{8,  0.1f,0.0f,0.31f, 0.80f,0.48f,0.50f, 0.40f,0.031f, 0.6f,0.013f, 0.3f,0.022f, 1.f, 1.f, 1.f}
};


void Demos::FractalDraw()
//....................................................................................
{
	if (bAuto)
		fractal = (cnt / 150) % FR_ALL;

	const Fractal* fpar = &sFractalSets[fractal];

	if (!bFrPause)
	{
		tFrZ += fpar->zSpd * frSpd;
		tFrY += fpar->ySpd * frSpd;
		tFrX += fpar->xSpd * frSpd;
	}
	const float zT = fpar->zAmp * sinf(tFrZ);
	const float yT = fpar->yAmp * sinf(tFrY);
	const float xT = fpar->xAmp * sinf(tFrX);

	register uint a = 0;
	for (uint y=0; y<H; ++y)
	{
		const float py0 = float(H-1-y) / (H-1) + fpar->y0;
		for (register uint x=0; x<W; ++x, ++a)
		{
			register float px,py,pz;
			py = py0;  pz = fpar->z0;
			px = float(x) / (W-1) + fpar->x0;

			for (register int i=0; i < fpar->iter; ++i)
			{
				const float l = px*px + py*py + pz*pz;  // dot(p,p)
				const float dx = (px / l);
				const float dy = (py / l);
				const float dz = (pz / l);
				px = fpar->xM * fabs(dx - fpar->xD + xT);  // xzy yzx xyz
				pz = fpar->yM * fabs(dy - fpar->yD + yT);
				py = fpar->zM * fabs(dz - fpar->zD + zT);
				px = min(px, 1.f);
				py = min(py, 1.f);
				pz = min(pz, 1.f);
			}
			const int r = int(px*31.f);
			const int g = int(py*63.f);
			const int b = int(pz*31.f);
			uint16_t c = RGB2(r,g,b);
			data[a] = c;
	}	}


	if (!iInfo || bFrPause)  return;  // txt name

	uint16_t c = RGB(28,30,31);
	d->setFont(OpenSans12);
	d->setColor(c,0);
	d->setCursor(0, H-1 - 15);
	d->print(sFractalNames[fractal]);

	if (bAuto)  return;  // txt par e

	d->setCursor(0, 1);
	d->print(yFrPar);  d->print("  ");

	#define Par(par)  d->print(#par);  d->print("  ");  d->println(fpar->par)
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
}

void Demos::KeyFractal(int k, const float fa)
{
	Fractal* fpar = &sFractalSets[fractal];
	switch (yFrPar)
	{
	case -1:  frSpd += fa*10.f;  break;
	case 0:  fpar->iter += k;  break;
	case 1:  fpar->xD += fa;  break;  case 2:  fpar->yD += fa;  break;  case 3:  fpar->zD += fa;  break;
	case 4:  fpar->z0 += fa;  break;  case 5:  fpar->x0 += fa;  break;  case 6:  fpar->y0 += fa;  break;
	case 7:  fpar->zAmp += fa;  break;  case 8:  fpar->zSpd += fa;  break;
	case 9:  fpar->yAmp += fa;  break;  case 10: fpar->ySpd += fa;  break;
	case 11: fpar->xAmp += fa;  break;  case 12: fpar->xSpd += fa;  break;
	case 13: fpar->xM += fa;  break;  case 14: fpar->yM += fa;  break;  case 15: fpar->zM += fa;  break;
	}
}
