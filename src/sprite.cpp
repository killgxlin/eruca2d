#include "common.h"
#include "sprite.h"

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

VOID SpritePlayer::Draw( Painter* pPainter, const Vector2F &vPos )
{
	SpriteAnimate::Draw(pPainter, vPos);

	Player* pPlayer = dynamic_cast<Player*>(m_pGameObj);
	if( pPlayer != NULL ) return;

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

VOID SpriteAnimate::Animate( FLOAT dt )
{
	FLOAT fLastTime = m_pAniProto->arrSurfaces[m_eType].fLastTime;

	m_fTimer += dt;
	if( m_fTimer > fLastTime )
	{
		if( m_bLoop )
		{
			m_fTimer -= floorf(m_fTimer / fLastTime) * fLastTime;
		}
		else
		{
			m_fTimer = fLastTime;
		}
	}
	
}

VOID SpriteAnimate::Draw( Painter* pPainter, const Vector2F &vPos )
{
	FLOAT fLastTime = m_pAniProto->arrSurfaces[m_eType].fLastTime;
	const tagAnimateProto::VecSurface &vec = m_pAniProto->arrSurfaces[m_eType].acts[m_eDir];
	INT nIdx = ceilf((vec.size() - 1) * m_fTimer / fLastTime);
	g_painter.WorldDrawImg(vPos, vec[nIdx]);
	
}

Square SpriteAnimate::GetAABBox( const Vector2F &vPos )
{
	FLOAT fLastTime = m_pAniProto->arrSurfaces[m_eType].fLastTime;
	const tagAnimateProto::VecSurface &vec = m_pAniProto->arrSurfaces[m_eType].acts[m_eDir];
	INT nIdx = ceilf((vec.size() - 1) * m_fTimer / fLastTime);
	//return Square(vPos, m_pAniProto->vBox);
	//return Square(vPos, Vector2F(vec[nIdx]->w, vec[nIdx]->h));
	return Square(vPos, Vector2F(XPlayerSize, XPlayerSize));
}

SpriteAnimate::SpriteAnimate( GameObj* pGameObj, const char* szAniName) :Sprite(pGameObj, 0, 0, 0, 0, 0), m_bPlay(FALSE), m_fTimer(0.0f), m_eType(EAT_Stand), m_eDir(EAD_Right)
{
	const tagAnimateProto* pProto = g_resmgr.GetAnimateProto(szAniName);
	assert(pProto != NULL);
	m_pAniProto = pProto;
}

SpriteAnimate::~SpriteAnimate()
{
	m_pAniProto = NULL;
}

VOID SpriteImage::Draw( Painter* pPainter, const Vector2F &vPos )
{
	pPainter->WorldDrawImg(vPos, m_pSurface);
}

Square SpriteImage::GetAABBox( const Vector2F &vPos )
{
	return Square(vPos, Vector2F(m_pSurface->w, m_pSurface->h));
}

SpriteImage::SpriteImage( GameObj* pGameObj, const char* szName ) : Sprite(pGameObj, 0, 0, 0, 0, 0)
{
	m_pSurface = const_cast<SDL_Surface*>(g_resmgr.GetImageSurface("icebrick"));
}
