#include "common.h"
#include "sprite.h"
#include "SDL_rotozoom.h"

#include "gameobj.h"
//-----------------------------------------------------------------------------------------------

VOID Sprite::Draw(Painter* pPainter, const Vector2F &vPos)
{
	pPainter->WorldDrawRect(vPos, m_Size * m_fSizeFactor, pPainter->GetColor(m_u8R, m_u8G, m_u8B));
}

Square Sprite::GetAABBox( const Vector2F &vPos )
{
	return Square(vPos, m_Size * m_fSizeFactor);
}
//-----------------------------------------------------------------------------------------------

VOID SpritePlayer::Animate( FLOAT dt )
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameObj);

	FLOAT fPct = pPlayer->m_vVel.Length() / FLOAT(XMaxPlayerSpeedX);
	
	if( fPct > 1.0f )
	{
		fPct = 1.0f;
	}

	m_u8R = UINT8(fPct * 255);
}

VOID SpritePlayer::Draw( Painter* pPainter, const Vector2F &vPos )
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameObj);

	Sprite::Draw(pPainter, vPos);

	Vector2F vHead = vPos;
	Vector2F vTail = vHead + pPlayer->m_vArrowDir * XArrowSize;
	g_painter.WorldDrawLine(vHead, vTail, g_painter.GetColor(255, 255, 255));
}

//-----------------------------------------------------------------------------------------------

VOID SpriteArrow::Draw( Painter* pPainter, const Vector2F &vPos )
{
	Vector2F vHead = vPos - m_vVelUnit/2;
	Vector2F vTail = vPos + m_vVelUnit/2;

	g_painter.WorldDrawLine(vHead, vTail, g_painter.GetColor(m_u8R, m_u8G, m_u8B));
}

Square SpriteArrow::GetAABBox( const Vector2F &vPos )
{
	Vector2F vLastVel = static_cast<Arrow*>(m_pGameObj)->m_vLastVel;

	m_vVelUnit = vLastVel * m_Size.x / vLastVel.Length();

	Square tmpBox;
	tmpBox.AddPoint(vPos + m_vVelUnit / 2);
	tmpBox.AddPoint(vPos - m_vVelUnit / 2);

	return tmpBox;
}

BOOL SpriteEx::Load()
{
	struct tagAniInfo
	{
		EActType	eActType;
		const char* szName;
		INT			nNum;
	};

	tagAniInfo arrInfo[] = {
		{	EAT_Stand,	"rockman/stand%02d.bmp",	6	},
		{	EAT_Run,	"rockman/run%02d.bmp",		11	},
		{	EAT_Jump,	"rockman/jmp%02d.bmp",		6	},
		{	EAT_Fall,	"rockman/fall%02d.bmp",		4	},
		{	EAT_Land,	"rockman/land%02d.bmp",		2	},
	};
	
	INT nInfoLen = (sizeof(arrInfo) / sizeof(arrInfo[0]));
	for(INT i=0; i<nInfoLen; ++i)
	{
		char buffer[100] = {0};
		for(INT j=1; j<=arrInfo[i].nNum; ++j)
		{
			sprintf_s(buffer, arrInfo[i].szName, j);
			SDL_Surface* pSrc = SDL_LoadBMP(buffer);
			if( pSrc == NULL ) return FALSE;

			SDL_Surface* pRight = zoomSurface(pSrc, 0.5f, 0.5f, SMOOTHING_ON);
			SDL_Surface* pLeft = zoomSurface(pSrc, -0.5f, 0.5f, SMOOTHING_ON);
			if( pRight == NULL || pLeft == NULL ) return FALSE;


			SDL_SetColorKey(pRight, SDL_SRCCOLORKEY, SDL_MapRGB(pSrc->format, 0, 0, 0));
			SDL_SetColorKey(pLeft, SDL_SRCCOLORKEY, SDL_MapRGB(pSrc->format, 0, 0, 0));
			SDL_FreeSurface(pSrc);

			m_arrSurfaces[EAD_Right][arrInfo[i].eActType].push_back(pRight);
			m_arrSurfaces[EAD_Left][arrInfo[i].eActType].push_back(pLeft);
		}
	}
	return TRUE;
}

VOID SpriteEx::UnLoad()
{
	for(INT i=EAD_Right; i<EAD_End; ++i)
	{
		for(INT j=EAT_Stand; j<EAT_End; ++j)
		{
			for(INT nSize = m_arrSurfaces[i][j].size()-1; nSize >= 0; --nSize)
			{
				SDL_FreeSurface(m_arrSurfaces[i][j][nSize]);
			}
			m_arrSurfaces[i][j].clear();
		}
	}
}

VOID SpriteEx::Animate( FLOAT dt )
{
	m_fTimer += dt;
	if( m_fTimer > 1.0f )
	{
		if( m_bLoop )
		{
			m_fTimer = m_fTimer - floorf(m_fTimer);
		}
		else
		{
			m_fTimer = 1.0f;
		}
	}
	
}

VOID SpriteEx::Draw( Painter* pPainter, const Vector2F &vPos )
{
	VecSurface &vec = m_arrSurfaces[m_eDir][m_eType];
	INT nIdx = ceilf((vec.size() - 1) * m_fTimer);
	g_painter.WorldDrawImg(vPos, vec[nIdx]);
	
}

Square SpriteEx::GetAABBox( const Vector2F &vPos )
{
	VecSurface &vec = m_arrSurfaces[m_eDir][m_eType];
	INT nIdx = ceilf((vec.size() - 1) * m_fTimer);
	return Square(vPos, Vector2F(vec[nIdx]->w, vec[nIdx]->h));
}
