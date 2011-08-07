#include "common.h"
#include "gameobj.h"

#include "sprite.h"
#include "keyboard.h"

VOID GameObj::Update( float dt )
{
	m_pSprite->Animate(dt);
}

VOID GameObj::Draw( Painter* pScreen )
{
	m_pSprite->Draw(pScreen, m_vPos);
}

UINT32 GameObj::Collide( GameObj* pRunner )
{
	AABBox thisBox = GetAABBox();
	AABBox thatBox = pRunner->GetAABBox();

	return thisBox.IntersectTest(thatBox);
}

AABBox GameObj::GetAABBox() const
{
	return m_pSprite->GetAABBox(m_vPos);
}

Tile::Tile( /*const Vector2 &vPos*/ ) :GameObj(Vector2(0,0), ECP_Tile, ECD_All)
{
	m_pSprite = new SpriteTile(this);
}

Tile::~Tile()
{
	delete m_pSprite;
}

UINT32 Tile::Collide( GameObj* pRunner )
{
	UINT32 uFlag = GameObj::Collide(pRunner);
	if( !(uFlag & GetCollideDirFlag()) )
	{
		return ECD_None;
	}

	AABBox thisBox = GetAABBox();
	AABBox thatBox = pRunner->GetAABBox();

	if( pRunner->GetCollidePri() == ECP_Player )
	{
		Vector2 vOffset(0, 0);

		Player *pPlayer = dynamic_cast<Player *>(pRunner);
		if( uFlag & ECD_Top )
		{
			vOffset.nY -= thatBox.vMax.nY - thisBox.vMin.nY;
			if( pPlayer->m_vVel.nY > 0 )
			{
				pPlayer->m_vVel.nY *= -1;
			}
		}
		if( uFlag & ECD_Down )
		{
			vOffset.nY += thisBox.vMax.nY - thatBox.vMin.nY;
		}
		if( uFlag & ECD_Left )
		{
			vOffset.nX -= thatBox.vMax.nX - thisBox.vMin.nX;
			pPlayer->m_vVel.nX = 0;
		}
		if( uFlag & ECD_Right )
		{
			vOffset.nX += thisBox.vMax.nX - thatBox.vMin.nX;
			pPlayer->m_vVel.nX = 0;
		}

		pPlayer->SetPos(pPlayer->GetPos() + vOffset);
	}
	return uFlag;
}

Player::Player() :GameObj(Vector2(0, 0), ECP_Player, ECD_None), m_vVel(0, 0)
{
	m_pSprite = new SpritePlayer(this);
}

Player::~Player()
{
	delete m_pSprite;
}

VOID Player::Update( float dt )
{
	GameObj::Update(dt);


	m_vVel.nY += XGravity;
	if( m_vVel.nY > XMaxPlayerSpeed )
	{
		m_vVel.nY = XMaxPlayerSpeed;
	}

	if( g_keyboard.m_bKey[SDLK_LEFT] )
	{
		m_vVel.nX -= XCtrlAcc;
	}
	if( g_keyboard.m_bKey[SDLK_RIGHT] )
	{
		m_vVel.nX += XCtrlAcc;
	}
	if( g_keyboard.m_bKey[SDLK_UP] )
	{
		m_vVel.nY -= XCtrlAcc;
	}
	if( g_keyboard.m_bKey[SDLK_DOWN] )
	{
		m_vVel.nY += XCtrlAcc;
	}

	Vector2 vOffset = m_vVel * dt;


	m_vPos += vOffset;

	if( m_vPos.nX > 630 )
	{
		m_vPos.nX = 10;
	}
	if( m_vPos.nX < 10 )
	{
		m_vPos.nX = 630;
	}
}
