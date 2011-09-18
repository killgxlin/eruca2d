#include "common.h"
#include "level.h"

#include "gameobj.h"

BOOL Level::Init()
{
	m_nRefreshTimes = 0;

	m_lstBlocks.clear();

	m_pPlayer = new Player;
	m_pPlayer->SetPos(Vector2F(XPlayerSize*2 + XPlayerSize/2, XPlayerSize*2 + XPlayerSize/2));
	m_pPlayer->m_vVel = Vector2F(0, 0);

	m_vLastIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	RefreshBlocks(m_vLastIdx);

	return TRUE;
}

VOID Level::Destroy()
{
	while( !m_lstBlocks.empty() )
	{
		m_lstBlocks.front()->UnLoad();
		delete m_lstBlocks.front();

		m_lstBlocks.pop_front();
	}

	delete m_pPlayer;
	m_pPlayer = NULL;

}

struct BlockUpdate
{
	BlockUpdate(float dt):m_fDt(dt){}
	VOID	operator()(tagBlock* pBlock)
	{
		pBlock->Update(m_fDt);
	}

	float	m_fDt;
};

struct BlockCollide
{
	BlockCollide(Movable* pMover):m_pMover(pMover){}
	VOID	operator()(tagBlock* pBlock)
	{
		pBlock->Collide(m_pMover);
	}
	Movable*	m_pMover;
};

struct ArrowUpdate
{
	ArrowUpdate(float dt):m_fDt(dt){}
	VOID	operator()(Arrow* pArrow)
	{
		pArrow->Update(m_fDt);
	}

	float	m_fDt;
};


VOID Level::Update( FLOAT dt )
{
	dt *= g_framerate.GetSpeedRate();

	ForEachBlock(BlockUpdate(dt));
	m_pPlayer->Update(dt);
	for_each(m_lstArrows.begin(), m_lstArrows.end(), ArrowUpdate(dt));

	ForEachBlock(BlockCollide(m_pPlayer));
	for(list<Arrow*>::iterator itr = m_lstArrows.begin();
		itr != m_lstArrows.end();
		++itr)
	{
		ForEachBlock(BlockCollide(*itr));
	}
	
	

	Vector2N vNewIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	if( m_vLastIdx != vNewIdx )
	{
		RefreshBlocks(vNewIdx);
		m_vLastIdx = vNewIdx;
	}

	g_painter.SetCenter(m_pPlayer->GetPos());

	Vector2N vIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "player on land :%4d", m_pPlayer->m_bLand);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "block of player:%4d ,%4d", vIdx.x, vIdx.y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "pos of player  :%4.2f ,%4.2f", m_pPlayer->GetPos().x, m_pPlayer->GetPos().y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "vel of player  :%4.2f ,%4.2f, %4.2f", m_pPlayer->m_vVel.x, m_pPlayer->m_vVel.y, m_pPlayer->m_vVel.Length());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "refresh        : %4d", g_level.m_nRefreshTimes);

}

struct BlockDraw
{
	BlockDraw(Painter* pPainter):m_pPainter(pPainter){}
	VOID	operator()(tagBlock* pBlock)
	{
		pBlock->Draw(m_pPainter);
	}

	Painter*	m_pPainter;
};

struct ArrowDraw
{
	ArrowDraw(Painter* pPainter):m_pPainter(pPainter){}
	Painter*	m_pPainter;
	VOID	operator()(Arrow* pArrow)
	{
		pArrow->Draw(m_pPainter);
	}
};

VOID Level::Draw( Painter* pPainter )
{
	ForEachBlock(BlockDraw(pPainter));
	m_pPlayer->Draw(pPainter);
	for_each(m_lstArrows.begin(), m_lstArrows.end(), ArrowDraw(pPainter));
}

VOID Level::RefreshBlocks( const Vector2N &vIdx )
{
	++m_nRefreshTimes;

	BOOL	bMat[3][3];
	memset(bMat, 0, sizeof(bMat));
	memset(m_matBlocks, 0, sizeof(m_matBlocks));

	list<tagBlock*>	lstNeedDel;

	for(list<tagBlock*>::iterator itr = m_lstBlocks.begin();
		itr != m_lstBlocks.end();
		++itr)
	{
		if( IsBetweenClose((*itr)->vIdx.x, vIdx.x-1, vIdx.x+1) && IsBetweenClose((*itr)->vIdx.y, vIdx.y-1, vIdx.y+1) )
		{
			Vector2N vOffset = (*itr)->vIdx - vIdx;
			bMat[vOffset.x+1][vOffset.y+1] = TRUE;
			m_matBlocks[vOffset.x+1][vOffset.y+1] = (*itr);
		}
		else
		{
			lstNeedDel.push_back(*itr);
		}
	}

	while( !lstNeedDel.empty() )
	{
		list<tagBlock*>::iterator res_itr = std::find(m_lstBlocks.begin(), m_lstBlocks.end(), lstNeedDel.front());
		if( res_itr != m_lstBlocks.end() )
		{
			m_lstBlocks.erase(res_itr);
			lstNeedDel.front()->UnLoad();
			delete lstNeedDel.front();

		}

		lstNeedDel.pop_front();
	}

	for( INT i=0; i<3; ++i )
	{
		for( INT j=0; j<3; ++j )
		{
			if( !bMat[i][j] )
			{
				Vector2N vOffset(i-1, j-1);
				tagBlock* pNew = new tagBlock;
				pNew->Load(vIdx, vOffset);
				m_lstBlocks.push_back(pNew);
				m_nNewXIdx = vIdx.x - 1;
				m_matBlocks[i][j] = pNew;
			}
		}
	}
	
}

Vector2N Level::ConvertToBlockIdx( const Vector2F &vPos )
{
	Vector2N vIdx(INT(vPos.x / XScreenW), INT(vPos.y / XScreenH));

	if( vPos.x < 0.0f )
	{
		vIdx.x -= 1;
	}
	if( vPos.y < 0.0f )
	{
		vIdx.y -= 1;
	}

	return vIdx;
}

Level g_level;

BOOL tagBlock::Load( const Vector2N &vCenterIdx, const Vector2N &vOffset )
{
	vIdx = vCenterIdx + vOffset;

	Vector2F vOri(FLOAT(vIdx.x * XScreenW), FLOAT(vIdx.y * XScreenH));

	for( FLOAT f=XTerrainSize/2; f<=XScreenW - XTerrainSize/2; f+=XTerrainSize )
	{
		Terrain* pNew = new Terrain;
		pNew->SetPos(vOri + Vector2F(f, XTerrainSize/2));
		pNew->SetCollideDirFlag(ECD_Top);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		vecTerrains.push_back(pNew);
	}

//	for( FLOAT f=XTerrainSize/2; f<=XScreenH - XTerrainSize/2; f+=XTerrainSize )
	{
		Terrain* pNew = new Terrain;
		pNew->SetPos(vOri + Vector2F(XTerrainSize/2, XTerrainSize*2/*f*/));
		pNew->SetCollideDirFlag(ECD_All);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		vecTerrains.push_back(pNew);
	}

	return TRUE;
}

struct TerrainDelete
{
	void operator()(Terrain* pTerrain)
	{
		delete pTerrain;
	}
};

VOID tagBlock::UnLoad()
{
	for_each(vecTerrains.begin(), vecTerrains.end(), TerrainDelete());

	vecTerrains.clear();
}

struct TerrainDraw
{
	TerrainDraw(Painter* pPainter):m_pPainter(pPainter){}
	Painter*	m_pPainter;
	VOID operator()(Terrain* pTerrain)
	{
		pTerrain->Draw(m_pPainter);
	}
};

VOID tagBlock::Draw( Painter* pPainter )
{
	for_each(vecTerrains.begin(), vecTerrains.end(), TerrainDraw(pPainter));

	Vector2F vCenter(FLOAT(vIdx.x*XScreenW), FLOAT(vIdx.y*XScreenH));
	vCenter += Vector2F(XScreenW/2, XScreenH/2);

	pPainter->WorldDrawText(vCenter, pPainter->GetColor(255, 0, 0),"%d, %d", vIdx.x, vIdx.y );
}

struct TerrainUpdate
{
	TerrainUpdate(FLOAT dt):m_dt(dt){}
	VOID operator()(Terrain* pTerrain)
	{
		pTerrain->m_fDist = -1.0f;
		pTerrain->Update(m_dt);
	}
	FLOAT m_dt;
};

VOID tagBlock::Update( float dt )
{
	for_each(vecTerrains.begin(), vecTerrains.end(), TerrainUpdate(dt));
}


struct TerrainCollide
{
	TerrainCollide(Movable* pMover, tagCollideRes* pResult):m_pMover(pMover), m_pResult(pResult){}
	VOID operator()(Terrain* pTerrain)
	{
		pTerrain->Collide(m_pMover, m_pResult);
	}
	Movable* m_pMover;
	tagCollideRes* m_pResult;
};

struct TerrainCmp
{
	TerrainCmp(GameObj* pObj):m_pObj(pObj){}
	bool	operator()(Terrain* pLhs, Terrain* pRhs)
	{
		if( pLhs->m_fDist < 0.0f )
		{
			pLhs->m_fDist = (m_pObj->GetPrePos() - pLhs->GetPrePos()).Length2();
		}
		if( pRhs->m_fDist < 0.0f )
		{
			pRhs->m_fDist = (m_pObj->GetPrePos() - pRhs->GetPrePos()).Length2();
		}

		return pLhs->m_fDist < pRhs->m_fDist;
	}

	GameObj*	m_pObj;
};

VOID tagBlock::Collide( Movable* pMover )
{
	std::sort(vecTerrains.begin(), vecTerrains.end(), TerrainCmp(pMover));

	tagCollideRes result;
	
	for_each(vecTerrains.begin(), vecTerrains.end(), TerrainCollide(pMover, &result));
}
