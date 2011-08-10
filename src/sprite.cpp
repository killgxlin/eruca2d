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
	Vector2N vSize(m_Size.w, m_Size.h);
	
	AABBox aabb;
	aabb.vMin = vPos - vSize / 2;
	aabb.vMax = vPos + vSize / 2;
	
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
