#include "common.h"
#include "painter.h"

Painter g_painter;

BOOL Painter::Init( int w, int h, const char* title )
{
	m_pScreen = SDL_SetVideoMode(w, h, 8, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if( NULL == m_pScreen ) return FALSE;

	if( title != NULL )
		SDL_WM_SetCaption(title, 0);

	SetCenter(Vector2(320, 240));
	return true;
}

VOID Painter::Destroy()
{
	if( NULL != m_pScreen )
		SDL_FreeSurface( m_pScreen );
	m_pScreen = NULL;
}

VOID Painter::Flush()
{
	SDL_Flip(m_pScreen);
}

VOID Painter::Clear()
{
	SDL_FillRect(m_pScreen, 0, GetColor(0, 0, 0));
}

VOID Painter::DrawRect( const Vector2 &vPos, const Size &sSize, UINT32 uColor )
{
	SDL_Rect rect;
	rect.w = sSize.w;
	rect.h = sSize.h;
	rect.x = vPos.x - sSize.w / 2;
	rect.y = vPos.y - sSize.h / 2;

	SDL_FillRect(m_pScreen, &rect, uColor);
}

Uint32 Painter::GetColor( Uint8 u8R, Uint8 u8G, Uint8 u8B )
{
	return SDL_MapRGB(m_pScreen->format, u8R, u8G, u8B);
}

VOID Painter::WorldDrawRect( const Vector2 &vWorldPos, const Size &sSize, UINT32 uColor )
{
	Vector2 vPos = vWorldPos;
	WorldToScreen(&vPos);
	ScreenToSDL(&vPos);

	DrawRect(vPos, sSize, uColor);
}

VOID Painter::WorldToScreen( Vector2* pPt )
{
	Vector2 vSize(m_pScreen->w, m_pScreen->h);
	Vector2 vLeftBottom = m_vCenter - vSize / 2;
	*pPt -= vLeftBottom;
}

VOID Painter::ScreenToSDL( Vector2* pPt )
{
	pPt->y = m_pScreen->h - pPt->y;
}
