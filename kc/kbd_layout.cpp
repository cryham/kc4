#include "kbd_layout.h"
#include "def.h"
#include "keys_usb.h"
#include "pgmspace.h"

const uint16_t clrRect[Omax] = {  // _colors for key types_
//  0 letters  1 digits       2 func F1      3 symbols ,.   4 modifiers L  5 modif R
RGB(9,9,16), RGB(12,11,10),  RGB(6,11,16), RGB(16,16,18),	RGB(11,15,19), RGB(11,19,19),
//  6 ent,tab  7 ent R        8 arrows      9 tiny
RGB(16,14,19), RGB(16,14,19), RGB(5,9,14), RGB(14,15,17) };


#if defined(CKtest)   //  CK test  ------------------------------------------------
#define  wh  kW,kH
const char* CKname = "CK test 2x4";
const int8_t  kW = 16, kH = 16,  fH = 12,  X = -kW;
const uint8_t Y1=32,YY=21, Y2=Y1+YY, Y3=Y2+YY, Y4=Y3+YY, Y5=Y4+YY, Y6=Y5+YY;

const DrawKey drawKeys[nDrawKeys] = {  //  Layout draw  ::
 {90,Y1,wh, '<',3, 1,K_BACK}, {110,Y1,wh, 28 ,3, 7,K_ENT},
{140,Y1,wh, 'e',3, 3,K_ESC},  {140,Y2,wh, '-',3, 5,K_SPACE},
 {20,Y1,wh, 24 ,8, 6,K_UP},
  {0,Y2,wh, 27 ,8, 4,K_LEFT},  {40,Y2,wh, 26 ,8, 2,K_RIGHT},
 {20,Y3,wh, 25 ,8, 0,K_DOWN},
};
//symbols:  2. 3,  7 cir  9 dot` 28 ent
const uint8_t gGui=NO, gMslow=NO, gLoad=5,gSave=NO,gDiv=NO, gEsc=3,gAdd=NO,gEnt=7,gEnt2=NO,
	gRight=2,gLeft=4,gDown=0,gUp=6, gPgDn=NO,gPgUp=NO,gEnd=NO,gHome=NO,
	gCtrl=NO,gSh=NO,gMul=NO,gSub=NO, gIns=NO,gDel=NO, gBckSp=1,gSpc=NO, gC=NO,gV=NO,gX=NO,
	gF1=NO,gF2=NO,gF3=NO,gF6=NO,gF7=NO,gF8=NO,gF9=NO,gF10=NO,gF11=NO,gF12=NO;

#elif defined(CK1)   //  CK1  ------------------------------------------------
#define  wf  kW,fH
#define  wh  kW,kH
const char* CKname = "CK1 8x8";  const int16_t XN = 254;  // pos
const int8_t  kW = 16, kH = 18, fH = 12, /* size */  X = -kW;
const uint8_t Y1=32,YY=18, Y2=Y1+YY, Y3=Y2+YY, Y4=Y3+YY, Y5=Y4+YY, Y6=Y5+YY;

const DrawKey drawKeys[nDrawKeys] = {  //  Layout draw  ::::
{ 0,Y1,wf, 'e',6,NO,K_ESC}, //Esc 13x
{-26,0,wf, '1',2,NO,K_F1},{X, 0,wf, '2',2,16,K_F2}, {X,0,wf, '3',2, 0,K_F3}, {X,0,wf, '4',2,48,K_F4},
{-24,0,wf, '5',2,NO,K_F5},{X, 0,wf, '6',2,NO,K_F6}, {X,0,wf, '7',2,NO,K_F7}, {X,0,wf, '8',2,NO,K_F8},
{-24,0,wf, '9',2,18,K_F9},{X, 0,wf, '0',2,NO,K_F10},{X,0,wf, '1',2,50,K_F11},{X,0,wf, '2',2,34,K_F12},

{ 0,Y2,16,kH, '`',3,NO,K_TILDE}, //~
{-16,0,wh, '1',1,NO,K_1},{X, 0,wh, '2',1,NO,K_2},{X,0,wh, '3',1,NO,K_3},{X,0,wh, '4',1,NO,K_4},
{ X, 0,wh, '5',1,20,K_5},{X, 0,wh, '6',1,23,K_6},{X,0,wh, '7',1,NO,K_7},{X,0,wh, '8',1,NO,K_8},
{ X, 0,wh, '9',1,NO,K_9},{X, 0,wh, '0',1,NO,K_0},
{X,0,wh, '-',3,NO,K_MINUS},{X, 0,wh, '=',3,NO,K_EQUAL}, {X,0,26,kH, '<',7, 2,K_BACK}, //Back 14x

{ 0,Y3,24,kH, 29 ,6,NO,K_TAB}, //Tab
{-24,0,wh, 'Q',0,NO,K_Q},{X, 0,wh, 'W',0,NO,K_W},{X,0,wh, 'E',0, 8,K_E},{X,0,wh, 'R',0,12,K_R},
{ X, 0,wh, 'T',0, 4,K_T},{X, 0,wh, 'Y',0, 7,K_Y},{X,0,wh, 'U',0,15,K_U},{X,0,wh, 'I',0,NO,K_I},
{ X, 0,wh, 'O',0,NO,K_O},{X, 0,wh, 'P',0,NO,K_P},{X,0,wh, '[',3,NO,K_LBRACE},
{X, 0,wh, ']',3,NO,K_RBRACE}, {X,0, 18,kH,'\\',3,58,K_BSLASH}, // \| 14x

{ 0,Y4,30,kH,  2 ,6,NO,K_A}, //Caps
{-30,0,wh, 'A',0,NO,K_A},{X, 0,wh, 'S',0,NO,K_S},{X,0,wh, 'D',0,56,K_D},{X,0,wh, 'F',0,60,K_F},
{ X, 0,wh, 'G',0,52,K_G},{X, 0,wh, 'H',0,55,K_H},{X,0,wh, 'J',0,63,K_J},{X,0,wh, 'K',0,NO,K_K},
{ X, 0,wh, 'L',0,NO,K_L},{X,0,wh, ';',3,NO,K_SEMIC},
{X, 0,wh,'\'',3,NO,K_QUOTE}, {X,0, 28,kH, 28,7,42,K_ENT}, //Ent 13x

{ 0,Y5,36,kH, 's',4,NO,K_LSHIFT}, //L Sh
{-36,0,wh, 'Z',0,NO,K_Z},{X, 0,wh, 'X',0,NO,K_X},{X,0,wh, 'C',0,40,K_C},{X,0,wh, 'V',0,44,K_V},
{ X, 0,wh, 'B',0,36,K_B},{X, 0,wh, 'N',0,39,K_N},{X,0,wh, 'M',0,47,K_M},{X,0,wh, ',',3,NO,K_COMMA},
{ X, 0,wh, '.',3,NO,K_PERIOD},{X,0,wh, '/',3,NO,K_SLASH}, {X, 0, 38,kH, 's',5,NO,K_RSHIFT}, //R Sh 12x

//modif:  L Ctrl, L Gui, L Alt, space, R Alt, App,K_Ss, R Gui, R Ctrl
{ 0,Y6,24,kH, 'c',4,NO,K_LCTRL},{-24,0,16,kH,  9 ,4,NO,K_LGUI},{-16,0,24,kH,'a',4,NO,K_LALT},
{-24,0,90,kH, ' ',6,51,K_SPACE}, //Space
{-90,0,20,kH, 'a',5,NO,K_RALT}, {-20,0,16,kH,  9 ,6,NO,K_RGUI},{-16,0,18,kH,254,5,NO,K_MENU},
{-18,0,26,kH, 'c',5,NO,K_RCTRL}, //R Ct 8x
//numpad: 17x
{XN,Y2,wh,  3 ,6,43,K_NUML},{X, 0,wh, '/',3,46,KP_DIV},{X,0,wh, 'x',3,45,KP_MUL},{X,0,wh,  '-',3,37,KP_SUB},
{XN,Y3,wh, '7',8,11,K_HOME},{X, 0,wh, 24 ,8,14,K_UP},  {X,0,wh, '9',8,13,K_PGUP},{X,0,kW,kH*2+2,'+',3,10,KP_ADD},
{XN,Y4,wh, 27 ,8, 3,K_LEFT},{X, 0,wh,  7 ,6, 6,K_DEL}, {X,0,wh, 26 ,8, 5,K_RIGHT},
{XN,Y5,wh, '1',8,59,K_END}, {X, 0,wh, 25 ,8,62,K_DOWN},{X,0,wh, '3',8,61,K_PGDN},{X,0,kW,kH*2+2, 28,6,57,KP_ENT},
{XN,Y6,kW*2,kH,'_',6,54,K_INS},{-kW*2,0,wh,'.',6,53,K_DEL}, //Ins Del
};
//symbols:  2. 3,  7 cir  9 dot` 28 ent
const uint8_t gGui=34, gMslow=NO, gLoad=0/*F3*/,gSave=48,gDiv=46, gEsc=NO,gAdd=10,gEnt=57,gEnt2=NO,
	gRight=5,gLeft=3,gDown=62,gUp=14, gPgDn=61,gPgUp=13,gEnd=NO,gHome=NO,
	gCtrl=NO,gSh=58/*\*/,gMul=45,gSub=37, gIns=54,gDel=53, gBckSp=2,gSpc=51, gC=40,gV=44,gX=36/*B*/,
	gF1=NO,gF2=16,gF3=NO,gF6=NO,gF7=NO,gF8=NO,gF9=18,gF10=NO,gF11=NO,gF12=50/*F11*/;  //--


#elif defined(CK8)   //  CK8  ------------------------------------------------
#define  wf  kW,fH
#define  wh  kW,kH
const char* CKname = "CK8 20x8";  const int16_t XN = 254;  // pos
const int8_t  kW = 16, kH = 18, fH = 12, /* size */  X = -kW;
const uint8_t Y1=32,YY=18, Y2=Y1+YY, Y3=Y2+YY, Y4=Y3+YY, Y5=Y4+YY, Y6=Y5+YY;

const DrawKey drawKeys[nDrawKeys] = {  //  Layout draw  ::::
{ 0,Y1,wf, 'e',6,157,K_ESC}, //Esc 13x
{-26,0,wf, '1',2, 50,K_F1},{X, 0,wf, '2',2, 49,K_F2}, {X,0,wf, '3',2,109,K_F3}, {X,0,wf, '4',2,149,K_F4},
{-24,0,wf, '5',2, 73,K_F5},{X, 0,wf, '6',2,146,K_F6}, {X,0,wf, '7',2,105,K_F7}, {X,0,wf, '8',2, 45,K_F8},
{-24,0,wf, '9',2, 56,K_F9},{X, 0,wf, '0',2, 76,K_F10},{X,0,wf, '1',2,156,K_F11},{X,0,wf, '2',2, 36,K_F12},

{ 0,Y2,16,kH, '`',3, 57,K_TILDE}, //~
{-16,0,wh, '1',1, 77,K_1},{X, 0,wh, '2',1, 70,K_2},{X,0,wh, '3',1, 69,K_3},{X,0,wh, '4',1, 68,K_4},
{ X, 0,wh, '5',1, 48,K_5},{X, 0,wh, '6',1, 47,K_6},{X,0,wh, '7',1, 67,K_7},{X,0,wh, '8',1, 66,K_8},
{ X, 0,wh, '9',1, 65,K_9},{X, 0,wh, '0',1, NO,K_0},
{X,0,wh, '-',3, NO,K_MINUS},{X, 0,wh, '=',3, 46,K_EQUAL}, {X,0,26,kH, '<',7,116,K_BACK}, //Back 14x

{ 0,Y3,24,kH, 29 ,6,117,K_TAB}, //Tab
{-24,0,wh, 'Q',0, 97,K_Q},{X, 0,wh, 'W',0, 90,K_W},{X,0,wh, 'E',0, 89,K_E},{X,0,wh, 'R',0, 88,K_R},
{ X, 0,wh, 'T',0,108,K_T},{X, 0,wh, 'Y',0,107,K_Y},{X,0,wh, 'U',0, 87,K_U},{X,0,wh, 'I',0, 86,K_I},
{ X, 0,wh, 'O',0, 85,K_O},{X, 0,wh, 'P',0, NO,K_P},{X,0,wh, '[',3, NO,K_LBRACE},
{X, 0,wh, ']',3,106,K_RBRACE}, {X,0, 18,kH,'\\',3,136,K_BSLASH}, // \| 14x

{ 0,Y4,30,kH,  2 ,6, NO,K_A}, //Caps
{-30,0,wh, 'A',0,137,K_A},{X, 0,wh, 'S',0,130,K_S},{X,0,wh, 'D',0,129,K_D},{X,0,wh, 'F',0,128,K_F},
{ X, 0,wh, 'G',0,148,K_G},{X, 0,wh, 'H',0,147,K_H},{X,0,wh, 'J',0,127,K_J},{X,0,wh, 'K',0,126,K_K},
{ X, 0,wh, 'L',0,125,K_L},{X,0,wh, ';',3, NO,K_SEMIC},
{X, 0,wh,'\'',3, NO,K_QUOTE}, {X,0, 28,kH, 28,7, 16,K_ENT}, //Ent 13x

{ 0,Y5,36,kH, 's',4,100,K_LSHIFT}, //L Sh
{-36,0,wh, 'Z',0, 17,K_Z},{X, 0,wh, 'X',0, 10,K_X},{X,0,wh, 'C',0,  9,K_C},{X,0,wh, 'V',0,  8,K_V},
{ X, 0,wh, 'B',0, 28,K_B},{X, 0,wh, 'N',0, 27,K_N},{X,0,wh, 'M',0,  7,K_M},{X,0,wh, ',',3,  6,K_COMMA},
{ X, 0,wh, '.',3,  5,K_PERIOD},{X,0,wh, '/',3, NO,K_SLASH}, {X, 0, 38,kH, 's',5,120,K_RSHIFT}, //R Sh 12x

//modif:  L Ctrl, L Gui, L Alt, space, R Alt, App,K_Ss, R Gui, R Ctrl
{ 0,Y6,24,kH, 'c',4, 53,K_LCTRL},{-24,0,16,kH,  9 ,4,112,K_LGUI},{-16,0,24,kH,'a',4, NO,K_LALT},
{-24,0,90,kH, ' ',6,159,K_SPACE}, //Space
{-90,0,20,kH, 'a',5, NO,K_RALT}, {-20,0,16,kH,  9 ,6, NO,K_RGUI},{-16,0,18,kH,254,5, 25,K_MENU},
{-18,0,26,kH, 'c',5, 13,K_RCTRL}, //R Ct 8x
//numpad: 17x
{XN,Y2,wh,  3 ,6,  1,K_NUML},{X, 0,wh, '/',3,  2,KP_DIV},{X,0,wh, 'x',3,  3,KP_MUL},{X,0,wh,  '-',3, 23,KP_SUB},
{XN,Y3,wh, '7',8, 81,K_HOME},{X, 0,wh, 24 ,8, 82,K_UP},  {X,0,wh, '9',8, 83,K_PGUP},{X,0,kW,kH*2+2,'+',3, 84,KP_ADD},
{XN,Y4,wh, 27 ,8,101,K_LEFT},{X, 0,wh,  7 ,6,102,K_DEL}, {X,0,wh, 26 ,8,103,K_RIGHT},
{XN,Y5,wh, '1',8,121,K_END}, {X, 0,wh, 25 ,8,122,K_DOWN},{X,0,wh, '3',8,123,K_PGDN},{X,0,kW,kH*2+2, 28,6,124,KP_ENT},
{XN,Y6,kW*2,kH,'_',6,158,K_INS},{-kW*2,0,wh,'.',6,143,K_DEL}, //Ins Del
};
//symbols:  2. 3,  7 cir  9 dot` 28 ent   // todo .. enum, array
const uint8_t gGui=93/*pause*/, gMslow=100, gLoad=73,gSave=149,gDiv=2, gEsc=157,gAdd=84,gEnt=16,gEnt2=124,
	gRight=103,gLeft=101,gDown=122,gUp=82, gPgDn=123,gPgUp=83,gEnd=121,gHome=81,
	gCtrl=53,gSh=100,gMul=3,gSub=23, gIns=158,gDel=143, gBckSp=116,gSpc=159, gC=9,gV=8,gX=10,
	gF1=50,gF2=49,gF3=109,gF6=146,gF7=105,gF8=45,gF9=56,gF10=76,gF11=156,gF12=36;  //--


#elif defined(CK9)   //  CK9/6/3  ------------------------------------------------
#define  wf  fW,fH
#define  wh  kW,kH
const char* CKname = "CK9 18x8";  const int16_t XR = 235, XN = 256;  // pos
const int8_t  kW = 16, kH = 18, /* size */ fW = 14, fH = 12, F = -fW, X = -kW;
const uint8_t Y1=32,YY=18, Y2=Y1+YY, Y3=Y2+YY, Y4=Y3+YY, Y5=Y4+YY, Y6=Y5+YY;

// extra: 4x4
#if 0  // mini
#define  wm  7,7
const int8_t  M = -6, M2 = -9;
const uint8_t Y0a=Y1-2,Y0b=Y0a+6;
const DrawKey drawKeys[nDrawKeys] = {  //  Layout draw  ::::
{XN,Y0a,wm, '.',9, 47,K_F13},{M,0,wm, '.',9, 65,K_F14},{M2,0,wm, '.',9,137,K_F15}, {M,0,wm, '.',9,119,K_F16},
  {M2,0,wm, '.',9,120,K_F17},{M,0,wm, '.',9, 84,K_F18},{M2,0,wm, '.',9, 83,K_F19}, {M,0,wm, '.',9,101,K_F20},
{XN,Y0b,wm, '.',9, 64,K_F21},{M,0,wm, '.',9, 66,K_F22},{M2,0,wm, '.',9,138,K_F23}, {M,0,wm, '.',9,136,K_F24},
  {M2,0,wm, '.',9,118,K_F13},{M,0,wm, '.',9, 82,K_F14},{M2,0,wm, '.',9,100,K_F15}, {M,0,wm, '.',9,102,K_F16},
#else  // big
const int16_t XE = 204;  // pos
const uint8_t Y0a=4,Y0b=Y0a+11;  const int8_t M = -13, M2 = M-3;
const DrawKey drawKeys[nDrawKeys] = {  //  Layout draw  ::::
{XE,Y0a,wf, '1',9, 47,K_F13},{M,0,wf, '2',9, 65,K_F14},{M2,0,wf, '3',9,137,K_F15}, {M,0,wf, '4',9,119,K_F16},
  {M2,0,wf, '4',9,120,K_F17},{M,0,wf, '6',9, 84,K_F18},{M2,0,wf, '7',9, 83,K_F19}, {M,0,wf, '8',9,101,K_F20},
{XE,Y0b,wf, '1',9, 64,K_F21},{M,0,wf, '2',9, 66,K_F22},{M2,0,wf, '3',9,138,K_F23}, {M,0,wf, '4',9,136,K_F24},
  {M2,0,wf, '4',9,118,K_F13},{M,0,wf, '6',9, 82,K_F14},{M2,0,wf, '7',9,100,K_F15}, {M,0,wf, '8',9,102,K_F16},
#endif

{ 0,Y1,wf, 'e',6, 72,K_ESC},  //Esc 15x
{-32,0,wf, '1',2, 56,K_F1},{F, 0,wf, '2',2, 20,K_F2}, {F,0,wf, '3',2, 74,K_F3}, {F,0,wf, '4',2, 44,K_F4},
{-30,0,wf, '5',2, 31,K_F5},{F, 0,wf, '6',2, 67,K_F6}, {F,0,wf, '7',2, 61,K_F7}, {F,0,wf, '8',2, 43,K_F8},
{-28,0,wf, '9',2, 97,K_F9},{F, 0,wf, '0',2, 45,K_F10},{F,0,wf, '1',2,135,K_F11},{F,0,wf, '2',2,134,K_F12},
 {XR+1,0,wf, '.',6, 9,0},  //Displ

{ 0,Y2,16,kH, '`',3, 54,K_TILDE}, //~
{-16,0,wh, '1',1, 36,K_1},{X, 0,wh, '2',1, 37,K_2},{X,0,wh, '3',1, 38,K_3},{X,0,wh, '4',1, 52,K_4},
{ X, 0,wh, '5',1, 70,K_5},{X, 0,wh, '6',1, 69,K_6},{X,0,wh, '7',1, 51,K_7},{X,0,wh, '8',1, 50,K_8},
{ X, 0,wh, '9',1, 49,K_9},{X, 0,wh, '0',1, 42,K_0},
{ X, 0,wh, '-',3, 60,K_MINUS},{X, 0,wh, '=',3, 68,K_EQUAL},{X,0,24,kH, '<',7, 25,K_BACK}, //Back 15x
 {XR,0,wh, 'e',6, 57,K_ESC},

{ 0,Y3,24,kH, 29 ,6, 18,K_TAB}, //Tab
{-24,0,wh, 'Q',0,126,K_Q},{X, 0,wh, 'W',0,127,K_W},{X,0,wh, 'E',0,128,K_E},{X,0,wh, 'R',0,142,K_R},
{ X, 0,wh, 'T',0, 34,K_T},{X, 0,wh, 'Y',0, 33,K_Y},{X,0,wh, 'U',0,141,K_U},{X,0,wh, 'I',0,140,K_I},
{ X, 0,wh, 'O',0,139,K_O},{X, 0,wh, 'P',0,132,K_P},{X,0,wh, '[',3, 24,K_LBRACE},
{ X, 0,wh, ']',3, 32,K_RBRACE},{X,0,16,kH,'\\',3,  7,K_BSLASH}, // \| 15x
 {XR,0,wh, '<',7, 58,K_BACK},

{ 0,Y4,30,kH,  2 ,6, 19,K_Layer1}, //Caps
{-30,0,wh, 'A',0,  0,K_A},{X, 0,wh, 'S',0,  1,K_S},{X,0,wh, 'D',0,  2,K_D},{X,0,wh, 'F',0, 16,K_F},
{ X, 0,wh, 'G',0, 88,K_G},{X, 0,wh, 'H',0, 87,K_H},{X,0,wh, 'J',0, 15,K_J},{X,0,wh, 'K',0, 14,K_K},
{ X, 0,wh, 'L',0, 13,K_L},{X, 0,wh, ';',3,  6,K_SEMIC},
{ X, 0,wh,'\'',3, 78,K_QUOTE},{X, 0,26,kH, 28,7,115,K_ENT}, //Ent 14x
 {XR,0,wh, 28,7, 40,K_ENT},

{ 0,Y5,36,kH, 's',4, 28,K_LSHIFT}, //L Sh
{-36,0,wh, 'Z',0,108,K_Z},{X, 0,wh, 'X',0,109,K_X},{X,0,wh, 'C',0,110,K_C},{X,0,wh, 'V',0,124,K_V},
{ X, 0,wh, 'B',0,106,K_B},{X, 0,wh, 'N',0,105,K_N},{X,0,wh, 'M',0,123,K_M},{X,0,wh, ',',3,122,K_COMMA},
{ X, 0,wh, '.',3,121,K_PERIOD},{X,0,wh, '/',3, 96,K_SLASH},
{ X, 0,20,kH, 's',5, 10,K_RSHIFT}, {-20,0,16,kH, '^',5, 75,K_F15}, //R Sh 14x
 {XR,0,wh, 's',7, 39,K_RSHIFT}, // 12 up  120 end

// modif:  L Ctrl, L Gui, L Alt, space, R Alt, App,K_Ss, R Gui, R Ctrl
{ 0,Y6,22,kH, 'c',4, 62,K_LCTRL},{-22,0,14,kH,  9,4, 29,K_LGUI},{-14,0,14,kH,  '\\',4, 73,K_F22},
{-14,0,22,kH, 'a',4, 81,K_LALT}, {-22,0,68,kH,' ',6, 89,K_SPACE}, //Space
{-68,0,18,kH, 'a',5, 99,K_RALT}, // 23 menu 136 ins 135 del  30 ri 27 dn  28 le
{-18,0,16,kH,  9 ,6,103,K_RGUI}, {-16,0,16,kH,254,5, 59,K_MENU},{-16,0,14,kH,'c',5, 71,K_RCTRL}, //R Ct 12x
{-14,0,14,kH, '>',5, 93,K_F13},  {-14,0,14,kH,'v',5,107,K_F14},
 {XR,0,wh,'c',7, 95,K_RCTRL},

// numpad: 17x
{XN,Y2,wh,  3 ,6,125,K_NUML},{X, 0,wh, '/',3,113,KP_DIV},{X,0,wh, 'x',3,112,KP_MUL},{X,0,kW-2,kH,  '-',3, 94,KP_SUB},
{XN,Y3,wh, '7',8,143,K_HOME},{X, 0,wh, 24 ,8,131,K_UP},  {X,0,wh, '9',8,130,K_PGUP},{X,0,kW-2,kH*2,'+',3,129,KP_ADD},
{XN,Y4,wh, 27 ,8, 35,K_LEFT},{X, 0,wh,  7 ,6, 23,K_DEL}, {X,0,wh, 26 ,8, 22,K_RIGHT},
{XN,Y5,wh, '1',8, 17,K_END}, {X, 0,wh, 25 ,8,  5,K_DOWN},{X,0,wh, '3',8,  4,K_PGDN},{X,0,kW-2,kH*2, '|',3,  3,KP_ENT},
{XN,Y6,kW*2,kH,'_',6, 77,K_INS},{-kW*2,0,wh,'.',6, 76,K_DEL}, //Ins Del
};
const uint8_t gGui=9, gMslow=28, gLoad=31,gSave=44,gDiv=113, gEsc=72,gAdd=129,gEnt=115/*40*/,gEnt2=3,
	gRight=22,gLeft=35,gDown=5,gUp=131, gPgUp=130,gPgDn=4,gEnd=17,gHome=143,
	gCtrl=62,gSh=28,gMul=112,gSub=94, gIns=77,gDel=76, gBckSp=25,gSpc=89, gC=110,gV=124,gX=109,
	gF1=56,gF2=20,gF3=74,gF6=67,gF7=61,gF8=43,gF9=97,gF10=45,gF11=135,gF12=134;

#endif
#undef wf
#undef wh
#undef wm
