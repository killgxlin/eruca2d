#include "common.h"
#include "painter.h"
#include "SDL_gfxPrimitives.h"

Painter g_painter;

BOOL Painter::Init( INT w, INT h, const char* title )
{
	m_fDrawPerSec	= 0.0f;
	m_dwDt			= 0;
	m_dwDrawTimes	= 0;
	m_fZoomRate		= 1.0f;
	m_screenBox.AddPoint(Vector2F(0, 0));
	m_screenBox.AddPoint(Vector2F(XScreenW, XScreenH));

	m_pScreen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
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

VOID Painter::DrawRect( const Vector2F &vPos, const Vector2F &sSize, UINT32 uColor )
{
	SDL_Rect rect;
	rect.w = static_cast<UINT16>(sSize.x);
	rect.h = static_cast<UINT16>(sSize.y);
	rect.x = static_cast<INT16>(vPos.x - sSize.x / 2);
	rect.y = static_cast<INT16>(vPos.y - sSize.y / 2);

	SDL_FillRect(m_pScreen, &rect, uColor);
	
	++m_dwDrawTimes;
}

UINT32 Painter::GetColor( UINT8 u8R, UINT8 u8G, UINT8 u8B )
{
	return SDL_MapRGBA(m_pScreen->format, u8R, u8G, u8B, 255);
}

VOID Painter::WorldDrawRect( const Vector2F &vWorldPos, const Vector2F &vSize, UINT32 uColor )
{
	{
		Vector2F vPos = vWorldPos;
		Vector2F vLSize = vSize;

		WorldToScreen(m_vCenter, &vPos, &vLSize);
		ScreenToSDL(&vPos);

		Square rectBox(vPos, vLSize);

		if( m_screenBox.IntersectBox(rectBox) )
		{
			DrawRect(vPos, vLSize, uColor);
		}
	}

	if( m_vOtherCenter != m_vCenter )
	{
		Vector2F vPos = vWorldPos;
		Vector2F vLSize = vSize;

		WorldToScreen(m_vOtherCenter, &vPos, &vLSize);
		ScreenToSDL(&vPos);

		Square rectBox(vPos, vLSize);

		if( m_screenBox.IntersectBox(rectBox) )
		{
			DrawRect(vPos, vLSize, uColor);
		}
	}
}

VOID Painter::WorldToScreen( const Vector2F &vCenter, Vector2F* pPt, Vector2F* pSize )
{
	Vector2F vSize(FLOAT(m_pScreen->w), FLOAT(m_pScreen->h));
	Vector2F vLeftBottom = vCenter - vSize / 2;
	*pPt -= vLeftBottom;

	Vector2F vVec = *pPt - vSize / 2;
	vVec *= m_fZoomRate;

	*pPt = vVec + vSize / 2;
	if( NULL != pSize )
	{
		*pSize *= m_fZoomRate;
	}
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
	if( g_keyboard.GetKey(SDLK_INSERT) )
	{
		ModZoomRate(0.01f);
	}
	if( g_keyboard.GetKey(SDLK_DELETE) )
	{
		ModZoomRate(-0.01f);
	}

	g_text.AddText(g_painter.GetColor(255, 0, 0), "draw ps  : %4.2f", g_painter.GetDrawPerSec());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "zoom rate: %4.2f", g_painter.GetZoomRate());
}

VOID Painter::WorldDrawText( const Vector2F &vWorldPos, UINT32 uColor, const char* szFormat, ... )
{
	va_list argList;
	va_start(argList, szFormat);
	char buffer[1024];
	vsprintf_s(buffer, 1024, szFormat, argList);
	va_end(argList);

	Vector2F vPos = vWorldPos;
	WorldToScreen(m_vCenter, &vPos, NULL);
	ScreenToSDL(&vPos);
	g_text.DrawText(vPos, uColor, buffer);

	if( m_vOtherCenter != m_vCenter )
	{
		Vector2F vPos = vWorldPos;
		WorldToScreen(m_vCenter, &vPos, NULL);
		ScreenToSDL(&vPos);
		g_text.DrawText(vPos, uColor, buffer);

	}
}

VOID Painter::SetCenter( const Vector2F &vPos )
{
	m_vCenter = vPos;
	m_vOtherCenter = m_vCenter;
	if( m_vCenter.x + m_pScreen->w / m_fZoomRate / 2 >= XTotalW )
	{
		m_vOtherCenter.x -= XTotalW;
	}
	else if( m_vCenter.x - m_pScreen->w / m_fZoomRate / 2 < 0 )
	{
		m_vOtherCenter.x += XTotalW;
	}

}

VOID Painter::WorldDrawLine( const Vector2F &vWorldPosHead, const Vector2F &vWorldPosTail, DWORD dwColor )
{
	if( vWorldPosHead == vWorldPosTail )
	{
		return;
	}

	Vector2F vHead = vWorldPosHead, vTail = vWorldPosTail;

	if( vWorldPosHead.x > vWorldPosTail.x )
	{
		 vHead = vWorldPosTail;
		 vTail = vWorldPosHead;
	}

	Vector2F vHeadPos = vHead;
	WorldToScreen(m_vCenter, &vHeadPos, NULL);
	ScreenToSDL(&vHeadPos);

	Vector2F vTailPos = vTail;
	WorldToScreen(m_vCenter, &vTailPos, NULL);
	ScreenToSDL(&vTailPos);

	thickLineColor(m_pScreen, vHeadPos.x, vHeadPos.y, vTailPos.x, vTailPos.y, 1, dwColor);
	++m_dwDrawTimes;

	if( m_vOtherCenter != m_vCenter )
	{
		Vector2F vHeadPos = vHead;
		WorldToScreen(m_vOtherCenter, &vHeadPos, NULL);
		ScreenToSDL(&vHeadPos);

		Vector2F vTailPos = vTail;
		WorldToScreen(m_vOtherCenter, &vTailPos, NULL);
		ScreenToSDL(&vTailPos);

		thickLineColor(m_pScreen, vHeadPos.x, vHeadPos.y, vTailPos.x, vTailPos.y, 1, dwColor);
		++m_dwDrawTimes;
	}
}

Square Painter::GetOtherScreenBox() const
{
	return Square(m_vOtherCenter, Vector2F(XScreenW, XScreenH) / m_fZoomRate);
}

Square Painter::GetScreenBox() const
{
	return Square(m_vCenter, Vector2F(XScreenW, XScreenH) / m_fZoomRate);
}

VOID Painter::SDLToWorld( Vector2F* pPt )
{
	// SDL Screen
	pPt->y = m_pScreen->h - pPt->y;

	// Screen World
	Vector2F vSize(FLOAT(m_pScreen->w), FLOAT(m_pScreen->h));

	Vector2F vVec = *pPt - vSize / 2;
	vVec /= m_fZoomRate;

	*pPt = vVec + vSize / 2;

	Vector2F vLeftBottom = m_vCenter - vSize / 2;
	*pPt += vLeftBottom;

}

VOID Painter::WorldDrawImg( const Vector2F &vWorldPos, SDL_Surface* pSurface )
{
	Vector2F vPos = vWorldPos;
	WorldToScreen(m_vCenter, &vPos, NULL);
	ScreenToSDL(&vPos);

	Vector2F vSize(pSurface->w, pSurface->h);
	Vector2F vLB = vPos - vSize / 2;

	SDL_Rect rect;
	rect.x = vLB.x;
	rect.y = vLB.y;
	rect.w = pSurface->w;
	rect.h = pSurface->h;
	
	SDL_BlitSurface(pSurface, NULL, m_pScreen, &rect);
	++m_dwDrawTimes;
}
