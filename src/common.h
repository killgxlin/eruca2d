#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>

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
using std::swap;
using std::make_pair;
using std::for_each;

enum ECollidePriority
{
	ECP_Dynamic	= 1,
	ECP_Static	= 2,
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

#define XMaxPlayerSpeed		1000	//pixel per sec
#define XGravity			-10	//pixel * pixel per sec
#define XCtrlAcc			15	//

#define XScreenW			640
#define XScreenH			(XScreenW * 3 / 4)

#define XPlayerSize			(XScreenW / 40)
#define XTileSize			XPlayerSize

template<typename T = FLOAT>
class Size
{
public:
	Size(void) {}
	Size(T width, T height) : w(width), h(height) {}

	BOOL operator==(const Size& other) const;
	BOOL operator!=(const Size& other) const;

	T w, h;
};

typedef Size<FLOAT>		SizeF;
typedef Size<INT>		SizeN;
typedef Size<UINT32>	SizeU;

template<typename T = FLOAT>
class Vector2
{
public:
	Vector2(void) {}
	Vector2(T x_, T y_) : x(x_), y(y_) {}
	template<typename TT>
	Vector2(TT x_, TT y_) : x(T(x_)), y(T(y_)){}

	template<typename TT>
	Vector2<T>& operator+=(const Vector2<TT>& vec)
	{
		x += vec.x;
		y += vec.y;

		return *this;
	}

	template<typename TT>
	Vector2<T>& operator-=(const Vector2<TT>& vec)
	{
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

	Vector2<T>& operator/=(const FLOAT factor)
	{
		if( factor != 0.0f )
		{
			x /= factor;
			y /= factor;
		}
		return *this;
	}

	Vector2<T>& operator*=(const FLOAT factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	template<typename TT>
	Vector2<T>	operator+(const Vector2<TT>& vec) const
	{
		return Vector2<T>(x + vec.x, y + vec.y);
	}

	template<typename TT>
	Vector2<T>	operator-(const Vector2<TT>& vec) const
	{
		return Vector2<T>(x - vec.x, y - vec.y);
	}

	template<typename TT>
	Vector2<T>	operator*(const Vector2<TT>& vec) const
	{
		return Vector2<T>(x * vec.x, y * vec.y);
	}

	Vector2<T>	operator*(const FLOAT factor) const
	{
		return Vector2<T>(x * factor, y * factor);
	}

	Vector2<T>	operator/(FLOAT factor) const
	{
		if( factor == 0.0f ) factor = 1.0f;

		return Vector2<T>(T(x / factor),T( y / factor));
	}

	template<typename TT>
	BOOL	operator==(const Vector2<TT>& vec) const
	{
		return ((x == vec.x) && (y == vec.y));
	}

	template<typename TT>
	BOOL	operator!=(const Vector2<TT>& vec) const
	{
		return !(operator==(vec));
	}

	Size<T>	asSize() const		{ return Size(x, y); }
	T		Length()const		{ return sqrt(x*x+y*y); }
	T		x, y;
};

typedef Vector2<FLOAT>	Vector2F;
typedef Vector2<INT>	Vector2N;
typedef Vector2<UINT32>	Vector2U;


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
	UINT32 IntersectTest(const AABBox &other, FLOAT &fDeep, DWORD dwColDirFlag)
	{
		UINT32 uFlag = ECD_None;
		fDeep = 99;

		if( IsBetweenClose(other.vMin.x, this->vMin.x, this->vMax.x) || IsBetweenClose(other.vMax.x, this->vMin.x, this->vMax.x) )
		{
			// top
			if( ECD_Top & dwColDirFlag && IsBetweenClose(other.vMin.y, this->vMin.y, this->vMax.y) && other.vMax.y > this->vMax.y )
			{
				FLOAT fCurDeep = this->vMax.y - other.vMin.y;
				if( fDeep > fCurDeep )
				{
					fDeep = fCurDeep;
					uFlag = ECD_Top;
				}				
			}
			// down
			if( ECD_Down & dwColDirFlag && IsBetweenClose(other.vMax.y, this->vMin.y, this->vMax.y) && other.vMin.y < this->vMin.y)
			{
				FLOAT fCurDeep = other.vMax.y - this->vMin.y;
				if( fDeep > fCurDeep )
				{
					fDeep = fCurDeep;
					uFlag = ECD_Down;
				}
			}
		}
		if( IsBetweenClose(other.vMin.y, this->vMin.y, this->vMax.y) || IsBetweenClose(other.vMax.y, this->vMin.y, this->vMax.y) )
		{
			// left
			if( ECD_Left & dwColDirFlag && IsBetweenClose(other.vMax.x, this->vMin.x, this->vMax.x) && other.vMin.x < this->vMin.x)
			{
				FLOAT fCurDeep = other.vMax.x - this->vMin.x;
				if( fDeep > fCurDeep )
				{
					fDeep = fCurDeep;
					uFlag = ECD_Left;
				}
			}
			// right
			if( ECD_Right & dwColDirFlag && IsBetweenClose(other.vMin.x, this->vMin.x, this->vMax.x) && other.vMax.x > this->vMax.x )
			{
				FLOAT fCurDeep = this->vMax.x - other.vMin.x;
				if( fDeep > fCurDeep )
				{
					fDeep = fCurDeep;
					uFlag = ECD_Right;
				}
			}
		}

		return uFlag;
	}
	float IntersectMovingAABB( const AABBox &movingBox, const Vector2F &d, DWORD &dwDirFlag ) 
	{
			const float kNoIntersection = 1e30f;

			dwDirFlag = ECD_None;

			float	tEnter = 0.0f;
			float	tLeave = 1.0f;

			float	xEnter = 0.0f;
			float	xLeave = 1.0f;
			float	yEnter = 0.0f;
			float	yLeave = 1.0f;

			bool	bTop	= false;
			bool	bRight	= false;

			if (d.x == 0.0f) 
			{
				if ( (this->vMin.x >= movingBox.vMax.x) || (this->vMax.x <= movingBox.vMin.x) ) 
					return kNoIntersection;
			} 
			else 
			{
				float	oneOverD = 1.0f / d.x;

				xEnter = (this->vMin.x - movingBox.vMax.x) * oneOverD;
				xLeave = (this->vMax.x - movingBox.vMin.x) * oneOverD;

				if (xEnter > xLeave) 
				{
					bRight = true;
					swap(xEnter, xLeave);
				}

				if (xEnter > tEnter) tEnter = xEnter;
				if (xLeave < tLeave) tLeave = xLeave;

				if (tEnter > tLeave) return kNoIntersection;
			}

			if (d.y == 0.0f) 
			{
				if ( (this->vMin.y >= movingBox.vMax.y) || (this->vMax.y <= movingBox.vMin.y) ) 
					return kNoIntersection;
			} 
			else 
			{
				float	oneOverD = 1.0f / d.y;

				yEnter = (this->vMin.y - movingBox.vMax.y) * oneOverD;
				yLeave = (this->vMax.y - movingBox.vMin.y) * oneOverD;

				if (yEnter > yLeave) 
				{
					bTop = true;
					swap(yEnter, yLeave);
				}

				if (yEnter > tEnter) tEnter = yEnter;
				if (yLeave < tLeave) tLeave = yLeave;

				if (tEnter > tLeave) return kNoIntersection;
			}

			if( xEnter > yEnter )
			{
				dwDirFlag = bRight ? ECD_Right : ECD_Left;
			}
			else
			{
				dwDirFlag = bTop ? ECD_Top : ECD_Down;
			}

			return tEnter;
	}

	Vector2F		vMin, vMax;
};

#include "keyboard.h"
#include "text.h"
#include "painter.h"

#endif