#include "common.h"
#include "sprite.h"

#include "painter.h"

#include "gameobj.h"

VOID Sprite::Draw(Painter* pPainter, const Vector2 &vPos)
{
	pPainter->DrawRect(vPos, m_Size, SDL_MapRGB(pPainter->GetScreen()->format,m_u8R, m_u8G, m_u8B));
}

AABBox Sprite::GetAABBox( const Vector2 &vPos )
{
	Vector2 vSize(m_Size.w, m_Size.h);
	
	AABBox aabb;
	aabb.vMin = vPos - vSize / 2;
	aabb.vMax = vPos + vSize / 2;
	
	return aabb;
}

VOID SpritePlayer::Animate( float dt )
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameObj);

	float fPct = pPlayer->m_vVel.Length() / float(XMaxPlayerSpeed);
	
	if( fPct > 1.0f )
	{
		fPct = 1.0f;
	}

	m_u8R = fPct * 255;
}
