#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include <map>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

using std::map;
using std::vector;
using std::multimap;
using std::list;
using std::pair;
using std::string;
using std::swap;
using std::make_pair;
using std::for_each;

enum EActDir
{
	EAD_Right	= 0,
	EAD_Left	= 1,
	EAD_End,
};

enum EActType
{
	EAT_Stand	= 0,
	EAT_Run		= 1,
	EAT_Jump	= 2,
	EAT_Fall	= 3,
	EAT_Land	= 4,
	EAT_End,
};

enum ECollideDir
{
	ECD_None	= 0,
	ECD_Top		= 1,
	ECD_Down	= 2,
	ECD_Left	= 4,
	ECD_Right	= 8,
	ECD_All		= ECD_Top | ECD_Down | ECD_Left | ECD_Right,
};

#define XMaxArrowSpeed		1600
#define XMaxPlayerSpeedX	200	//pixel per sec
#define XGravity			Vector2F(0, -400)	//pixel * pixel per sec
#define XCtrlAcc			800	//
#define XJumpSpeed			400

#define XScreenW			640
#define XScreenH			(XScreenW * 3 / 4)

#define XPlayerSize			(XScreenW / 40)
#define XTerrainSize		XPlayerSize
#define XArrowSize			(XPlayerSize/2)
#define XAnimalSize			(XPlayerSize)

#define XTotalW				(XScreenW * 5)
#define XTotalH				(XScreenH * 3)

#define XTilesW				(XTotalW / XTerrainSize)
#define XTilesH				(XTotalH / XTerrainSize)


#include "shape.h"
#include "keyboard.h"
#include "text.h"
#include "painter.h"
#include "timer.h"

#endif