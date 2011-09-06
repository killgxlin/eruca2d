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

// 	pRes->vCollidePos = pRunner->GetPos();
// 
// 	FLOAT fDeep = 0.0f;
// 	pRes->dwDirFlag = thisBox.IntersectTest(thatBox, fDeep, pRunner->GetCollideDirFlag());
// 	if( pRes->dwDirFlag & ECD_Top )
// 	{
// 		pRes->vCollidePos.y += fDeep;
// 	}
// 	else if( pRes->dwDirFlag & ECD_Down )
// 	{
// 		pRes->vCollidePos.y -= fDeep;
// 	}
// 	else if( pRes->dwDirFlag & ECD_Left )
// 	{
// 		pRes->vCollidePos.x -= fDeep;
// 	}
// 	else if( pRes->dwDirFlag & ECD_Right )
// 	{
// 		pRes->vCollidePos.x += fDeep;
// 	}
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
			pRes->vCollidePos.x = pPlayer->GetPos().x;
			pPlayer->m_bLand = true;
			pPlayer->m_fJump = 0.0f;
		}
		if( pRes->dwDirFlag & ECD_Down )
		{
			pRes->vCollidePos.x = pPlayer->GetPos().x;
			if( pPlayer->m_vVel.y > 0 )
			{
				pPlayer->m_vVel.y *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Left )
		{
			pRes->vCollidePos.y = pPlayer->GetPos().y;
			if( pPlayer->m_vVel.x > 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}
		if( pRes->dwDirFlag & ECD_Right )
		{
			pRes->vCollidePos.y = pPlayer->GetPos().y;
			if( pPlayer->m_vVel.x < 0 )
			{
				pPlayer->m_vVel.x *= -1;
			}
		}
		
		pPlayer->SetPos(pRes->vCollidePos);

		SetColor(255, 255, 255);
	}
}

Player::Player() :GameObj(Vector2F(0, 0), ECP_Dynamic, ECD_None), m_vVel(0, 0), m_fJump(0.0f), m_bJmpPressed(false)
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

	if( g_keyboard.GetKey(SDLK_DOWN) )
	{
		m_vVel.y -= XCtrlAcc * dt;
	}

	if( m_vVel.x > XMaxPlayerSpeedX )
	{
		m_vVel.x = XMaxPlayerSpeedX;
	}

	if( m_fJump >= 0.0f )
	{
		if( g_keyboard.GetKey(SDLK_UP) )
		{
			if( !m_bJmpPressed && m_bLand)
			{
				m_vVel.y = XCtrlAcc;
				m_bLand = false;	
			}

			m_fJump += dt;
			if( m_fJump >= 0.5f )
			{
				m_vVel.y = 0;
				m_fJump = -1.0f;
			}

			m_bJmpPressed = true;
		}
		else
		{
			m_vVel.y = 0;
			m_fJump = -1.0f;

			m_bJmpPressed = false;
		}
	}

	if( m_bLand )
	{
		m_vVel.y = 0;
//		m_vVel.x++;
	}


	Vector2F vOffset = m_vVel * dt;


	m_vPos += vOffset;


	m_bLand = false;

	g_text.AddText(g_painter.GetColor(255, 0, 0), "offset  : %4.2f %4.2f", vOffset.x, vOffset.y);
}