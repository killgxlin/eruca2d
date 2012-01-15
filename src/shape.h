#ifndef __SHAPE_H__
#define __SHAPE_H__


template<typename T = FLOAT>
class Size
{
public:
	Size(VOID) {}
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
	Vector2(VOID) {}
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

class Segment;
class Square;

class Shape
{
public:
	DWORD IntersectTest(const Square* pOther, FLOAT &fDeep, DWORD dwDirFlag);
	DWORD IntersectTest(const Segment* pOther, FLOAT &fDeep, DWORD dwDirFlag);
};

class Segment : public Shape
{
public:
	DWORD IntersectTest(const Square* pOther, FLOAT &fDeep, DWORD dwDirFlag);
	DWORD IntersectTest(const Segment* pOther, FLOAT &fDeep, DWORD dwDirFlag);
};
// 
// class Square : public Shape
// {
// public:
// 	virtual DWORD IntersectTest(const Square &other, FLOAT &fDeep, DWORD dwDirFlag);
// 	virtual DWORD IntersectTest(const Segment &other, FLOAT &fDeep, DWORD dwDirFlag);
// };

class Square : public Shape
{
public:
	DWORD IntersectTest(const Segment* pOther, FLOAT &fDeep, DWORD dwDirFlag);
	DWORD IntersectTest(const Square* pOther, FLOAT &fDeep, DWORD dwDirFlag)
	{
		if( dwDirFlag == ECD_None )
		{
			return ECD_None;
		}

		Square intersect;
		if( !IntersectRegion(pOther, &intersect) )
		{
			return ECD_None;
		}

		DWORD dwResult = ECD_None;

		Vector2F vCenter = (this->vMin + this->vMax) / 2;
		FLOAT fW = intersect.vMax.x - intersect.vMin.x;
		FLOAT fH = intersect.vMax.y - intersect.vMin.y;
		if( fW >= fH )
		{
			if( dwDirFlag & ECD_Down && vCenter.y > intersect.vMax.y )
			{
				dwResult |= ECD_Down;
			}
			else if( dwDirFlag & ECD_Top && vCenter.y < intersect.vMin.y )
			{
				dwResult |= ECD_Top;
			}

			fDeep = intersect.vMax.y - intersect.vMin.y;
		}
		else if( fW <= fH )
		{
			if( dwDirFlag & ECD_Left && vCenter.x > intersect.vMax.x )
			{
				dwResult |= ECD_Left;
			}
			else if( dwDirFlag & ECD_Right && vCenter.x < intersect.vMin.x )
			{
				dwResult |= ECD_Right;
			}
			fDeep = intersect.vMax.x - intersect.vMin.x;
		}

		return dwResult;
	}

	Square(const Vector2F &vCenter, const Vector2F &vSize)
	{
		Vector2F vHalf = vSize/2;
		vMin = vCenter - vHalf;
		vMax = vCenter + vHalf;
	}


	BOOL	IntersectBox(const Square &other)
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

	VOID	AddBox(const Square &vBox)
	{
		AddPoint(vBox.vMin);
		AddPoint(vBox.vMax);
	}
	Square():vMax(-9999999.0f, -9999999.0f),vMin(9999999.0f, 9999999.0f){}
	Vector2F		vMin, vMax;
private:
	BOOL IntersectRegion(const Square* pOther, Square* pResult)
	{
		pResult->vMin.x = Max(this->vMin.x, pOther->vMin.x);
		pResult->vMax.x = Min(this->vMax.x, pOther->vMax.x);
		pResult->vMin.y = Max(this->vMin.y, pOther->vMin.y);
		pResult->vMax.y = Min(this->vMax.y, pOther->vMax.y);

		if( pResult->vMin.x >= pResult->vMax.x || pResult->vMin.y >= pResult->vMax.y )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	BOOL	IntersectPoint(const Vector2F &vPoint)
	{
		return IsBetweenClose(vPoint.x, vMin.x, vMax.x) && IsBetweenClose(vPoint.y, vMin.y, vMax.y);
	}
};

#endif