#include "common.h"
#include "level.h"

#include "gameobj.h"
#include "painter.h"
#include "text.h"
#include "timer.h"

BOOL Level::Init()
{
	m_lstBlocks.clear();
	m_lstTiles.clear();

	m_pPlayer = new Player;
	m_pPlayer->SetPos(Vector2F(400, 30));
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

	Vector2N vNewIdx = ConvertToBlockIdx(m_pPlayer->GetPos());
	if( m_vLastIdx != vNewIdx )
	{
		RefreshBlocks(vNewIdx);
		m_vLastIdx = vNewIdx;
	}

	for_each(m_lstTiles.begin(), m_lstTiles.end(), TileUpdate(dt));
	m_pPlayer->Update(dt);

	m_collider.Collide();
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

VOID Level::Draw( Painter* pPainter )
{
	for_each(m_lstTiles.begin(), m_lstTiles.end(), TileDraw(pPainter));
	m_pPlayer->Draw(pPainter);

	Vector2N vIdx = ConvertToBlockIdx(m_pPlayer->GetPos());

	g_text.AddText(g_painter.GetColor(255, 255, 255), "vel of player:%4.2f ,%4.2f, %4.2f", m_pPlayer->m_vVel.x, m_pPlayer->m_vVel.y, m_pPlayer->m_vVel.Length());
	g_text.AddText(g_painter.GetColor(255, 255, 255), "block of player:%4d ,%4d", vIdx.x, vIdx.y);
	
}

VOID Level::RefreshBlocks( const Vector2N &vIdx )
{
	BOOL	bMat[3][3];
	ZeroMemory(bMat, sizeof(bMat));

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
	return Vector2N(INT(vPos.x / XScreenW), INT(vPos.y / XScreenH));
}

Level g_level;

BOOL tagBlock::Load( const Vector2N &vCenterIdx, const Vector2N &vOffset )
{
	vIdx = vCenterIdx + vOffset;

	Vector2F vOri(vIdx.x * XScreenW, vIdx.y * XScreenH);

	for( FLOAT f=XTileSize/2; f<=XScreenW/2; f+=XTileSize )
	{
		Tile* pNew = new Tile;
		pNew->SetPos(vOri + Vector2F(f, XTileSize/2));
		pNew->SetCollideDirFlag(ECD_All);

		g_level.AddObj(pNew);
		lstTiles.push_back(pNew);
	}

	lstTiles.front()->SetColor(255, 255, 255);
	lstTiles.back()->SetColor(255, 255, 255);

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
