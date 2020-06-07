#include "demos.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_OpenSans.h"


//  Version
//....................................................................................
void Demos::Version()
{
	//  logo, ver
	d->setCursor(0,18);
	d->setFont( OpenSans28 );
	d->setTextColor(RGB(1,28,28));
	d->println("CrystaL");

	d->setCursor(36,60);
	d->setTextColor(RGB(22,16,31));
	d->print("Keyboard");
	d->setFont( OpenSans20 );

	d->setCursor(0,120);
	d->setTextColor(RGB(12,21,31));
	d->print("K.C. \"Kacey\" Controller");

	d->setCursor(0,160);
	d->setTextColor(RGB(18,20,31));
	d->print("ver 4.01");
	//  version text  ^

	d->setTextColor(RGB(21,26,31));
	d->setCursor(0, H-30);
	const char* a={__DATE__}, *m={__TIME__};
	const char dt[] = {  //  build date, time   format yyyy-mmm-dd hh:mm
		a[7],a[8],a[9],a[10],' ',a[0],a[1],a[2],' ',a[4],a[5],' ',' ',m[0],m[1],':',m[3],m[4],0};
	d->print(dt);
}


#ifdef DEMOS

//  regular polygons with diagonals
//....................................................................................
void Demos::Ngons()
{
	#define ngtM 200
	float a=0, b=0, e = 2*PI / ngCur;  // angles
	float tm = float(ngt)/ngtM;
	float c = 0.0, s = 0.8;

	switch (ngRot)
	{
	case 0: c = -e*2 * tm;  s = 0.85 + 0.75 * tm;  break;
	case 1: c = -e * tm;  s = 1.0;  break;
	case 2: c = 0.0;  s = 0.8 + 0.6 * tm;  break;
	case 3: c = 0.0;  s = 1.0;  break;
	}
	float xx = W/2, yy = H/2, sy = s * yy;

	uint8_t cc=0;
	for (b=0; b < 2*PI; b+=e)
	{
		float xb = xx + sy*cos(b+c), yb = yy - sy*sin(b+c);
		for (a=0; a < b && a < 2*PI; a+=e)
		{
			float xa = xx + sy*cos(a+c), ya = yy - sy*sin(a+c);
			if (a != b)
				d->drawLine(xb, yb, xa, ya, RGB(31-cc/32,(cc/4)%32,(cc*2)%32));
			++cc;
		}
	}
	++ngt;
	if (ngt > ngtM) {  ngt = 0;
		if (ngtOn) {  ++ngCur;  if (ngCur > ngMax)  ngCur = ngMin;  }  }

	if (iInfo > 0)
	{
		d->setCursor(0,0);
		//d->setClr(29,29,20);
		d->print("An");  d->println(ngRot);

		d->setCursor(0, H-8);
		d->println(ngCur);  // n sides
	}
}


//  Rain circles
//....................................................................................
void Demos::Rain()
{
	int x,y,r,i, s = rCur ? r2Size : r1Size;
	#define RClr  RGB(random(12)+5, random(20)+8, random(5)+25)
	if (!rCur)  // 0
	{		x=random(W); y=random(H);  r= random(26);  d->drawCircle(x,y,r,0);
			x=random(W); y=random(H);  r= random(24);  d->drawCircle(x,y,r,0);
			x=random(W); y=random(H);  r= random(20);  d->drawCircle(x,y,r,0);

		if ((int)random(100) < 30 + r1Int * 5)
		{	x=random(W); y=random(H);  r= random(6+s)+1+s;  d->drawCircle(x,y,r,RClr);
		}
	}else  // 1
	{
		for (i=0; i < 12 + r2Int; ++i)
		{
			if (random(400) < 10) {  x=random(W); y=random(H);  r= random(28)+20;  d->drawCircle(x,y,r,0);  }
			if (random(200) < 10) {  x=random(W); y=random(H);  r= random(20)+16;  d->drawCircle(x,y,r,0);  }
			if (random(100) < 20) {  x=random(W); y=random(H);  r= random(14)+14;  d->drawCircle(x,y,r,0);  }
			if (random(10)  < 3)  {  x=random(W); y=random(H);  r= random(10)+1;   d->drawCircle(x,y,r,0);  }
		}
		for (i=0; i < 2 + r2Int; ++i)  //if (random(10) < 8)
		{
			if (random(600) < 30) {  x=random(W); y=random(H);  r= s+random(6+s)+3;  d->drawCircle(x,y,r,RClr);  }
			if (random(300) < 40) {  x=random(W); y=random(H);  r= s+random(5+s)+4;  d->drawCircle(x,y,r,RClr);  }
			if (random(100) < 50) {  x=random(W); y=random(H);  r= s+random(4)+2;  d->drawCircle(x,y,r,RClr);  }
									 x=random(W); y=random(H);  r= random(3)+1;  d->drawCircle(x,y,r,RClr);
	}	}
	if (iInfo > 0)
	{
		d->setCursor(0,0);
		d->print("Int ");  d->println(rCur ? r2Int : r1Int);
		d->print("Size "); d->println(s);
		d->print("Cur ");  d->println(rCur);
	}
}
#endif


//  Fonts
//....................................................................................
void Demos::Fonts()
{
	Version();  // todo -
}
