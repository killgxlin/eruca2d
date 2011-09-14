#include "common.h"
#include "level.h"

#include "gameobj.h"

BOOL Level::Init()
{
	m_nRefreshTimes = 0;

	m_lstBlocks.clear();

	m_pPlayer = new Player;
	m_pPlayer->SetPos(Vector2F(XPlayerSize*2 + XPlayerSize/2, XPlayerSize*2 + XPlayerSize/2));
	m_pPlayer->m_Move.m_vVel = Vector2F(0, 0);

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
	BlockCollide(GameObj* pObj):m_pObj(pObj){}
	VOID	operator()(tagBlock* pBlock)
	{
		pBlock->Collide(m_pObj);
	}
	GameObj*	m_pObj;
};

VOID Level::Update( FLOAT dt )
{
	dt *= g_framerate.GetSpeedRate();

	ForEachBlock(BlockUpdate(dt));
	m_pPlayer->Update(dt);

	ForEachBlock(BlockCollide(m_pPlayer));

	Vector2N vNewIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	if( m_vLastIdx != vNewIdx )
	{
		RefreshBlocks(vNewIdx);
		m_vLastIdx = vNewIdx;
	}

	g_painter.SetCenter(m_pPlayer->GetPos());

	Vector2N vIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "player on land :%4d", m_pPlayer->m_Move.m_bLand);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "block of player:%4d ,%4d", vIdx.x, vIdx.y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "pos of player  :%4.2f ,%4.2f", m_pPlayer->GetPos().x, m_pPlayer->GetPos().y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "vel of player  :%4.2f ,%4.2f, %4.2f", m_pPlayer->m_Move.m_vVel.x, m_pPlayer->m_Move.m_vVel.y, m_pPlayer->m_Move.m_vVel.Length());
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

VOID Level::Draw( Painter* pPainter )
{
	ForEachBlock(BlockDraw(pPainter));
	m_pPlayer->Draw(pPainter);
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

	for( FLOAT f=XTileSize/2; f<=XScreenW - XTileSize/2; f+=XTileSize )
	{
		Tile* pNew = new Tile;
		pNew->SetPos(vOri + Vector2F(f, XTileSize/2));
		pNew->SetCollideDirFlag(ECD_Top);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		vecTiles.push_back(pNew);
	}

//	for( FLOAT f=XTileSize/2; f<=XScreenH - XTileSize/2; f+=XTileSize )
	{
		Tile* pNew = new Tile;
		pNew->SetPos(vOri + Vector2F(XTileSize/2, XTileSize*2/*f*/));
		pNew->SetCollideDirFlag(ECD_All);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		vecTiles.push_back(pNew);
	}

	return TRUE;
}

struct TileDelete
{
	void operator()(Tile* pTile)
	{
		delete pTile;
	}
};

VOID tagBlock::UnLoad()
{
	for_each(vecTiles.begin(), vecTiles.end(), TileDelete());

	vecTiles.clear();
}

struct TileDraw
{
	TileDraw(Painter* pPainter):m_pPainter(pPainter){}
	Painter*	m_pPainter;
	VOID operator()(Tile* pTile)
	{
		pTile->Draw(m_pPainter);
	}
};

VOID tagBlock::Draw( Painter* pPainter )
{
	for_each(vecTiles.begin(), vecTiles.end(), TileDraw(pPainter));

	Vector2F vCenter(FLOAT(vIdx.x*XScreenW), FLOAT(vIdx.y*XScreenH));
	vCenter += Vector2F(XScreenW/2, XScreenH/2);

	pPainter->WorldDrawText(vCenter, pPainter->GetColor(255, 0, 0),"%d, %d", vIdx.x, vIdx.y );
}

struct TileUpdate
{
	TileUpdate(FLOAT dt):m_dt(dt){}
	VOID operator()(Tile* pTile)
	{
		pTile->m_fDist = -1.0f;
		pTile->Update(m_dt);
	}
	FLOAT m_dt;
};

VOID tagBlock::Update( float dt )
{
	for_each(vecTiles.begin(), vecTiles.end(), TileUpdate(dt));
}


struct TileCollide
{
	TileCollide(GameObj* pObj, tagCollideRes* pResult):m_pObj(pObj), m_pResult(pResult){}
	VOID operator()(Tile* pTile)
	{
		pTile->Collide(m_pObj, m_pResult);
	}
	GameObj* m_pObj;
	tagCollideRes* m_pResult;
};

struct TileCmp
{
	TileCmp(GameObj* pObj):m_pObj(pObj){}
	bool	operator()(Tile* pLhs, Tile* pRhs)
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

VOID tagBlock::Collide( GameObj* pObj )
{
	std::sort(vecTiles.begin(), vecTiles.end(), TileCmp(pObj));

	tagCollideRes result;
	
	for_each(vecTiles.begin(), vecTiles.end(), TileCollide(pObj, &result));
}
