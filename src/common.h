#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <tchar.h>
#include <math.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include <map>
#include <list>
#include <string>
#include <algorithm>

using std::map;
using std::list;
using std::pair;
using std::string;
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
	Size(FLOAT width, FLOAT height) : w(width), h(height) {}

	bool operator==(const Size& other) const;
	bool operator!=(const Size& other) const;

	FLOAT w, h;
};
class Vector2
{
public:
	Vector2(void) {}
	Vector2(FLOAT x, FLOAT y) : x(x), y(y) {}

	Vector2& operator+=(const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;

		return *this;
	}

	Vector2& operator-=(const Vector2& vec)
	{
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

	Vector2	operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	Vector2	operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	Vector2	operator*(const Vector2& vec) const
	{
		return Vector2(x * vec.x, y * vec.y);
	}

	Vector2	operator*(const FLOAT factor) const
	{
		return Vector2(x * factor, y * factor);
	}

	Vector2	operator/(FLOAT factor) const
	{
		if( factor == 0.0f ) factor = 1.0f;

		return Vector2(x / factor, y / factor);
	}


	bool	operator==(const Vector2& vec) const
	{
		return ((x == vec.x) && (y == vec.y));
	}

	bool	operator!=(const Vector2& vec) const
	{
		return !(operator==(vec));
	}

	Size	asSize() const     { return Size(x, y); }
	FLOAT	Length()const		{ return sqrt(x*x+y*y); }
	FLOAT	x, y;
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
BOOL	IsBetweenClose(T val, T min, T max)
{
	return (val>=min && val<=max);
}

template<typename T>
T		Cut(T val, T min, T max)
{
	val = Max(min, val);
	val = Min(max, val);

	return val;
}

class AABBox
{
public:
	UINT32		IntersectTest(const AABBox &other)
	{
		UINT32 uFlag = ECD_None;
		FLOAT fDeep = 0;

		if( IsBetweenClose(other.vMin.x, this->vMin.x, this->vMax.x) || IsBetweenClose(other.vMax.x, this->vMin.x, this->vMax.x) )
		{
			// top
			if( IsBetweenClose(other.vMax.y, this->vMin.y, this->vMax.y) && other.vMin.y < this->vMin.y)
			{
				if( fDeep < this->vMin.y - other.vMin.y )
				{
					fDeep = this->vMin.y - other.vMin.y;
					uFlag = ECD_Top;
				}
			}
			// down
			if( IsBetweenClose(other.vMin.y, this->vMin.y, this->vMax.y) && other.vMax.y > this->vMax.y )
			{
				if( fDeep < other.vMax.y - this->vMax.y )
				{
					fDeep = other.vMax.y - this->vMax.y;
					uFlag = ECD_Down;
				}				
			}
		}
		if( IsBetweenClose(other.vMin.y, this->vMin.y, this->vMax.y) || IsBetweenClose(other.vMax.y, this->vMin.y, this->vMax.y) )
		{
			// left
			if( IsBetweenClose(other.vMax.x, this->vMin.x, this->vMax.x) && other.vMin.x < this->vMin.x)
			{
				if( fDeep < this->vMin.x - other.vMin.x )
				{
					fDeep = this->vMin.x - other.vMin.x;
					uFlag = ECD_Left;
				}
			}
			// right
			if( IsBetweenClose(other.vMin.x, this->vMin.x, this->vMax.x) && other.vMax.x > this->vMax.x )
			{
				if( fDeep < other.vMax.x - this->vMax.x )
				{
					fDeep = other.vMax.x - this->vMax.x;
					uFlag = ECD_Right;
				}
			}
		}

		return uFlag;
	}
	Vector2		vMin, vMax;
};

#endif