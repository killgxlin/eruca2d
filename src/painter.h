#ifndef __PAINTER_H__
#define __PAINTER_H__

class Painter
{
public:
	BOOL			Init(INT w, INT h, const char* title);

	VOID			Destroy();

	VOID			Flush();

	VOID			Clear();

	VOID			DrawRect(const Vector2F &vPos, const SizeN &sSize, UINT32 uColor);

	UINT32			GetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);

	SDL_Surface*	GetScreen() { return m_pScreen; }

	VOID			Update(DWORD dwDt);
	FLOAT			GetDrawPerSec()	const	{ return m_fDrawPerSec; }

private:
	SDL_Surface*	m_pScreen;

public:
	VOID		SetCenter(const Vector2F &vPos)	{ m_vCenter = vPos; }
	VOID		WorldDrawRect(const Vector2F &vWorldPos, const SizeN &sSize, UINT32 uColor);
	VOID		WorldDrawText(const Vector2F &vWorldPos, UINT32 uColor, const char* szFormat, ...);
private:
	VOID		WorldToScreen(Vector2F* pPt);
	VOID		ScreenToSDL(Vector2F* pPt);
	Vector2F	m_vCenter;

	FLOAT		m_fDrawPerSec;
	DWORD		m_dwDt;
	DWORD		m_dwDrawTimes;
};

extern Painter g_painter;

#endif