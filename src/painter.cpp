#include "common.h"
#include "painter.h"

Painter g_painter;

BOOL Painter::Init( INT w, INT h, const char* title )
{
	m_fDrawPerSec	= 0.0f;
	m_dwDt			= 0;
	m_dwDrawTimes	= 0;

	m_pScreen = SDL_SetVideoMode(w, h, 8, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if( NULL == m_pScreen ) return FALSE;

	if( title != NULL )
		SDL_WM_SetCaption(title, 0);

	SetCenter(Vector2F(XScreenW/2, XScreenH/2));
	return TRUE;
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

VOID Painter::DrawRect( const Vector2F &vPos, const SizeN &sSize, UINT32 uColor )
{
	SDL_Rect rect;
	rect.w = static_cast<UINT16>(sSize.w);
	rect.h = static_cast<UINT16>(sSize.h);
	rect.x = static_cast<INT16>(vPos.x - sSize.w / 2);
	rect.y = static_cast<INT16>(vPos.y - sSize.h / 2);

	SDL_FillRect(m_pScreen, &rect, uColor);
	++m_dwDrawTimes;
}

UINT32 Painter::GetColor( UINT8 u8R, UINT8 u8G, UINT8 u8B )
{
	return SDL_MapRGB(m_pScreen->format, u8R, u8G, u8B);
}

VOID Painter::WorldDrawRect( const Vector2F &vWorldPos, const SizeN &sSize, UINT32 uColor )
{
	Vector2F vPos = vWorldPos;
	WorldToScreen(&vPos);
	ScreenToSDL(&vPos);

	if( IsBetweenClose<FLOAT>(vPos.x, 0, XScreenW) && IsBetweenClose<FLOAT>(vPos.y, 0, XScreenH) )
	{
	}	
	DrawRect(vPos, sSize, uColor);

}

VOID Painter::WorldToScreen( Vector2F* pPt )
{
	Vector2F vSize(FLOAT(m_pScreen->w), FLOAT(m_pScreen->h));
	Vector2F vLeftBottom = m_vCenter - vSize / 2;
	*pPt -= vLeftBottom;
}

VOID Painter::ScreenToSDL( Vector2F* pPt )
{
	pPt->y = m_pScreen->h - pPt->y;
}

VOID Painter::Update( DWORD dwDt )
{
	m_dwDt += dwDt;
	if( m_dwDt >= 1000 )
	{
		m_fDrawPerSec = m_dwDrawTimes * 1000.0f / m_dwDt;
		m_dwDt = 0;
		m_dwDrawTimes = 0;
	}
}
