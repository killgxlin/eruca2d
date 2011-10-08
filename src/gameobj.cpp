#include "common.h"
#include "gameobj.h"
#include "level.h"

#include "sprite.h"

//-----------------------------------------------------------------------------------------------
VOID tagPhysic::UpdatePhysic( FLOAT dt )
{
	Vector2F vOffset = m_vVel * dt;
	FLOAT fLen = vOffset.Length();
	if( fLen >= XTerrainSize )
	{
		vOffset /= fLen;
	}
	m_pMover->SetPos(m_pMover->GetPos()+vOffset);
	m_bHitWall = FALSE;
	m_bLand = FALSE;
	if( m_pMover->m_Input.bLeft )
	{
		m_bFaceRight = FALSE;
	}
	else if( m_pMover->m_Input.bRight )
	{
		m_bFaceRight = TRUE;
	}
}

BOOL tagPhysic::Init( Movable* pMover )
{
	m_pMover = pMover;
	m_vVel = Vector2F(0.0f, 0.0f);
	m_bLand = FALSE;
	m_bHitWall = FALSE;
	m_bFaceRight = TRUE;

	return TRUE;
}

VOID tagPhysic::Destroy()
{
	m_pMover = NULL;
}
//-----------------------------------------------------------------------------------------------

VOID AIListener::Listen( tagInputMap &input )
{
	InputListener::Listen(input);

	Player* pPlayer = g_level.m_lstPlayers.front();
	
	Vector2F vDir = pPlayer->GetPos() - m_pMover->GetPos();

	if( vDir.x > 0 )
	{
		input.bRight = TRUE;
	}
	if( vDir.x < 0 )
	{
		input.bLeft = TRUE;
	}

	if( m_pMover->m_bHitWall )
	{
		input.bJump = TRUE;
	}
	else
	{
		input.bJump = FALSE;
	}

}

//-----------------------------------------------------------------------------------------------

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

Vector2F GameObj::GetSize() const
{
	return m_pSprite->GetSize();
}

BOOL GameObj::Init( const Vector2F &vPos, UINT32 uCollideDirFlag, EGameObjType eGOT )
{
	m_vPos = vPos;
	m_uCollideDirFlag = uCollideDirFlag;
	m_eGot = eGOT;
	m_bDelete = FALSE;
	switch( eGOT )
	{
	case EGOT_Player:
		m_pSprite = new SpritePlayer(this);
		break;
	case EGOT_Arrow:
		m_pSprite = new SpriteArrow(this);
		break;
	case EGOT_Terrain:
		m_pSprite = new SpriteTerrain(this);
		break;
	case EGOT_Animal:
		m_pSprite = new SpriteAnimal(this);
		break;
	default:
		return FALSE;
		break;

	}
	return TRUE;
}

VOID GameObj::Destroy()
{
	delete m_pSprite;
	m_pSprite = NULL;
}

//-----------------------------------------------------------------------------------------------


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
				pPlayer->m_bLand = TRUE;
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
				pPlayer->m_bHitWall = TRUE;
			}
			if( pRes->dwDirFlag & ECD_Right )
			{
				pRes->vCollidePos.y = pPlayer->GetPos().y;
				if( pPlayer->m_vVel.x < 0 )
				{
					pPlayer->m_vVel.x *= 0;
				}
				pPlayer->m_bHitWall = TRUE;
			}

			pPlayer->SetPos(pRes->vCollidePos);
			SetColor(255, 255, 255);
		}
		break;
	case EGOT_Arrow:
		{
			Arrow* pArrow = dynamic_cast<Arrow *>(pMover);
			pArrow->m_vVel = Vector2F(0, 0);
			SetColor(255, 255, 255);
			if( pRes->dwDirFlag & (ECD_Left | ECD_Right))
			{
				pArrow->m_bHitWall = TRUE;	
			}
			else
			{
				pArrow->m_bLand = TRUE;
			}
		}
		break;
	case EGOT_Animal:
		{
			Animal *pAnimal = dynamic_cast<Animal *>(pMover);
			if( pRes->dwDirFlag & ECD_Top )
			{
				if( pAnimal->m_vVel.y < 0 )
				{
					pAnimal->m_vVel.y *= 0;
				}
				pRes->vCollidePos.x = pAnimal->GetPos().x;
				pAnimal->m_bLand = TRUE;
				pAnimal->m_fJump = 0.0f;
			}
			if( pRes->dwDirFlag & ECD_Down )
			{
				pRes->vCollidePos.x = pAnimal->GetPos().x;
				if( pAnimal->m_vVel.y > 0 )
				{
					pAnimal->m_vVel.y *= -1;
				}
			}
			if( pRes->dwDirFlag & ECD_Left )
			{
				pRes->vCollidePos.y = pAnimal->GetPos().y;
				if( pAnimal->m_vVel.x > 0 )
				{
					pAnimal->m_vVel.x *= 0;
				}
				pAnimal->m_bHitWall = TRUE;
			}
			if( pRes->dwDirFlag & ECD_Right )
			{
				pRes->vCollidePos.y = pAnimal->GetPos().y;
				if( pAnimal->m_vVel.x < 0 )
				{
					pAnimal->m_vVel.x *= 0;
				}
				pAnimal->m_bHitWall = TRUE;
			}

			pAnimal->SetPos(pRes->vCollidePos);
			SetColor(255, 255, 255);
		}
		break;
	}
}

BOOL Terrain::Init()
{
	if( !GameObj::Init(Vector2F(0.0f, 0.0f), ECD_All, EGOT_Terrain) )
	{
		return FALSE;
	}
	return TRUE;
}

VOID Terrain::Destroy()
{
	GameObj::Destroy();
}

//-----------------------------------------------------------------------------------------------

VOID Player::Update( FLOAT dt )
{

	Movable::Update(dt);
	
	if( m_Input.bFire )
	{
		Arrow* pArrow = new Arrow;
		pArrow->Init(this);
		pArrow->SetPos(GetPos());
		
		pArrow->m_vVel.x = XMaxArrowSpeed*2*cos(m_fBear);
		pArrow->m_vVel.y = XMaxArrowSpeed*2*sin(m_fBear);
		pArrow->m_vVel.x *= (m_bFaceRight ? 1 : -1);
		g_level.m_lstArrows.push_back(pArrow);
	}
	if( m_Input.bConstruct )
	{
		INT nIdxX = GetPos().x / XTerrainSize;
		INT nIdxY = GetPos().y / XTerrainSize;

		g_level.m_matTerrain[nIdxX][nIdxY] = 1;
	}
	if( m_Input.bUp )
	{
		m_fBear += 0.01f;
	}
	if( m_Input.bDown )
	{
		m_fBear -= 0.01f;
	}
	m_fBear = Cut(m_fBear, -1.57f, 1.57f);
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
				m_bLand = FALSE;	
			}

			m_fJump += dt;
			if( m_fJump >= 0.5f )
			{
				m_vVel.y = 0;
				m_fJump = -1.0f;
			}

			m_bJmpPressed = TRUE;
		}
		else
		{
			m_vVel.y = 0;
			m_fJump = -1.0f;

			m_bJmpPressed = FALSE;
		}
	}

	if( m_bLand )
	{
		m_vVel.y = 0;
	}

	Movable::UpdatePhysic(dt);
}

BOOL Player::Init()
{
	if( !Movable::Init(Vector2F(0, 0), ECD_None, EGOT_Player) ) return FALSE;
	m_fJump = 0.0f;
	m_bJmpPressed = FALSE;
	m_fBear = 0.0f;

	return TRUE;
}

VOID Player::Destroy()
{
	Movable::Destroy();
}

//-----------------------------------------------------------------------------------------------

VOID Arrow::Collide( GameObj* pRunner, tagCollideRes* pRes )
{
	GameObj::Collide(pRunner, pRes);
	if( !pRes->dwDirFlag ) return;

	switch( pRunner->GetType() )
	{
	case EGOT_Player:
		{
			if( m_bLand | m_bHitWall )
			{
				SetDelete();
			}
			if( pRunner != m_pOwner )
			{
				pRunner->SetDelete();
			}
		}
		break;
	case EGOT_Animal:
		{
			if( m_bLand | m_bHitWall )
			{
				SetDelete();
			}
			else
			{
				pRunner->SetDelete();
			}
		}
		break;
	}
}

VOID Arrow::Update( FLOAT dt )
{
	Movable::Update(dt);
	if( m_vVel != Vector2F(0.0f ,0.0f) )
	{
		m_vLastVel = m_vVel;
	}
}

VOID Arrow::UpdatePhysic( FLOAT dt )
{
	Vector2F vAcc = XGravity;

	m_vVel += vAcc*dt;
	if( m_bLand | m_bHitWall )
	{
		m_vVel = Vector2F(0, 0);
	}

	Movable::UpdatePhysic(dt);
}

BOOL Arrow::Init( Player* pOwner )
{
	if( !Movable::Init(Vector2F(0, 0), ECD_All, EGOT_Arrow) ) return FALSE;

	m_pOwner = pOwner;

	return TRUE;
}

VOID Arrow::Destroy()
{
	m_pOwner = NULL;
	Movable::Destroy();
}

//-----------------------------------------------------------------------------------------------

VOID Movable::Update( float dt )
{
	GameObj::Update(dt);
	m_pListener->Listen(m_Input);
	UpdatePhysic(dt);
}

BOOL Movable::Init( const Vector2F &vPos, DWORD dwDirFlag, EGameObjType eGOT )
{
	if( !GameObj::Init(vPos, dwDirFlag, eGOT) ) return FALSE;

	if( !tagPhysic::Init(this) ) return FALSE;

	m_Input.u16Data = 0;

	switch( eGOT )
	{
	case EGOT_Player:
		m_pListener = new KeyBoardListener;
		break;
	case EGOT_Arrow:
		m_pListener = new DummyListener;
		break;
	case EGOT_Terrain:
		m_pListener = new DummyListener;
		break;
	case EGOT_Animal:
		m_pListener = new AIListener;
		break;
	default:
		return FALSE;
		break;

	}
	m_pListener->m_pMover = this;

	return TRUE;
}

VOID Movable::Destroy()
{
	delete m_pListener;
	m_pListener = NULL;

	tagPhysic::Destroy();
	GameObj::Destroy();
}

//-----------------------------------------------------------------------------------------------

VOID Animal::UpdatePhysic( FLOAT dt )
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
				m_bLand = FALSE;	
			}

			m_fJump += dt;
			if( m_fJump >= 0.5f )
			{
				m_vVel.y = 0;
				m_fJump = -1.0f;
			}

			m_bJmpPressed = TRUE;
		}
		else
		{
			m_vVel.y = 0;
			m_fJump = -1.0f;

			m_bJmpPressed = FALSE;
		}
	}

	if( m_bLand )
	{
		m_vVel.y = 0;
	}

	Movable::UpdatePhysic(dt);
}

VOID Animal::Update( FLOAT dt )
{

	Movable::Update(dt);
}

VOID Animal::Collide( GameObj* pRunner, tagCollideRes* pRes )
{

}

BOOL Animal::Init()
{
	if( !Movable::Init(Vector2F(0.0f, 0.0f), ECD_All, EGOT_Animal) ) return FALSE;

	return TRUE;
}

VOID Animal::Destroy()
{
	Movable::Destroy();
}
