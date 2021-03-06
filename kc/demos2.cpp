#include "def.h"
#include "demos.h"
#include "ILI9341_t3n.h"

//  sin table
const int16_t sint[SX] /*PROGMEM*/ =
{
	#include "sint8k.h"
};


//  Fire
//....................................................................................
void Demos::Fire()
{
	if (bAuto)
		fire = (cnt / 100) % 2;

	switch (fire)
	{
	case 0:  Fire1();  break;
	case 1:  Fire2();  break;
	}

	if (iInfo > 0)
	{
		d->setCursor(0, H-1 - 7);
		d->print("Spd ");  d->print(fireSpd[fire]);
	}
	t += fireSpd[fire];
}

void Demos::Fire1()
{
	uint tt[4]={t*10,t*12, t*8,t*14};

	uint yy[4]={0,0,0,0};
	int cd = 10*SY;
	uint a = 0;
	for (uint y=0; y<H; ++y)
	{
		uint xx[4]={0,0,0,0};
		uint f = 2 + pow(y, 1.15);
		for (uint x=0; x<W; ++x,++a)
		{
			int c;
			c = 6*Sin( (-xx[0]- yy[0])*f/256+f*14 +tt[0] ) + 6*Cos( xx[1]+ yy[1]*f/128-f*32 +tt[1] );
			c+= 6*Sin( (-xx[2]+ yy[2])*f/256+f*18 +tt[2] ) + 6*Cos( xx[3]+ yy[3]*f/128-f*36 +tt[3] );
			c-= 4*Sin( (-xx[1]+ yy[0])*f/128+f*16 +tt[1] ) + 4*Cos( xx[2]+ yy[0]*f/128-f*26 +tt[2] );

			xx[0]+=56; xx[1]+=79; xx[2]+=84; xx[3]+=121;

			c = abs(c);
			c /= cd;
			data[a] = RGB(min(31, c/2), min(31, c/8), c/24);
		}
		yy[0]+=13; yy[1]+=25; yy[2]+=34; yy[3]+=45;
		cd -= cd / 44 + 4;
	}
}

void Demos::Fire2()
{
	uint tt[6]={t*8,t*7, t*5,t*6, t*9,t*10};

	uint yy[6]={0,0,0,0,0,0};
	int cd = 4424*SY;
	uint a = 0;
	for (uint y=0; y<H; ++y)
	{
		uint xx[6]={0,0,0,0,0,0};
		//uint f = 1 + pow(y, 1.25)*4;
		uint f = 1 + pow(y, 1.15);
		for (uint x=0; x<W; ++x, ++a)
		{
			int c;
			c = 1*Sin( ( xx[0]+ yy[1])*f/256+f*14 +tt[0] ) * Cos(-xx[4]+ yy[5]*f/256-f*32 +tt[1] );
			c+= 1*Sin( ( xx[2]- yy[0])*f/256+f*16 +tt[2] ) * Cos(-xx[5]+ yy[3]*f/256-f*36 +tt[3] );
			c-= 1*Sin( ( xx[3]+ yy[4])*f/256+f*18 +tt[4] ) * Cos(-xx[1]+ yy[2]*f/256-f*26 +tt[5] );

			xx[0]+=56; xx[1]+=79; xx[2]+=84; xx[3]+=121; xx[4]+=24; xx[5]+=41;

			c = abs(c);
			c /= cd;
			data[a] = RGB(min(31, c/18), min(31, c/12), c/26);
		}
		yy[0]+=13; yy[1]+=25; yy[2]+=34; yy[3]+=45; yy[4]+=21; yy[5]+=28;
		cd -= cd / 52 + 4;
	}
}


//  Wave
//....................................................................................
void Demos::Wave()
{
	uint tt[16]={t*9,t*7,t*5,t*4, t*14,t*13,t*11,t*12, t*21,t*23,t*25,t*28, t*32,t*36,t*31,t*34};
	int16_t yw[W];

	uint xx[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	for (uint x=0; x<W; ++x)
	{
		int c;
		c =11*Sin( xx[0] +tt[0]) + 10*Cos( xx[1] +tt[1]);
		c-=12*Sin( xx[2] +tt[2]) + 10*Cos( xx[3] +tt[3]);
		c+= 8*Cos( xx[4] +tt[4]) +  8*Sin( xx[5] +tt[5]);
		c-=10*Cos( xx[6] +tt[6]) +  9*Sin( xx[7] +tt[7]);
		c+= 7*Sin( xx[8] +tt[8]) +  8*Cos( xx[9] +tt[9]);
		c-= 8*Sin( xx[10]+tt[10])+  6*Cos( xx[11]+tt[11]);
		c+= 3*Cos( xx[12]+tt[12])+  3*Sin( xx[13]+tt[13]);
		c-= 2*Cos( xx[14]+tt[14])+  2*Sin( xx[15]+tt[15]);

		xx[0]+=22; xx[1]+=35; xx[2]+=31; xx[3]+=45; xx[4]+=66; xx[5]+=93;
		xx[6]+=131; xx[7]+=293; xx[8]+=458; xx[9]+=229; xx[10]+=388; xx[11]+=1256;
		xx[12]+=633; xx[13]+=1928; xx[14]+=836; xx[15]+=3301;

		c /= 60*(SY/(H/2));
		yw[x] = c;
	}

	uint a = 0;
	for (uint y=0; y<H; ++y)
	{
		for (uint x=0; x<W; ++x, ++a)
		{
			int c = yw[x], e, h = H/2 - (c > 0 ? c/2 : c/3);
			int d = y - h - c;  e = abs(d)/4;
			d = h - abs(d);  d = max(0,d);
			d = d == h ? 32*16 : d;  // dots
			c = RGB(min(31, e/6 + d/12),
					min(31, e/6 + d/6),
					min(31, e/3 + d/3));
			data[a] = c;
		}
	}

	if (iInfo > 0)
	{
		d->setCursor(0, H-1 - 7);
		d->print("Spd ");  d->print(waveSpd);
	}
	t += waveSpd;
}


//  CrystaL KeyboarD  logo lines
//....................................................................................

//  x,y points, 0 break
const static int word1[] = {
/*C*/194,43, 120,133, 8,193, 112,233, 224,299, 0,
	224,299, 141,227, 78,194, 144,139, 194,43, 0,
/*R*/192,110, 260,156, 211,176, 164,201, 220,153, 192,110, 0,
	164,201, 248,222, 314,258, 298,276, 251,237, 164,201, 0,
/*Y*/303,189, 388,224 , 450,190, 398,256 , 331,328, 374,242, 303,189, 0,
/*S*/541,92, 439,152, 533,196, 435,267, 507,226, 574,197, 517,173, 473,147, 541,92, 0,
/*T*/544,62, 628,61, 692,33, 629,75, 592,167, 615,76, 544,62, 0,
/*A*/623,216, 660,154, 673,82, 702,152, 746,206, 679,144, 623,216, 0,
/*L*/766,28, 793,147, 781,233, 738,300, 818,256,
	935,220, 815,229, 800,133, 766,28, 0, -100};

const static int word2[] = {
/*K*/217,345, 152,396, 62,440, 176,468, 288,507, 248,481, 117,438, 173,397, 217,345, 0,
	76,367, 118,471, 130,541, 145,516, 76,367, 0,
/*e*/329,359, 282,404, 207,434, 292,471, 368,517, 306,467, 256,438,
	318,433, 257,429, 297,401, 329,359, 0,
/*y*/357,450, 386,488, 424,507, 387,541, 334,561, 373,568, 427,521,
	468,453, 431,497, 357,450, 0,
/*b*/466,369, 492,399, 523,412, 486,435, 540,457, 505,476, 474,517,
	506,459, 486,435, 494,414, 466,369, 0,
/*o*/533,432, 557,414, 571,382, 587,409, 609,426, 591,452, 576,496, 559,455, 533,432, 0,
	573,409, 595,426, 575,461, 552,433, 573,409, 0,
/*a*/583,535, 608,493, 628,444, 649,477, 675,506, 630,481, 583,535, 0,
/*r*/660,385, 694,407, 651,427, 727,471, 706,479, 682,450,
	651,427, 670,407, 660,385, 0,
/*D*/717,363, 796,400, 861,432, 805,479, 761,504, 726,543,
	749,501, 795,445, 776,411, 717,363, 0, -100};

const static struct CK_par
{	//  centers,  amplitudes  x  words[2]
	int cx[2],cy[2], ax[2],ay[2],  // wave params
		count, far, fade;  // shadow params
}
ck_par[Demos::ckMax] = {
	{ 496, 402,  130, 180,  222, 262,  362, 282,  32, 2, 3 },
	{ 396, 302,  250, 320,  222, 312,  482, 342,  62, 2, 2 },
	{ 411, 511,  180, 160,  422, 402,  542, 412,  62, 2, 2 },
	{ 756, 472,   50, 520,  322, 402,  402, 442,  42, 3, 5 }};

void Demos::CK_logo()
{
	if (bAuto)
		ckCur = (cnt / 200) % ckMax;

	#define K 1024  // wave																		// scale ofs
	#define CX(x) {  x= w-cx;  x=( (x*(K +ax*Cos(8*w      +tt[0])/SY*Sin(7*w      +tt[1])/SY) /K) +cx)*2/7 +24;  }
	#define CY(y) {  y= w-cy;  y=( (y*(K +ay*Cos(9*w+ x*73+tt[2])/SY*Sin(6*w+ x*52+tt[3])/SY) /K) +cy)*2/6 +(w2 ? 16 :7);  }

	const auto& p = ck_par[ckCur];

	const int jj = p.count;  // old shadows
	for (int j = jj; j >= 0; --j)
	{
		const uint tj = t - j * ckSpeed * p.far / 2;
		const uint tt[4] = {tj*7, tj*5, tj*8, tj*5};
		
		for (int w2=0; w2<2; ++w2)  // words
		{
			const int cx = p.cx[w2], cy = p.cy[w2],
					ax = p.ax[w2]*3/2, ay = p.ax[w2]*3/2;
			int a=0,w, i=0,rst=1, n=0,
				x1=0,y1=0, x=0,y=0;
			do
			{	w = w2 ? word2[a++] : word1[a++];
				if (w<=0) {  rst=1;  i=0;  ++n;  }
				else
				if (rst)  switch(i)
				{	case 0:  CX(x)  ++i;  break;
					case 1:  CY(y)  rst=0; i=0;  break;  }
				else  switch(i)
				{	case 0:  x1=x;  CX(x)  ++i;  break;
					case 1:  y1=y;  CY(y)  i=2;  break;  }

				if (i==2)
				{	i=0;  const int cm = 31 - j * p.fade / 4;
					d->drawLine( x1,y1, x,y,
					w2 ? RGB(max(8, cm -n   -a%6), max(4, cm -n*2 -a%8), cm)
					   : RGB(max(4, cm -n*2 -a%9), max(8, cm -n   -a%7), cm));  }
			}
			while (w >= 0);
		}
	}

	if (iInfo > 0)
	{
		d->setCursor(0,0);
		d->print("Cur ");  d->println(ckCur);
		d->print("Spd ");  d->println(ckSpeed);
	}
	t += ckSpeed;
}
