#include "common.h"
#include "level.h"

#include "gameobj.h"
#include "painter.h"
#include "text.h"
#include "timer.h"

BOOL Level::Init()
{
	m_nRefreshTimes = 0;

	m_lstBlocks.clear();
	m_lstTiles.clear();

	m_pPlayer = new Player;
	m_pPlayer->SetPos(Vector2F(XPlayerSize + XPlayerSize/2, XPlayerSize + XPlayerSize/2));

	m_collider.AddGameObj(m_pPlayer);

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

	assert( m_lstTiles.empty() );

	m_collider.DelGameObj(m_pPlayer);
	delete m_pPlayer;
	m_pPlayer = NULL;

}

struct TileUpdate
{
	TileUpdate(FLOAT dt):m_dt(dt){}
	VOID operator()(Tile* pTile)
	{
		pTile->Update(m_dt);
	}
	FLOAT m_dt;
};

VOID Level::Update( FLOAT dt )
{
	dt *= g_framerate.GetSpeedRate();

	for_each(m_lstTiles.begin(), m_lstTiles.end(), TileUpdate(dt));
	m_pPlayer->Update(dt);

	m_collider.Collide();

	Vector2N vNewIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	if( m_vLastIdx != vNewIdx )
	{
		RefreshBlocks(vNewIdx);
		m_vLastIdx = vNewIdx;
	}

	Vector2N vIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "block of player:%4d ,%4d", vIdx.x, vIdx.y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "vel of player:%4.2f ,%4.2f, %4.2f", m_pPlayer->m_vVel.x, m_pPlayer->m_vVel.y, m_pPlayer->m_vVel.Length());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "refresh  : %4d", g_level.m_nRefreshTimes);

}

template<typename T>
struct FuncDraw
{
	FuncDraw(Painter* pPainter):m_pPainter(pPainter){}
	Painter*	m_pPainter;
	VOID operator()(T* pTile)
	{
		pTile->Draw(m_pPainter);
	}
};

VOID Level::Draw( Painter* pPainter )
{
	for_each(m_lstTiles.begin(), m_lstTiles.end(), FuncDraw<Tile>(pPainter));
	m_pPlayer->Draw(pPainter);

	for_each(m_lstBlocks.begin(), m_lstBlocks.end(), FuncDraw<tagBlock>(pPainter));
}

VOID Level::RefreshBlocks( const Vector2N &vIdx )
{
	++m_nRefreshTimes;

	BOOL	bMat[3][3];
	memset(bMat, 0, sizeof(bMat));

	list<tagBlock*>	lstNeedDel;

	for(list<tagBlock*>::iterator itr = m_lstBlocks.begin();
		itr != m_lstBlocks.end();
		++itr)
	{
		if( IsBetweenClose((*itr)->vIdx.x, vIdx.x-1, vIdx.x+1) && IsBetweenClose((*itr)->vIdx.y, vIdx.y-1, vIdx.y+1) )
		{
			Vector2N vOffset = (*itr)->vIdx - vIdx;
			bMat[vOffset.x+1][vOffset.y+1] = TRUE;
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
			}
		}
	}
	
}

BOOL Level::AddObj( Tile* pObj )
{
	m_lstTiles.push_back(pObj);
	m_collider.AddGameObj(pObj);

	return TRUE;
}

VOID Level::DelObj( Tile* pObj )
{
	list<Tile*>::iterator itr_del = std::find(m_lstTiles.begin(), m_lstTiles.end(), pObj);
	if( itr_del != m_lstTiles.end() )
	{
		m_lstTiles.erase(itr_del);
	}

	m_collider.DelGameObj(pObj);
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
		pNew->SetCollideDirFlag(ECD_All);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		g_level.AddObj(pNew);
		lstTiles.push_back(pNew);
	}

	for( FLOAT f=XTileSize/2; f<=XScreenH - XTileSize/2; f+=XTileSize )
	{
		Tile* pNew = new Tile;
		pNew->SetPos(vOri + Vector2F(XTileSize/2, f));
		pNew->SetCollideDirFlag(ECD_All);

		static int i=0;
		if( i++ %2 )
		{
			pNew->SetColor(255, 0, 0);
		}

		g_level.AddObj(pNew);
		lstTiles.push_back(pNew);
	}

	return TRUE;
}

VOID tagBlock::UnLoad()
{
	while( !lstTiles.empty() )
	{
		g_level.DelObj(lstTiles.front());
		delete lstTiles.front();

		lstTiles.pop_front();
	}
}

VOID tagBlock::Draw( Painter* pPainter )
{
	Vector2F vCenter(FLOAT(vIdx.x*XScreenW), FLOAT(vIdx.y*XScreenH));
	vCenter += Vector2F(XScreenW/2, XScreenH/2);

	pPainter->WorldDrawText(vCenter, pPainter->GetColor(255, 0, 0),"%d, %d", vIdx.x, vIdx.y );
}
