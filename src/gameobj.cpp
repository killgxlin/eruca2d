#include "common.h"
#include "gameobj.h"
#include "level.h"

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

Terrain::Terrain( /*const Vector2F &vPos*/ ) :GameObj(Vector2F(0,0), ECD_All, EGOT_Terrain)
{
	m_pSprite = new SpriteTerrain(this);
}

Terrain::~Terrain()
{
	delete m_pSprite;
}

VOID Terrain::Collide( Movable* pMover, tagCollideRes* pRes )
{
	GameObj::Collide(pMover, pRes);
	if( !pRes->dwDirFlag ) return;

	switch( pMover->GetType() )
	{
	case EGOT_Player:
		{
			Player *pPlayer = dynamic_cast<Player *>(pMover);
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
		break;
	case EGOT_Arrow:
		{
			Arrow* pArrow = dynamic_cast<Arrow *>(pMover);
			pArrow->m_vVel = Vector2F(0, 0);
			pArrow->m_bLand = true;
			SetColor(255, 255, 255);
		}
		break;
	}
}

Player::Player() :Movable(Vector2F(0, 0), ECD_None, EGOT_Player)
{
	m_bJmpPressed = false;
	m_bLand = false;
	m_fJump = 0.0f;
	m_vVel = Vector2F(0,0);

	m_pMover = this;

	m_pSprite = new SpritePlayer(this);
}

Player::~Player()
{
	delete m_pSprite;
}

VOID Player::Update( FLOAT dt )
{

	Movable::Update(dt);
	
	if( m_Input.bFire )
	{
		Arrow* pArrow = new Arrow;
		pArrow->SetPos(GetPos());
		pArrow->m_vVel.x = XMaxPlayerSpeedX*2;
		pArrow->m_vVel.y = XMaxPlayerSpeedX/2;
		g_level.m_lstArrows.push_back(pArrow);
	}
}

VOID Player::HandleInput()
{
	m_Input.bJump	= g_keyboard.GetKey(SDLK_UP);
	m_Input.bLeft	= g_keyboard.GetKey(SDLK_LEFT);
	m_Input.bRight	= g_keyboard.GetKey(SDLK_RIGHT);
	m_Input.bZoomIn	= g_keyboard.GetKey(SDLK_z);
	m_Input.bZoomOut= g_keyboard.GetKey(SDLK_x);
	m_Input.bFire	= g_keyboard.FetchKey(SDLK_SPACE);

}

VOID Player::UpdatePhysic( float dt )
{
	Vector2F vAcc = XGravity;

	if( m_pMover->m_Input.bZoomIn )
	{
		m_pMover->GetSprite()->SetSizeFactor(m_pMover->GetSprite()->GetSizeFactor() - 0.1f);
	}

	if( m_pMover->m_Input.bZoomOut )
	{
		m_pMover->GetSprite()->SetSizeFactor(m_pMover->GetSprite()->GetSizeFactor() + 0.1f);
	}

	if( m_pMover->m_Input.bLeft )
	{
		vAcc.x = -XCtrlAcc;
	}

	if( m_pMover->m_Input.bRight )
	{
		vAcc.x = XCtrlAcc;
	}

	if( vAcc.x == 0.0f )
	{
		if( m_vVel.x > 0.0f )
		{
			vAcc.x = -XCtrlAcc;
		}
		else if( m_vVel.x < 0.0f )
		{
			vAcc.x = XCtrlAcc;
		}
	}
	m_vVel += vAcc*dt;

	if( abs(m_vVel.x) > XMaxPlayerSpeedX )
	{
		m_vVel.x = float(m_vVel.x > 0 ? XMaxPlayerSpeedX : -XMaxPlayerSpeedX);
	}

	if( m_fJump >= 0.0f )
	{
		if( m_pMover->m_Input.bJump )
		{
			if( !m_bJmpPressed && m_bLand)
			{
				m_vVel.y = XJumpSpeed;
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

	Movable::UpdatePhysic(dt);
}

Arrow::Arrow() :Movable(Vector2F(0, 0), ECD_All, EGOT_Arrow)
{
	m_pMover = this;
	m_bLand = false;
	m_pSprite = new SpriteArrow(this);
}

Arrow::~Arrow()
{
	delete m_pSprite;
	m_pSprite = NULL;
}

VOID Arrow::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	if( pRunner->GetType() == EGOT_Player )
	{
		list<Arrow*>::iterator itr = find(g_level.m_lstArrows.begin(), g_level.m_lstArrows.end(), this);
		g_level.m_lstArrows.erase(itr);
		delete this;
	}
}

VOID Arrow::Update( FLOAT dt )
{
	Movable::Update(dt);
}

VOID Arrow::UpdatePhysic( FLOAT dt )
{
	Vector2F vAcc = XGravity;

	m_vVel += vAcc*dt;
	if( m_bLand )
	{
		m_vVel = Vector2F(0, 0);
	}

	Movable::UpdatePhysic(dt);
}

VOID Movable::Update( float dt )
{
	GameObj::Update(dt);
	HandleInput();
	UpdatePhysic(dt);
}

VOID tagPhysic::UpdatePhysic( FLOAT dt )
{
	Vector2F vOffset = m_vVel * dt;
	m_pMover->SetPos(m_pMover->GetPos()+vOffset);
	m_bLand = false;
}
