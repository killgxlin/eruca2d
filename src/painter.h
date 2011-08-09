#ifndef PAINTER_H
#define PAINTER_H

class Painter
{
public:
	BOOL	Init(int w, int h, const char* title);

	VOID	Destroy();

	VOID	Flush();

	VOID	Clear();

	VOID	DrawRect(const Vector2 &vPos, const Size &sSize, UINT32 uColor);

	Uint32	GetColor(Uint8 u8R, Uint8 u8G, Uint8 u8B);

	SDL_Surface*	GetScreen() { return m_pScreen; }

private:
	SDL_Surface*	m_pScreen;

public:
	VOID	SetCenter(const Vector2 &vPos)	{ m_vCenter = vPos; }
	VOID	WorldDrawRect(const Vector2 &vWorldPos, const Size &sSize, UINT32 uColor);
private:
	VOID	WorldToScreen(Vector2* pPt);
	VOID	ScreenToSDL(Vector2* pPt);
	Vector2	m_vCenter;

};

extern Painter g_painter;

#endif