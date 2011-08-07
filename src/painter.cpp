#include "common.h"
#include "painter.h"

Painter g_painter;

BOOL Painter::Init( int w, int h, const char* title )
{
	m_pScreen = SDL_SetVideoMode(w, h, 8, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if( NULL == m_pScreen ) return FALSE;

	if( title != NULL )
		SDL_WM_SetCaption(title, 0);
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
	SDL_FillRect(m_pScreen, 0, SDL_MapRGB(m_pScreen->format, 0, 0, 0));
}

VOID Painter::DrawRect( const Vector2 &vPos, const Size &sSize, UINT32 uColor )
{
	SDL_Rect rect;
	rect.w = sSize.nW;
	rect.h = sSize.nH;
	rect.x = vPos.nX - sSize.nW / 2;
	rect.y = vPos.nY - sSize.nH / 2;

	SDL_FillRect(m_pScreen, &rect, uColor);
}
