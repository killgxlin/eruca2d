#ifndef __PAINTER_H__
#define __PAINTER_H__

class Painter
{
public:
	BOOL			Init(INT w, INT h, const char* title);
	VOID			Destroy();
	VOID			Update(DWORD dwDt);

	VOID			Flush();
	VOID			Clear();
	VOID			WorldDrawRect(const Vector2F &vWorldPos, const Vector2F &vSize, UINT32 uColor);
	VOID			WorldDrawText(const Vector2F &vWorldPos, UINT32 uColor, const char* szFormat, ...);
	VOID			WorldDrawLine(const Vector2F &vWorldPosHead, const Vector2F &vWorldPosTail, DWORD dwColor);

	VOID			SetCenter(const Vector2F &vPos);
	VOID			SetZoomRate(FLOAT fRate)				{ m_fZoomRate = Cut(fRate, 0.5f, 2.0f); }

	VOID			ModZoomRate(FLOAT fDelta)				{ m_fZoomRate = Cut(m_fZoomRate+fDelta, 0.5f,2.0f); }

	UINT32			GetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
	FLOAT			GetZoomRate() const						{ return m_fZoomRate; }
	FLOAT			GetDrawPerSec()	const					{ return m_fDrawPerSec; }
	SDL_Surface*	GetScreen() const						{ return m_pScreen; }

	
	Vector2F		m_vCenter;
	Vector2F		m_vOtherCenter;
	Square			GetOtherScreenBox() const;
	Square			GetScreenBox() const;

	VOID			SDLToWorld(Vector2F* pPt);
private:
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

extern Painter g_painter;

#endif