#pragma once
#include <stdint.h>
#include "def.h"

//  ----- Enums -----
typedef enum Type {  //  Type of pin
	Type_StrobeOn, Type_StrobeOff, Type_StrobeSetup,
	Type_Sense, Type_SenseSetup,
} Type;

//  Sense/Strobe config
typedef enum Config {
	Config_Pullup,  Config_Pulldown,  Config_Opendrain,
} Config;

//  Keypress States
typedef enum KeyPosition {
	KeyState_Off = 0, KeyState_Press = 1,
	KeyState_Hold = 2, KeyState_Release = 3, KeyState_Invalid,
} KeyPosition;


//  ----***  Matrix configs  ***----

#if defined(CK8)   //  20 x 8  CK8
	const static uint8_t
	Matrix_cols[] = { 4, 5, 6, 7, 8, 17, 16, 15, 14, // x9
		24, 25, 28, 32, 30, 33, 31, 29, 27, 38, 39}, // x11 20
	Matrix_rows[] = { 0, 1, 2, 3, 34, 35, 36, 37 };

#elif defined(CK1)   //  8 x 8  CK1
	const static uint8_t
	Matrix_cols[] = { 32, 30, 33, 31, 29, 27, 38, 39}, // x11 20
	Matrix_rows[] = { 0, 1, 2, 3, 34, 35, 36, 37 };

//#elif defined(CK6) || defined(CK7)   //  18 x 8  CK6 or CK7
	//
#else
	#error "No keyboard defined! Put e.g. #define CK1 in def.h"
#endif

//  Type of scan matrix
#define /*Config*/ Matrix_type Config_Pullup

//  Enable anti-ghosting code
//  input rows "high",  columns output "high-z", one selected "low"
#define GHOSTING_MATRIX

//  ----***  End of config


#define NumCols  sizeof( Matrix_cols ) / sizeof( Matrix_cols[0] )
#define NumRows  sizeof( Matrix_rows ) / sizeof( Matrix_rows[0] )
#define ScanKeys  (NumCols * NumRows)  // sizeof( Matrix_scanArray ) / sizeof( KeyState )


#define DebounceDivThreshold_define 65535
#define DebounceCounter uint16_t

#define DebounceThrottleDiv_define 0


//  Debounce Element
typedef struct KeyState {
	DebounceCounter activeCount;
	DebounceCounter inactiveCount;
	KeyPosition     prevState;
	KeyPosition     curState;
	uint32_t        prevDecisionTime;
	KeyPosition  state;  // final after ghost
	int16_t  autoRepeat;  // auto repeat, press time, for gui
	int8_t       layerOn;  // layer on which key was pressed
} KeyState;

#ifdef GHOSTING_MATRIX
//  Ghost Element, after ghost detection/cancelation
typedef struct KeyGhost {
	KeyPosition     prev;
	KeyPosition     cur;
	KeyPosition     saved;  // state before ghosting
} KeyGhost;


//  -----  utility  -----

#define Key(x)   (Matrix_scanArray[x].state == KeyState_Press ? 1 : 0)
#define KeyH(x)  (Matrix_scanArray[x].state == KeyState_Hold ? 1 : 0)

inline uint8_t keyOn(/*KeyPosition*/uint8_t st)
{	return (st == KeyState_Press || st == KeyState_Hold) ? 1 : 0;  }

//  Ghost status info
extern int ghost_cols, ghost_rows;  // ghosting if any > 0
extern uint8_t col_use[NumCols], row_use[NumRows];  // used count
extern uint8_t col_ghost[NumCols], row_ghost[NumRows];  // marked as having ghost if 1
#endif

extern int cnt_press, cnt_hold, cnt_rel;  // testing and stats
extern int16_t cnt_press1min;


//  ----- Functions -----
#ifdef	__cplusplus
extern "C"
{
#endif

extern void Matrix_setup();
extern void Matrix_scan( uint16_t scanNum );

extern KeyState Matrix_scanArray[ScanKeys];

#ifdef	__cplusplus
}
#endif