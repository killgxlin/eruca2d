#include "common.h"
#include "level.h"

#include "gameobj.h"
#include "painter.h"
#include "text.h"
#include "timer.h"

BOOL Level::Load()
{
	m_pPlayer = new Player;
	m_pPlayer->SetPos(Vector2(100, 10));
	m_collider.AddGameObj(m_pPlayer);

	for( INT i=10; i<= 630; i+=20 )
	{
		Tile* pTile = new Tile;
		pTile->SetPos(Vector2(i, 400));
		pTile->SetCollideDirFlag(ECD_Top | ECD_Down);
		m_lstTiles.push_back(pTile);
		m_collider.AddGameObj(pTile);
	}

	{
		Tile* pTile = new Tile;
		pTile->SetPos(Vector2(320, 200));
		pTile->SetCollideDirFlag(ECD_All);
		m_lstTiles.push_back(pTile);
		m_collider.AddGameObj(pTile);
	}



	return TRUE;
}

VOID Level::UnLoad()
{
	while( !m_lstTiles.empty() )
	{
		m_collider.DelGameObj(m_lstTiles.front());

		delete m_lstTiles.front();
		m_lstTiles.pop_front();
	}

	m_collider.DelGameObj(m_pPlayer);
	delete m_pPlayer;
}
struct TileUpdate
{
	TileUpdate(float dt):m_dt(dt){}
	VOID operator()(Tile* pTile)
	{
		pTile->Update(m_dt);
	}
	float m_dt;
};

VOID Level::Update( float dt )
{
	dt *= g_framerate.GetSpeedRate();

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

	g_text.AddText(g_painter.GetColor(255, 255, 255), "vel of player:%4.2f ,%4.2f, %4.2f", m_pPlayer->m_vVel.x, m_pPlayer->m_vVel.y, m_pPlayer->m_vVel.Length());
	
}
