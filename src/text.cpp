#include "common.h"
#include "text.h"

#include "painter.h"

VOID Text::DrawText( Painter* pScreen, const Vector2 &vPos, const char* szText, UINT32 uColor )
{
	SDL_Surface *text_surface = TTF_RenderText_Solid(m_pFont, szText, *((SDL_Color*)&uColor));
	if (text_surface != NULL)
	{
		SDL_Rect rect;
		rect.x = vPos.nX;
		rect.y = vPos.nY;
		rect.w = text_surface->w;
		rect.h = text_surface->h;

		SDL_BlitSurface(text_surface, NULL, pScreen->GetScreen(), &rect);
		SDL_FreeSurface(text_surface);
	}
}

BOOL Text::Init()
{
	if( TTF_Init() == -1 ) return FALSE;

	return SetFont("font.ttf");
}

VOID Text::Destroy()
{
	if( m_pFont != NULL )
	{
		TTF_CloseFont(m_pFont);
		m_pFont = NULL;
	}

	TTF_Quit();
}

BOOL Text::SetFont( const char* szPath )
{
	if( m_pFont != NULL )
	{
		TTF_CloseFont(m_pFont);
		m_pFont = NULL;
	}
	m_pFont = TTF_OpenFont("font.ttf", 20);
	if( m_pFont == NULL ) return FALSE;

	return TRUE;
}


Text g_text;