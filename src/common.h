#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <tchar.h>
#include <math.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include <map>
#include <list>
#include <algorithm>

using std::map;
using std::list;
using std::pair;
using std::make_pair;
using std::for_each;

enum ECollidePriority
{
	ECP_Player	= 1,
	ECP_Tile	= 2,
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

#define XMaxPlayerSpeed		400	//pixel per sec
#define XGravity			10	//pixel * pixel per sec
#define XCtrlAcc			15	//

class Size
{
public:
	Size(void) {}
	Size(INT width, INT height) : nW(width), nH(height) {}

	bool operator==(const Size& other) const;
	bool operator!=(const Size& other) const;

	INT nW, nH;
};
class Vector2
{
public:
	Vector2(void) {}
	Vector2(INT x, INT y) : nX(x), nY(y) {}

	Vector2& operator+=(const Vector2& vec)
	{
		nX += vec.nX;
		nY += vec.nY;

		return *this;
	}

	Vector2& operator-=(const Vector2& vec)
	{
		nX -= vec.nX;
		nY -= vec.nY;

		return *this;
	}

	Vector2	operator+(const Vector2& vec) const
	{
		return Vector2(nX + vec.nX, nY + vec.nY);
	}

	Vector2	operator-(const Vector2& vec) const
	{
		return Vector2(nX - vec.nX, nY - vec.nY);
	}

	Vector2	operator*(const Vector2& vec) const
	{
		return Vector2(nX * vec.nX, nY * vec.nY);
	}

	Vector2	operator*(const FLOAT factor) const
	{
		return Vector2(nX * factor, nY * factor);
	}

	Vector2	operator/(FLOAT factor) const
	{
		if( factor == 0.0f ) factor = 1.0f;

		return Vector2(nX / factor, nY / factor);
	}


	bool	operator==(const Vector2& vec) const
	{
		return ((nX == vec.nX) && (nY == vec.nY));
	}

	bool	operator!=(const Vector2& vec) const
	{
		return !(operator==(vec));
	}

	Size	asSize() const     { return Size(nX, nY); }
	INT		Length()const		{ return sqrt(float(nX*nX+nY*nY)); }
	INT nX, nY;
};


template<typename T>
T		Max(T val1, T val2)
{
	return val1 > val2 ? val1 : val2;
}
template<typename T>
T		Min(T val1, T val2)
{
	return val1 < val2 ? val1 : val2;
}

template<typename T>
BOOL	IsBetween(T val, T min, T max)
{
	return (val>=min && val<max);
}

template<typename T>
T		Cut(T val, T min, T max)
{
	val = Max(min, val);
	val = Min(max-1, val);

	return val;
}

class AABBox
{
public:
	UINT32		IntersectTest(const AABBox &other)
	{
		UINT32 uFlag = ECD_None;
		INT nDeep = 0;

		if( IsBetween(other.vMin.nX, this->vMin.nX, this->vMax.nX) || IsBetween(other.vMax.nX-1, this->vMin.nX, this->vMax.nX) )
		{
			// top
			if( IsBetween(other.vMax.nY-1, this->vMin.nY, this->vMax.nY) && other.vMin.nY < this->vMin.nY)
			{
				if( nDeep < this->vMin.nY - other.vMin.nY )
				{
					nDeep = this->vMin.nY - other.vMin.nY;
					uFlag = ECD_Top;
				}
			}
			// down
			if( IsBetween(other.vMin.nY, this->vMin.nY, this->vMax.nY) && other.vMax.nY > this->vMax.nY )
			{
				if( nDeep < other.vMax.nY - this->vMax.nY )
				{
					nDeep = other.vMax.nY - this->vMax.nY;
					uFlag = ECD_Down;
				}				
			}
		}
		if( IsBetween(other.vMin.nY, this->vMin.nY, this->vMax.nY) || IsBetween(other.vMax.nY-1, this->vMin.nY, this->vMax.nY) )
		{
			// left
			if( IsBetween(other.vMax.nX-1, this->vMin.nX, this->vMax.nX) && other.vMin.nX < this->vMin.nX)
			{
				if( nDeep < this->vMin.nX - other.vMin.nX )
				{
					nDeep = this->vMin.nX - other.vMin.nX;
					uFlag = ECD_Left;
				}
			}
			// right
			if( IsBetween(other.vMin.nX, this->vMin.nX, this->vMax.nX) && other.vMax.nX > this->vMax.nX )
			{
				if( nDeep < other.vMax.nX - this->vMax.nX )
				{
					nDeep = other.vMax.nX - this->vMax.nX;
					uFlag = ECD_Right;
				}
			}
		}

		return uFlag;
	}
	Vector2		vMin, vMax;
};

#endif