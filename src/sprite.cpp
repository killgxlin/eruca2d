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
