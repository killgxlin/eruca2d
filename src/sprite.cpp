#include "common.h"
#include "sprite.h"

#include "painter.h"

#include "gameobj.h"

VOID Sprite::Draw(Painter* pPainter, const Vector2F &vPos)
{
	pPainter->WorldDrawRect(vPos, m_Size, pPainter->GetColor(m_u8R, m_u8G, m_u8B));
}

AABBox Sprite::GetAABBox( const Vector2F &vPos )
{
	Vector2N vHalfSize(m_Size.w/2, m_Size.h/2);
	
	AABBox aabb;
	aabb.vMin = vPos - vHalfSize;
	aabb.vMax = vPos + vHalfSize;
	
	return aabb;
}

VOID SpritePlayer::Animate( FLOAT dt )
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameObj);

	FLOAT fPct = pPlayer->m_vVel.Length() / FLOAT(XMaxPlayerSpeed);
	
	if( fPct > 1.0f )
	{
		fPct = 1.0f;
	}

	m_u8R = UINT8(fPct * 255);
}
