#include "common.h"
#include "gameobj.h"

#include "sprite.h"

VOID GameObj::Update( FLOAT dt )
{
	m_pSprite->Animate(dt);
	m_vPrePos = m_vPos;
}

VOID GameObj::Draw( Painter* pScreen )
{
	m_pSprite->Draw(pScreen, m_vPos);
}

VOID GameObj::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	AABBox thisBox = GetAABBox();
	AABBox thatBox = pRunner->GetAABBox();

	Vector2F vD = pRunner->GetPos() - pRunner->GetPrePos();

	FLOAT fT = thisBox.IntersectMovingAABB(thatBox, vD, pRes->dwDirFlag);
	if( fT >= 0.0f && fT <= 1.0f )
	{
		pRes->vCollidePos = pRunner->GetPrePos() + vD * fT;
	}
	else
	{
		pRes->vCollidePos = pRunner->GetPos();
	}

}

AABBox GameObj::GetAABBox() const
{
	return m_pSprite->GetAABBox(m_vPos);
}

VOID GameObj::SetColor( UINT8 u8R, UINT8 u8G, UINT8 u8B )
{
	m_pSprite->SetColor(u8R, u8G, u8B);
}

AABBox GameObj::GetMoveBox() const
{
	AABBox curBox = m_pSprite->GetAABBox(m_vPos);
	AABBox preBox = m_pSprite->GetAABBox(m_vPrePos);
	curBox.AddBox(preBox);
	
	return curBox;
}

Tile::Tile( /*const Vector2F &vPos*/ ) :GameObj(Vector2F(0,0), ECP_Static, ECD_All)
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
	if( !pRes->dwDirFlag ) return;

	if( pRunner->GetCollidePri() == ECP_Dynamic )
	{
		Player *pPlayer = dynamic_cast<Player *>(pRunner);
		if( pRes->dwDirFlag & ECD_Top )
		{
			if( pPlayer->m_vVel.y < 0 )
			{
				pPlayer->m_vVel.y *= 0;
			}
//			pRes->vCollidePos = GetPos() + Vector2F(0, XTileSize + 0.1f);
			pPlayer->m_bLand = true;
		}
		if( pRes->dwDirFlag & ECD_Down )
		{
			if( pPlayer->m_vVel.y > 0 )
			{
				pPlayer->m_vVel.y *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Left )
		{
			if( pPlayer->m_vVel.x > 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Right )
		{
			if( pPlayer->m_vVel.x < 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}

		pPlayer->SetPos(pRes->vCollidePos);
		SetColor(255, 255, 255);
	}
}

Player::Player() :GameObj(Vector2F(0, 0), ECP_Dynamic, ECD_None), m_vVel(0, 0)
{
	m_pSprite = new SpritePlayer(this);
}

Player::~Player()
{
	delete m_pSprite;
}

VOID Player::Update( FLOAT dt )
{
	GameObj::Update(dt);

	Vector2F vOld = m_vVel;

	m_vVel.y += XGravity * dt;

	if( g_keyboard.GetKey(SDLK_LEFT) )
	{
		m_vVel.x -= XCtrlAcc * dt;
	}
	if( g_keyboard.GetKey(SDLK_RIGHT) )
	{
		m_vVel.x += XCtrlAcc * dt;
	}
	if( g_keyboard.FetchKey(SDLK_UP) )
	{
		if( m_bLand )
		{
			m_vVel.y = m_vVel.x;
			m_bLand = false;
		}
	}
	if( g_keyboard.GetKey(SDLK_DOWN) )
	{
		m_vVel.y -= XCtrlAcc * dt;
	}

	if( m_vVel.Length() > XMaxPlayerSpeed )
	{
		m_vVel = vOld;
	}

	if( m_bLand )
	{
		m_vVel.y = 0;
		m_vVel.x++;
		if( m_vVel.x > 100 )
		{
			m_vVel.x = 100;
		}
	}

	Vector2F vOffset = m_vVel * dt;


	m_vPos += vOffset;

	g_painter.SetCenter(GetPos());

	m_bLand = false;
}