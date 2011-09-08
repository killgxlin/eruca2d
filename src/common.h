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

#define XMaxPlayerSpeedX	1000	//pixel per sec
#define XGravity			-2000	//pixel * pixel per sec
#define XCtrlAcc			500	//
#define XJumpSpeed			50

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
	T		Length2() const		{ return x*x + y*y; }
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
	BOOL IntersectRegion(const AABBox &other, AABBox &result)
	{
		result.vMin.x = Max(this->vMin.x, other.vMin.x);
		result.vMax.x = Min(this->vMax.x, other.vMax.x);
		result.vMin.y = Max(this->vMin.y, other.vMin.y);
		result.vMax.y = Min(this->vMax.y, other.vMax.y);

		if( result.vMin.x > result.vMax.x || result.vMin.y > result.vMax.y )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	DWORD IntersectTest(const AABBox &other, FLOAT &fDeep, DWORD dwDirFlag)
	{
		AABBox intersect;
		if( !IntersectRegion(other, intersect) )
		{
			return ECD_None;
		}

		DWORD dwResult = ECD_None;

		Vector2F vCenter = (this->vMin + this->vMax) / 2;
		if( intersect.vMax.x - intersect.vMin.x >= intersect.vMax.y - intersect.vMin.y )
		{
			if( vCenter.y > intersect.vMax.y )
			{
				dwResult |= ECD_Down;
			}
			else if( vCenter.y < intersect.vMin.y )
			{
				dwResult |= ECD_Top;
			}
			else
			{
				assert(0);
			}
			fDeep = intersect.vMax.y - intersect.vMin.y;
		}
		else
		{
			if( vCenter.x > intersect.vMax.x )
			{
				dwResult |= ECD_Left;
			}
			else if( vCenter.x < intersect.vMin.x )
			{
				dwResult |= ECD_Right;
			}
			fDeep = intersect.vMax.x - intersect.vMin.x;
		}

		return dwResult;
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

	AABBox(const Vector2F &vCenter, const Vector2F &vSize)
	{
		Vector2F vHalf = vSize/2;
		vMin = vCenter - vHalf;
		vMax = vCenter + vHalf;
	}

	BOOL	IntersectPoint(const Vector2F &vPoint)
	{
		return IsBetweenClose(vPoint.x, vMin.x, vMax.x) || IsBetweenClose(vPoint.y, vMin.y, vMax.y);
	}

	BOOL	IntersectBox(const AABBox &other)
	{
		return IntersectPoint(other.vMin) || IntersectPoint(other.vMax);
	}

	VOID	AddPoint(const Vector2F &vPoint)
	{
		if( vPoint.x < vMin.x )
		{
			vMin.x = vPoint.x;
		}
		if( vPoint.x > vMax.x )
		{
			vMax.x = vPoint.x;
		}
		if( vPoint.y < vMin.y )
		{
			vMin.y = vPoint.y;
		}
		if( vPoint.y > vMax.y )
		{
			vMax.y = vPoint.y;
		}
	}

	VOID	AddBox(const AABBox &vBox)
	{
		AddPoint(vBox.vMin);
		AddPoint(vBox.vMax);
	}
	AABBox():vMax(-9999999.0f, -9999999.0f),vMin(9999999.0f, 9999999.0f){}
	Vector2F		vMin, vMax;
};

#include "keyboard.h"
#include "text.h"
#include "painter.h"
#include "timer.h"

#endif