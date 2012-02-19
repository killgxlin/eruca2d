#ifndef __PAINTER_H__
#define __PAINTER_H__

#define XUseGL

#ifndef XUseGL
class Painter
{
public:
	VOID			WinToWorld(Vector2F* pPt);
	VOID			SetCenter(const Vector2F &vPos);
	Square			GetOtherScreenBox() const;
	Square			GetScreenBox() const;

	BOOL			Init(INT w, INT h, const char* title);
	VOID			Destroy();
	VOID			Update(DWORD dwDt);

	VOID			BeginDraw();
	VOID			EndDraw();
	VOID			Clear();

	VOID			WorldDrawLine(const Vector2F &vWorldPosHead, const Vector2F &vWorldPosTail, DWORD dwColor);
	VOID			WorldDrawImg(const Vector2F &vWorldPos, SDL_Surface* pSurface);
	VOID			WinDrawText(const Vector2F &vWinPos, SDL_Surface* pSurface);




private:
	FLOAT			GetZoomRate() const						{ return m_fZoomRate; }
	FLOAT			GetDrawPerSec()	const					{ return m_fDrawPerSec; }
	VOID			WorldDrawText(const Vector2F &vWorldPos, UINT32 uColor, const char* szFormat, ...);
	VOID			SetZoomRate(FLOAT fRate)				{ m_fZoomRate = Cut(fRate, 0.5f, 2.0f); }
	VOID			ModZoomRate(FLOAT fDelta)				{ m_fZoomRate = Cut(m_fZoomRate+fDelta, 0.5f,2.0f); }
	UINT32			GetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
	Vector2F		m_vCenter;
	Vector2F		m_vOtherCenter;

	VOID			WorldToScreen(const Vector2F &vCenter, Vector2F* pPt, Vector2F* pSize);
	VOID			ScreenToSDL(Vector2F* pPt);
	VOID			DrawRect(const Vector2F &vPos, const Vector2F &sSize, UINT32 uColor);

	FLOAT			m_fDrawPerSec;
	DWORD			m_dwDt;
	DWORD			m_dwDrawTimes;
	FLOAT			m_fZoomRate;
	Square			m_screenBox;

	SDL_Surface*	m_pScreen;
};

#else
#include <gl/glew.h>
class Painter
{
public:
	VOID			WinToWorld(Vector2F* pPt);
	VOID			SetCenter(const Vector2F &vPos);
	Square			GetOtherScreenBox() const			{ return m_sOtherScreenBox; }
	Square			GetScreenBox() const				{ return m_sScreenBox; }

	BOOL			Init(INT w, INT h, const char* title);
	VOID			Destroy();
	VOID			Update(DWORD dwDt);

	VOID			BeginDraw();
	VOID			EndDraw();
	VOID			Clear();

	VOID			WorldDrawLine(const Vector2F &vWorldPosHead, const Vector2F &vWorldPosTail, DWORD dwColor);
	VOID			WorldDrawImg(const Vector2F &vWorldPos, SDL_Surface* pSurface);
	VOID			WinDrawText(const Vector2F &vWinPos, char* szStr);

private:
	typedef map<SDL_Surface*, GLuint>	TexMap;
	TexMap			m_mapTex;

	GLuint			GetTex(SDL_Surface* pSurface, BOOL bNeedRet);
	VOID			ReturnTex(GLuint tex);
	Vector2F		m_vCenter;
	Vector2F		m_vOtherCenter;
	Vector2F		m_vSize;
	Square			m_sScreenBox;
	Square			m_sOtherScreenBox;

};


#endif

extern Painter g_painter;



#endif