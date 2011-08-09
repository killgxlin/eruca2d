#include "common.h"
#include "gameobj.h"

#include "sprite.h"
#include "keyboard.h"
#include "painter.h"

VOID GameObj::Update( float dt )
{
	m_pSprite->Animate(dt);
}

VOID GameObj::Draw( Painter* pScreen )
{
	m_pSprite->Draw(pScreen, m_vPos);
}

VOID GameObj::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	AABBox thisBox = GetAABBox();
	AABBox thatBox = pRunner->GetAABBox();

	
	pRes->dwDirFlag = thisBox.IntersectTest(thatBox, pRes->fDeep);
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

VOID Tile::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	GameObj::Collide(pRunner, pRes);
	if( !(pRes->dwDirFlag & GetCollideDirFlag()) )	return;

	if( pRunner->GetCollidePri() == ECP_Player )
	{
		Vector2 vOffset(0, 0);

		Player *pPlayer = dynamic_cast<Player *>(pRunner);
		if( pRes->dwDirFlag & ECD_Top )
		{
			vOffset.y += pRes->fDeep ;
			if( pPlayer->m_vVel.y < 0 )
			{
				pPlayer->m_vVel.y *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Down )
		{
			vOffset.y -= pRes->fDeep;
			if( pPlayer->m_vVel.y > 0 )
			{
				pPlayer->m_vVel.y *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Left )
		{
			vOffset.x -= pRes->fDeep;
			if( pPlayer->m_vVel.x > 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Right )
		{
			vOffset.x += pRes->fDeep;

			if( pPlayer->m_vVel.x < 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}

		pPlayer->SetPos(pPlayer->GetPos() + vOffset);
	}
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

	Vector2 vOld = m_vVel;

	m_vVel.y += XGravity * dt;

	if( g_keyboard.m_bKey[SDLK_LEFT] )
	{
		m_vVel.x -= XCtrlAcc * dt;
	}
	if( g_keyboard.m_bKey[SDLK_RIGHT] )
	{
		m_vVel.x += XCtrlAcc * dt;
	}
	if( g_keyboard.m_bKey[SDLK_UP] )
	{
		m_vVel.y += XCtrlAcc * dt;
	}
	if( g_keyboard.m_bKey[SDLK_DOWN] )
	{
		m_vVel.y -= XCtrlAcc * dt;
	}

	if( m_vVel.Length() > XMaxPlayerSpeed )
	{
		m_vVel = vOld;
	}

	Vector2 vOffset = m_vVel * dt;


	m_vPos += vOffset;

	if( m_vPos.x > 630 )
	{
		m_vPos.x = 10;
	}
	if( m_vPos.x < 10 )
	{
		m_vPos.x = 630;
	}

	g_painter.SetCenter(GetPos());
}
