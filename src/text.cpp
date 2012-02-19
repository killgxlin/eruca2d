#include "common.h"
#include "text.h"

BOOL Text::Init()
{
	if( TTF_Init() == -1 ) return FALSE;
	
	m_lstUnits.clear();
	m_bDrawText = TRUE;

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
	m_pFont = TTF_OpenFont("font.ttf", 15);
	if( m_pFont == NULL ) return FALSE;

	return TRUE;
}

VOID Text::DrawText(const Vector2F &vPos, UINT32 uColor, const char* szFormat, ...)
{
	if( !m_bDrawText ) return;

	va_list argList;
	va_start(argList, szFormat);
	char buffer[1024];
	vsprintf_s(buffer, 1024, szFormat, argList);
	va_end(argList);

#ifdef XUseGL
	g_painter.WinDrawText(vPos, buffer);
#else
	SDL_Surface *text_surface = TTF_RenderText_Blended(m_pFont, buffer, *((SDL_Color*)&uColor));//*((SDL_Color*)&uColor));
	if (text_surface != NULL)
	{
		g_painter.WinDrawText(vPos, text_surface);
		SDL_FreeSurface(text_surface);
	}

#endif

}

VOID Text::AddText( UINT32 uColor, const char* szFormat, ... )
{
	if( !m_bDrawText ) return;

	va_list argList;
	va_start(argList, szFormat);
	char buffer[1024];
	vsprintf_s(buffer, 1024, szFormat, argList);
	va_end(argList);

	tagUnit unit;
	unit.strMsg = buffer;
	unit.uColor = uColor;
	m_lstUnits.push_back(unit);

}

VOID Text::DrawTextAll()
{
	if( g_keyboard.FetchKey(SDLK_F11) )
	{
		DrawSwitch();
	}

	Vector2F vPos(0, 0);
	INT nFontHight = TTF_FontHeight(m_pFont);
	while( !m_lstUnits.empty() )
	{
		DrawText(vPos, m_lstUnits.front().uColor, m_lstUnits.front().strMsg.c_str());
		vPos.y += nFontHight;
		m_lstUnits.pop_front();
	}
}


Text g_text;