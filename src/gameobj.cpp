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

	pRes->vCollidePos = pRunner->GetPos();

	FLOAT fDeep = 0.0f;
	pRes->dwDirFlag = thisBox.IntersectTest(thatBox, fDeep, pRunner->GetCollideDirFlag());
	if( pRes->dwDirFlag & ECD_Top )
	{
		pRes->vCollidePos.y += fDeep;
	}
	else if( pRes->dwDirFlag & ECD_Down )
	{
		pRes->vCollidePos.y -= fDeep;
	}
	else if( pRes->dwDirFlag & ECD_Left )
	{
		pRes->vCollidePos.x -= fDeep;
	}
	else if( pRes->dwDirFlag & ECD_Right )
	{
		pRes->vCollidePos.x += fDeep;
	}
// 	Vector2F vD = pRunner->GetPos() - pRunner->GetPrePos();
// 
// 	FLOAT fT = thisBox.IntersectMovingAABB(thatBox, vD, pRes->dwDirFlag);
// 	if( fT >= 0.0f && fT <= 1.0f )
// 	{
// 		pRes->vCollidePos = pRunner->GetPrePos() + vD * fT;
// 	}
// 	else
// 	{
// 		pRes->vCollidePos = pRunner->GetPos();
// 	}

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

Terrain::Terrain( /*const Vector2F &vPos*/ ) :GameObj(Vector2F(0,0), ECD_All)
{
	m_pSprite = new SpriteTile(this);
}

Terrain::~Terrain()
{
	delete m_pSprite;
}

VOID Terrain::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	GameObj::Collide(pRunner, pRes);
	if( !pRes->dwDirFlag ) return;

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
				pPlayer->m_vVel.x *= 0;
			}
		}
		if( pRes->dwDirFlag & ECD_Right )
		{
			pRes->vCollidePos.y = pPlayer->GetPos().y;
			if( pPlayer->m_vVel.x < 0 )
			{
				pPlayer->m_vVel.x *= 0;
			}
		}
		
		pPlayer->SetPos(pRes->vCollidePos);
		SetColor(255, 255, 255);

}

Player::Player() :Movable(Vector2F(0, 0), ECD_None)
{
	m_bJmpPressed = false;
	m_bLand = false;
	m_fJump = 0.0f;
	m_vVel = Vector2F(0,0);
	m_vAcc = Vector2F(0,0);

	m_pPlayer = this;

	m_pSprite = new SpritePlayer(this);
}

Player::~Player()
{
	delete m_pSprite;
}

VOID Player::Update( FLOAT dt )
{
	GameObj::Update(dt);

	Listen();
	UpdatePhysic(dt);

}

VOID Player::Listen()
{
	m_Input.bJump	= g_keyboard.GetKey(SDLK_UP);
	m_Input.bLeft	= g_keyboard.GetKey(SDLK_LEFT);
	m_Input.bRight	= g_keyboard.GetKey(SDLK_RIGHT);
	m_Input.bZoomIn	= g_keyboard.GetKey(SDLK_z);
	m_Input.bZoomOut= g_keyboard.GetKey(SDLK_x);
	m_Input.bFire	= g_keyboard.GetKey(SDLK_SPACE);

}

VOID tagPhysic::UpdatePhysic( FLOAT dt )
{
	m_vAcc = Vector2F(0, XGravity);

	if( m_pPlayer->m_Input.bZoomIn )
	{
		m_pPlayer->GetSprite()->SetSizeFactor(m_pPlayer->GetSprite()->GetSizeFactor() - 0.1f);
	}

	if( m_pPlayer->m_Input.bZoomOut )
	{
		m_pPlayer->GetSprite()->SetSizeFactor(m_pPlayer->GetSprite()->GetSizeFactor() + 0.1f);
	}

	if( m_pPlayer->m_Input.bLeft )
	{
		m_vAcc.x = -XCtrlAcc;
	}

	if( m_pPlayer->m_Input.bRight )
	{
		m_vAcc.x = XCtrlAcc;
	}

	if( m_vAcc.x == 0.0f )
	{
		if( m_vVel.x > 0.0f )
		{
			m_vAcc.x = -XCtrlAcc;
		}
		else if( m_vVel.x < 0.0f )
		{
			m_vAcc.x = XCtrlAcc;
		}
	}
	m_vVel += m_vAcc*dt;

	if( abs(m_vVel.x) > XMaxPlayerSpeedX )
	{
		m_vVel.x = float(m_vVel.x > 0 ? XMaxPlayerSpeedX : -XMaxPlayerSpeedX);
	}

	if( m_fJump >= 0.0f )
	{
		if( m_pPlayer->m_Input.bJump )
		{
			if( !m_bJmpPressed && m_bLand)
			{
				m_vVel.y = XCtrlAcc/4;
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
	}

	Vector2F vOffset = m_vVel * dt;
	m_pPlayer->SetPos(m_pPlayer->GetPos()+vOffset);
	m_bLand = false;
}

