#include "common.h"
#include "painter.h"
#include "SDL_gfxPrimitives.h"
#ifndef XUseGL
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

VOID Painter::EndDraw()
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

// 	g_text.AddText(XColorR, "draw ps  : %4.2f", g_painter.GetDrawPerSec());
// 	g_text.AddText(XColorR, "zoom rate: %4.2f", g_painter.GetZoomRate());
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

VOID Painter::WinToWorld( Vector2F* pPt )
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

VOID Painter::BeginDraw()
{

}

VOID Painter::WinDrawText( const Vector2F &vWinPos, SDL_Surface* pSurface )
{
	SDL_Rect rect;

	rect.x = vWinPos.x;
	rect.y = vWinPos.y;
	rect.w = pSurface->w;
	rect.h = pSurface->h;

	SDL_BlitSurface(pSurface, NULL, m_pScreen, &rect);
}
#else
BOOL Painter::Init( INT w, INT h, const char* title )
{
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) return FALSE;

	const SDL_VideoInfo* info = SDL_GetVideoInfo();
    if( !info ) return FALSE;

    if( SDL_SetVideoMode( w, h, info->vfmt->BitsPerPixel, SDL_OPENGL ) == 0 ) return FALSE;
	if( title != NULL )
		SDL_WM_SetCaption(title, 0);


    glShadeModel( GL_SMOOTH );
	glEnable (GL_BLEND);


    glClearColor( 0, 0, 0, 0 );

    glViewport( 0, 0, w, h );

	m_vSize = Vector2F(w, h);

	SetCenter(Vector2F(0, 0));

	return TRUE;
}

void Painter::SetCenter(const Vector2F &vPos)
{
	m_vCenter = vPos;
	m_vOtherCenter = m_vCenter;
	m_sScreenBox = Square(m_vCenter, m_vSize);
	
	if( m_sScreenBox.vMax.x >= XTotalW )
	{
		m_vOtherCenter.x -= XTotalW;
	}
	else if( m_sScreenBox.vMin.x < 0 )
	{
		m_vOtherCenter.x += XTotalW;
	}
	m_sOtherScreenBox = Square(m_vOtherCenter, m_vSize);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	glOrtho(m_sScreenBox.vMin.x, m_sScreenBox.vMax.x, m_sScreenBox.vMin.y, m_sScreenBox.vMax.y, -10, 10);
}

VOID Painter::Destroy()
{
	// Çå¿ÕÎÆÀí»º³å
	for(TexMap::iterator itr = m_mapTex.begin();
		itr != m_mapTex.end();
		++itr)
	{
		ReturnTex(itr->second);
	}

	m_mapTex.clear();
	
}

VOID Painter::BeginDraw()
{
	glEnable(GL_TEXTURE_2D);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

}

VOID Painter::EndDraw()
{
	glPopMatrix();
	SDL_GL_SwapBuffers();
}

VOID Painter::Clear()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

VOID Painter::WinToWorld( Vector2F* pPt )
{
	// Win Screen
	pPt->y = m_vSize.y - pPt->y;

	// Screen World
	Vector2F vHalfSize = m_vSize / 2;

	Vector2F vVec = *pPt - vHalfSize;
/*	vVec /= m_fZoomRate;*/

	*pPt = vVec + vHalfSize;

	Vector2F vLeftBottom = m_vCenter - vHalfSize;
	*pPt += vLeftBottom;
}

VOID Painter::WorldDrawLine( const Vector2F &vWorldPosHead, const Vector2F &vWorldPosTail, DWORD dwColor )
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE);
		glVertex2f(vWorldPosHead.x, vWorldPosHead.y);
		glVertex2f(vWorldPosTail.x, vWorldPosTail.y);
	glEnd();
}

VOID Painter::WorldDrawImg( const Vector2F &vWorldPos, SDL_Surface* pSurface )
{
	Vector2F vSize(pSurface->w, pSurface->h);
	Square sBox(vWorldPos, vSize);

	GLuint tex = GetTex(pSurface, TRUE);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(sBox.vMin.x, sBox.vMin.y);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(sBox.vMax.x, sBox.vMin.y);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(sBox.vMax.x, sBox.vMax.y);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(sBox.vMin.x, sBox.vMax.y);
	glEnd();
}
#include <gl/glut.h>
void
print_bitmap_string(void* font, char* s)
{
	if (s && strlen(s)) {
		while (*s) {
			glutBitmapCharacter(font, *s);
			s++;
		}
	}
}
VOID Painter::WinDrawText( const Vector2F &vWinPos, char* szStr )
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2f(vWinPos.x, vWinPos.y);
	print_bitmap_string(GLUT_BITMAP_9_BY_15, szStr);
}

GLuint Painter::GetTex( SDL_Surface* pSurface, BOOL bNeedRet )
{
	GLuint tex;
	if( bNeedRet )
	{
		TexMap::iterator itr = m_mapTex.find(pSurface);
		if( itr == m_mapTex.end() )
		{
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pSurface->w, pSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, pSurface->pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			m_mapTex.insert(make_pair(pSurface, tex));
		}
		else
		{
			tex = itr->second;
		}
	}
	else
	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pSurface->w, pSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, pSurface->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	return tex;
}

VOID Painter::ReturnTex( GLuint tex )
{
	glDeleteTextures(1, &tex);
}


VOID Painter::Update( DWORD dwDt )
{

}

#endif

Painter g_painter;
