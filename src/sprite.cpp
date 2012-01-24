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
	SpriteDirAction::Draw(pPainter, vPos);

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

Square SpriteAction::GetAABBox( const Vector2F &vPos )
{
	if( m_pCurAction == NULL )
	{
		return Square(vPos, Vector2F(XPlayerSize, XPlayerSize));
	}
	return Square(vPos, m_pCurAction->vSize);
}

BOOL SpriteAction::Start( const char* szAlias, INT nCount /*= -1*/ )
{
	INT32 nAliasCrc = ssh_crc32(szAlias);
	if( nAliasCrc == m_nCurAliasCrc )
	{
		m_nCount = nCount;
		return TRUE;
	}
	ActionMap::iterator itr = m_mapActions.find(nAliasCrc);
	if( itr == m_mapActions.end() )
	{
		return FALSE;
	}

	m_nCurAliasCrc = nAliasCrc;
	m_fCurCuration = 0.0f;
	m_nCount = nCount;
	m_pCurAction = itr->second;

	return TRUE;
}

VOID SpriteAction::RegAction( tagAction* pAction, const char* szAlias )
{
	assert(pAction != NULL);

	if( pAction == NULL ) return;

	INT32 nAliasCrc = ssh_crc32(szAlias);
	ActionMap::iterator itr = m_mapActions.find(nAliasCrc);
	if( itr != m_mapActions.end() ) return;

	m_mapActions.insert(make_pair(nAliasCrc, pAction));
}

VOID SpriteAction::Draw( Painter* pPainter, const Vector2F &vPos )
{
	if( m_pCurAction == NULL ) return;

	tagAction::SurfaceVec &vec = m_pCurAction->vecSurfaces;
	FLOAT fDuration = m_pCurAction->fDuration;
	if( vec.size() == 0 ) return;

	INT nIdx = static_cast<INT>(ceilf((vec.size() - 1) * m_fCurCuration / fDuration));
	g_painter.WorldDrawImg(vPos, vec[nIdx]);
}

VOID SpriteAction::Animate( FLOAT dt )
{
	if( m_pCurAction == NULL ) return;

	FLOAT fDuration =m_pCurAction->fDuration;

	m_fCurCuration += dt;
	if( m_fCurCuration > m_pCurAction->fDuration )
	{
		if( m_nCount != 0 )
		{
			m_fCurCuration -= floorf(m_fCurCuration / fDuration) * fDuration;
			if( m_nCount > 0 )
			{
				-- m_nCount;
			}
		}
		else
		{
			m_fCurCuration = m_pCurAction->fDuration;
		}
	}
}
