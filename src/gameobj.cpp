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

VOID AIListener::Listen( tagCtrlData &input )
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

VOID GameObj::NormalCollide( GameObj* pRunner, tagCollideRes* pRes )
{
	Square thisBox = GetAABBox();
	Square thatBox = pRunner->GetAABBox();

	pRes->vCollidePos = pRunner->GetPos();

	FLOAT fDeep = 0.0f;
	pRes->dwDirFlag = thisBox.IntersectTest(&thatBox, fDeep, GetCollideDirFlag());

	if( pRes->dwDirFlag & ECD_Top )
	{
		pRes->vCollidePos.y += fDeep;
	}
	if( pRes->dwDirFlag & ECD_Down )
	{
		pRes->vCollidePos.y -= fDeep;
	}
	if( pRes->dwDirFlag & ECD_Left )
	{
		pRes->vCollidePos.x -= fDeep;
	}
	if( pRes->dwDirFlag & ECD_Right )
	{
		pRes->vCollidePos.x += fDeep;
	}
	
}

Square GameObj::GetAABBox() const
{
	return m_pSprite->GetAABBox(m_vPos);
}

VOID GameObj::SetColor( UINT8 u8R, UINT8 u8G, UINT8 u8B )
{
	m_pSprite->SetColor(u8R, u8G, u8B);
}

Vector2F GameObj::GetSize() const
{
	return m_pSprite->GetSize();
}

BOOL GameObj::Init( UINT32 uCollideDirFlag )
{
	m_vPrePos = m_vPos = Vector2F(0.0f, 0.0f);
	m_uCollideDirFlag = uCollideDirFlag;
	
	m_bDelete = FALSE;
	return TRUE;
}

VOID GameObj::Destroy()
{
	delete m_pSprite;
	m_pSprite = NULL;
}

VOID GameObj::BulletCollide( GameObj* pRunner, tagCollideRes* pRes )
{
	Square thisBox = this->GetAABBox();
	Square thatBox = pRunner->GetAABBox();

	pRes->vCollidePos = pRunner->GetPos();

	Vector2F vD = pRunner->GetPos() - pRunner->GetPrePos();

	FLOAT fT = thisBox.IntersectMovingAABB(thatBox, vD, pRes->dwDirFlag);
	if( fT >= 0.0f && fT < 1.0f )
	{
		pRes->vCollidePos = this->GetPrePos() + vD * fT;
	}
	else
	{
		pRes->vCollidePos = this->GetPos();
	}
}

//-----------------------------------------------------------------------------------------------


BOOL Terrain::Init()
{
	if( !GameObj::Init(ECD_All) )
	{
		return FALSE;
	}

	m_pSprite = new SpriteTerrain(this);

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
	
	m_vArrowDir = g_keyboard.m_vMousePos - GetPos();
	m_vArrowDir /= m_vArrowDir.Length();

	if( m_Input.bFire )
	{
		Arrow* pArrow = new Arrow;
		pArrow->Init(this);
		pArrow->SetPos(GetPos());
		
		pArrow->m_vVel.x = XMaxArrowSpeed*m_vArrowDir.x;
		pArrow->m_vVel.y = XMaxArrowSpeed*m_vArrowDir.y;

		g_level.m_lstArrows.push_back(pArrow);
	}
	if( m_Input.bConstruct )
	{
		INT nIdxX = GetPos().x / XTerrainSize;
		INT nIdxY = GetPos().y / XTerrainSize;

		g_level.m_matTerrain[nIdxX][nIdxY].bExist = true;

	}
}


VOID Player::UpdatePhysic( float dt )
{
	Vector2F vAcc = XGravity;
	bool bNewAction = false;

// 	if( m_pMover->m_Input.bZoomIn )
// 	{
// 		m_pMover->GetSprite()->SetSizeFactor(m_pMover->GetSprite()->GetSizeFactor() - 0.1f);
// 	}
// 
// 	if( m_pMover->m_Input.bZoomOut )
// 	{
// 		m_pMover->GetSprite()->SetSizeFactor(m_pMover->GetSprite()->GetSizeFactor() + 0.1f);
// 	}

	if( m_pMover->m_Input.bLeft )
	{
		vAcc.x = -XCtrlAcc;
		m_bFaceRight = FALSE;
		bNewAction = true;
	}

	if( m_pMover->m_Input.bRight )
	{
		vAcc.x = XCtrlAcc;
		m_bFaceRight = TRUE;
		bNewAction = true;
	}

	if( m_bLand )
	{
		vAcc.y = 0;
	}

	if( vAcc.x == 0.0f )
	{
		FLOAT fFraction = dt * XCtrlAcc;
		if( fabs(m_vVel.x) > fFraction )
		{
			vAcc.x = (m_vVel.x > 0 ? -XCtrlAcc : XCtrlAcc);
		}
		else
		{
			vAcc.x = 0.0f;
			m_vVel.x = 0;
			bNewAction = true;
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
				bNewAction = true;
			}

			m_fJump += dt;
			if( m_fJump >= 0.5f )
			{
				m_vVel.y = 0;
				m_fJump = -1.0f;
				bNewAction = true;
			}

			m_bJmpPressed = TRUE;
		}
		else
		{
			m_vVel.y = 0;
			m_fJump = -1.0f;
			bNewAction = true;

			m_bJmpPressed = FALSE;
		}
	}

	const char* pType = NULL;
	if( m_bLand )
	{
		if( vAcc.x != 0.0f )
		{
			pType = "run";
		}
		else
		{
			pType = "stand";
		}
	}
	else
	{
		if( m_fJump >= 0.0F && m_bJmpPressed )
		{
			pType = "jump";
		}
		else
		{
			pType = "fall";
		}
	}

	if( bNewAction )
	{
		static_cast<SpritePlayer*>(m_pSprite)->Start(pType, m_bFaceRight);
	}
	

	Movable::UpdatePhysic(dt);
}

BOOL Player::Init()
{
	if( !Movable::Init(ECD_None) ) return FALSE;

	m_pListener = new KeyBoardListener(this);
	m_pSprite = new SpritePlayer(this);

	m_fJump = 0.0f;
	m_bJmpPressed = FALSE;

	return TRUE;
}

VOID Player::Destroy()
{
	Movable::Destroy();
}

VOID Player::CheckTouch( Terrain* pTerrain, tagCollideRes* pRes )
{
	pTerrain->NormalCollide(this, pRes);
	if( !pRes->dwDirFlag ) return;

	if( pRes->dwDirFlag & ECD_Top )
	{
		if( this->m_vVel.y < 0 )
		{
			this->m_vVel.y *= 0;
		}
		this->m_bLand = TRUE;
		this->m_fJump = 0.0f;
	}
	if( pRes->dwDirFlag & ECD_Down )
	{
		if( this->m_vVel.y > 0 )
		{
			this->m_vVel.y *= -1;
		}
	}
	if( pRes->dwDirFlag & ECD_Left )
	{
		if( this->m_vVel.x > 0 )
		{
			this->m_vVel.x *= 0;
		}
		this->m_bHitWall = TRUE;
	}
	if( pRes->dwDirFlag & ECD_Right )
	{
		if( this->m_vVel.x < 0 )
		{
			this->m_vVel.x *= 0;
		}
		this->m_bHitWall = TRUE;
	}

	this->SetPos(pRes->vCollidePos);
}

VOID Player::CheckTouch( Arrow* pArrow, tagCollideRes* pRes )
{
	pArrow->NormalCollide(this, pRes);
	if( !pRes->dwDirFlag ) return;

	if( pArrow->m_bLand || pArrow->m_bHitWall )
	{
		pArrow->SetDelete();
	}
	else if( this != pArrow->m_pOwner )
	{
		this->SetDelete();
	}

}

VOID Player::CheckTouch( Animal* pAnimal, tagCollideRes* pRes )
{

}

//-----------------------------------------------------------------------------------------------

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
	if( m_bLand || m_bHitWall )
	{
		m_vVel.y = 0;
		m_vVel.x = 0;
	}
	Movable::UpdatePhysic(dt);
}

BOOL Arrow::Init( Player* pOwner )
{
	if( !Movable::Init(ECD_All) ) return FALSE;

	m_pListener = new DummyListener(this);
	m_pSprite = new SpriteArrow(this);

	m_pOwner = pOwner;

	return TRUE;
}

VOID Arrow::Destroy()
{
	m_pOwner = NULL;
	Movable::Destroy();
}

VOID Arrow::CheckTouch( Terrain* pTerrain, tagCollideRes* pRes )
{
	pTerrain->BulletCollide(this, pRes);
	if( !pRes->dwDirFlag ) return;

	this->m_vVel = Vector2F(0, 0);
	if( pRes->dwDirFlag & (ECD_Left | ECD_Right))
	{
		this->m_bHitWall = TRUE;	
	}
	else if( pRes->dwDirFlag & ECD_Top )
	{
		this->m_bLand = TRUE;
	}
}

//-----------------------------------------------------------------------------------------------

VOID Movable::Update( float dt )
{
	GameObj::Update(dt);
	m_pListener->Listen(m_Input);
	UpdatePhysic(dt);
}

BOOL Movable::Init( DWORD dwDirFlag )
{
	if( !GameObj::Init(dwDirFlag) ) return FALSE;

	if( !tagPhysic::Init(this) ) return FALSE;

	m_Input.u16Data = 0;

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
		FLOAT fFraction = dt * XCtrlAcc;
		if( fabs(m_vVel.x) > fFraction )
		{
			vAcc.x = (m_vVel.x > 0 ? -XCtrlAcc : XCtrlAcc);
		}
		else
		{
			m_vVel.x = 0;
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


BOOL Animal::Init()
{
	if( !Movable::Init(ECD_All) ) return FALSE;

	m_pListener = new AIListener(this);
	m_pSprite = new SpriteAnimal(this);

	return TRUE;
}

VOID Animal::Destroy()
{
	Movable::Destroy();
}

VOID Animal::CheckTouch( Terrain* pTerrain, tagCollideRes* pRes )
{
	pTerrain->NormalCollide(this, pRes);
	if( !pRes->dwDirFlag ) return;

	Vector2F vPos = pRes->vCollidePos;

	if( pRes->dwDirFlag & ECD_Top )
	{
		if( this->m_vVel.y < 0 )
		{
			this->m_vVel.y *= 0;
		}
		vPos.x = this->GetPos().x;
		this->m_bLand = TRUE;
		this->m_fJump = 0.0f;
	}
	if( pRes->dwDirFlag & ECD_Down )
	{
		vPos.x = this->GetPos().x;
		if( this->m_vVel.y > 0 )
		{
			this->m_vVel.y *= -1;
		}
	}
	if( pRes->dwDirFlag & ECD_Left )
	{
		vPos.y = this->GetPos().y;
		if( this->m_vVel.x > 0 )
		{
			this->m_vVel.x *= 0;
		}
		this->m_bHitWall = TRUE;
	}
	if( pRes->dwDirFlag & ECD_Right )
	{
		vPos.y = this->GetPos().y;
		if( this->m_vVel.x < 0 )
		{
			this->m_vVel.x *= 0;
		}
		this->m_bHitWall = TRUE;
	}

	this->SetPos(vPos);
}

VOID Animal::CheckTouch( Arrow* pArrow, tagCollideRes* pRes )
{
	pArrow->NormalCollide(this, pRes);
	if( !pRes->dwDirFlag ) return;


	if(pArrow-> m_bLand | pArrow->m_bHitWall )
	{
		pArrow->SetDelete();
	}
	else
	{
		SetDelete();
	}
}
