#include "common.h"
#include "level.h"

#include "gameobj.h"


Level g_level;

BOOL Level::Init()
{
	memset(m_matTerrain, 0, sizeof(m_matTerrain));
	for(INT i=0; i<XTilesW; ++i)
	{
		for(INT j=0; j<XTilesH; ++j)
		{
			if( j < 1 )
			{
				m_matTerrain[i][j] = 1;
			}
			else
			{
				if( j > 0 && m_matTerrain[i][j-1] && (rand()%2))
				{
					m_matTerrain[i][j] = 1;
				}
				else
				{
					m_matTerrain[i][j] = 0;
				}
			}
		}
	}

	Player* pPlayer = new Player;
	pPlayer->Init();
	pPlayer->SetPos(Vector2F(rand()%XScreenW, XScreenH));
	pPlayer->m_vVel = Vector2F(0, 0);

	m_lstPlayers.push_back(pPlayer);

	Animal* pAnimal = new Animal;
	pAnimal->Init();
	pAnimal->SetPos(pPlayer->GetPos());
	pAnimal->m_vVel = Vector2F(0, 0);

	m_lstAnimals.push_back(pAnimal);


	return TRUE;
}

VOID Level::Destroy()
{
	while( !m_lstPlayers.empty() )
	{
		m_lstPlayers.front()->Destroy();
		delete m_lstPlayers.front();
		m_lstPlayers.pop_front();
	}

	while( !m_lstArrows.empty() )
	{
		m_lstArrows.front()->Destroy();
		delete m_lstArrows.front();
		m_lstArrows.pop_front();
	}

	while( !m_lstAnimals.empty() )
	{
		m_lstAnimals.front()->Destroy();
		delete m_lstAnimals.front();
		m_lstAnimals.pop_front();
	}
}


template<typename T>
struct UpdateGameObj
{
	UpdateGameObj(float dt):m_dt(dt){}
	VOID operator()(T* pT)
	{
		pT->Update(m_dt);
	}

	float m_dt;
};	

struct CollideWithTerrain
{
	VOID operator()(Movable* pT)
	{
		Terrain tmpTerrain;
		tmpTerrain.Init();
		tagCollideRes result;

		pT->SetPos(g_level.GetProperPos(pT->GetPos()));

		AABBox box = pT->GetAABBox();
		INT nIdxXMin  = max(INT(floor(box.vMin.x / XTerrainSize)), 0);
		INT nIdxYMin  = max(INT(floor(box.vMin.y / XTerrainSize)), 0);
		INT nIdxXMax  = min(INT(floor(box.vMax.x / XTerrainSize)), XTilesW - 1);
		INT nIdxYMax  = min(INT(floor(box.vMax.y / XTerrainSize)), XTilesH - 1);

		// 当前计算碰撞
		for(INT i = nIdxXMin; i <= nIdxXMax; ++i)
		{
			for(INT j = nIdxYMin; j <= nIdxYMax; ++j)
			{
				if( !g_level.m_matTerrain[i][j] ) continue;

				tmpTerrain.SetPos(Vector2F(XTerrainSize*(i + 0.5f), XTerrainSize*(j + 0.5f )));
				tmpTerrain.SetCollideDirFlag(CalcCollideFlag(i, j));
				tmpTerrain.Collide(pT, &result);
			}
		}

		// 得到结果
		Vector2F vNewPos = pT->GetPos();

		// 若结果盒子和边界重叠
		if( g_level.OverLapEdge(vNewPos, pT->GetSize()) )
		{
			// 转换坐标系
			vNewPos = g_level.TransPos(vNewPos);
			pT->SetPos(vNewPos);

			// 计算碰撞
			AABBox box = pT->GetAABBox();
			INT nIdxXMin  = max(INT(floor(box.vMin.x / XTerrainSize)), 0);
			INT nIdxYMin  = max(INT(floor(box.vMin.y / XTerrainSize)), 0);
			INT nIdxXMax  = min(INT(floor(box.vMax.x / XTerrainSize)), XTilesW - 1);
			INT nIdxYMax  = min(INT(floor(box.vMax.y / XTerrainSize)), XTilesH - 1);

			// 当前计算碰撞
			for(INT i = nIdxXMin; i <= nIdxXMax; ++i)
			{
				for(INT j = nIdxYMin; j <= nIdxYMax; ++j)
				{
					if( !g_level.m_matTerrain[i][j] ) continue;

					tmpTerrain.SetPos(Vector2F(XTerrainSize*(i + 0.5f), XTerrainSize*(j + 0.5f )));
					tmpTerrain.SetCollideDirFlag(CalcCollideFlag(i, j));
					tmpTerrain.Collide(pT, &result);
				}
			}
			// 切换到合适的坐标系
			pT->SetPos(g_level.GetProperPos(pT->GetPos()));
		}

		tmpTerrain.Destroy();
	}

	DWORD CalcCollideFlag( INT i, INT j ) 
	{
		DWORD dwDirFlag = ECD_All;
		if( i-1 >= 0 && g_level.m_matTerrain[i-1][j] )
		{
			dwDirFlag &= ~ECD_Left;
		}
		if( i+1 < XTilesW && g_level.m_matTerrain[i+1][j] )
		{
			dwDirFlag &= ~ECD_Right;
		}
		if( j-1 >= 0 && g_level.m_matTerrain[i][j-1] )
		{
			dwDirFlag &= ~ECD_Down;
		}
		if( j+1 < XTilesH && g_level.m_matTerrain[i][j+1] )
		{
			dwDirFlag &= ~ECD_Top;
		}

		return dwDirFlag;
	}

};

struct CollideWithGameObj
{
	Movable*	m_pMover;
	CollideWithGameObj(Movable* pMover):m_pMover(pMover){}
	VOID operator()(GameObj* pGameObj)
	{
		tagCollideRes result;
		pGameObj->Collide(m_pMover, &result);
	}
};

template<typename T>
struct CollideWithGameObjs
{
	const list<T*>	&m_lstGameObjs;
	CollideWithGameObjs(const list<T*> &lstGameObjs):m_lstGameObjs(lstGameObjs){}
	VOID operator()(Movable* pMover)
	{
		for_each(m_lstGameObjs.begin(), m_lstGameObjs.end(), CollideWithGameObj(pMover));
	}
};

VOID Level::Update( FLOAT dt )
{
	for(list<Player*>::iterator itr = m_lstPlayers.begin();
		m_lstPlayers.end() != itr;
		)
	{
		list<Player*>::iterator tmpitr = itr++;
		if( (*tmpitr)->IsDeleted() )
		{
			(*tmpitr)->Destroy();
			delete (*tmpitr);
			m_lstPlayers.erase(tmpitr);
		}
	}

	for(list<Arrow*>::iterator itr = m_lstArrows.begin();
		m_lstArrows.end() != itr;
		)
	{
		list<Arrow*>::iterator tmpitr = itr++;
		if( (*tmpitr)->IsDeleted() )
		{
			(*tmpitr)->Destroy();
			delete (*tmpitr);
			m_lstArrows.erase(tmpitr);
		}
	}

	for(list<Animal*>::iterator itr = m_lstAnimals.begin();
		m_lstAnimals.end() != itr;
		)
	{
		list<Animal*>::iterator tmpitr = itr++;
		if( (*tmpitr)->IsDeleted() )
		{
			(*tmpitr)->Destroy();
			delete (*tmpitr);
			m_lstAnimals.erase(tmpitr);
		}
	}

	for_each(m_lstPlayers.begin(), m_lstPlayers.end(), UpdateGameObj<Player>(dt));
	for_each(m_lstArrows.begin(), m_lstArrows.end(), UpdateGameObj<Arrow>(dt));
	for_each(m_lstAnimals.begin(), m_lstAnimals.end(), UpdateGameObj<Animal>(dt));

	for_each(m_lstPlayers.begin(), m_lstPlayers.end(), CollideWithTerrain());
	for_each(m_lstArrows.begin(), m_lstArrows.end(), CollideWithTerrain());
	for_each(m_lstAnimals.begin(), m_lstAnimals.end(), CollideWithTerrain());

	for_each(m_lstPlayers.begin(), m_lstPlayers.end(), CollideWithGameObjs<Arrow>(m_lstArrows));
	for_each(m_lstAnimals.begin(), m_lstAnimals.end(), CollideWithGameObjs<Arrow>(m_lstArrows));
	
	for_each(m_lstPlayers.begin(), m_lstPlayers.end(), CollideWithGameObjs<Animal>(m_lstAnimals));

	g_painter.SetCenter(m_lstPlayers.front()->GetPos());

	g_text.AddText(g_painter.GetColor(255, 0, 0), "player on land :%4d", m_lstPlayers.front()->m_bLand);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "pos of player  :%4.2f ,%4.2f", m_lstPlayers.front()->GetPos().x, m_lstPlayers.front()->GetPos().y);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "vel of player  :%4.2f ,%4.2f, %4.2f", m_lstPlayers.front()->m_vVel.x, m_lstPlayers.front()->m_vVel.y, m_lstPlayers.front()->m_vVel.Length());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "num of arrows  :%d", m_lstArrows.size());

}

template<typename T>
struct DrawGameObj
{
	VOID operator()(T* pT)
	{
		pT->Draw(&g_painter);
	}

};

VOID Level::Draw( Painter* pPainter )
{
	Terrain tmpTerrain;
	tmpTerrain.Init();
	for(INT i=0; i<XTilesW; ++i)
	{
		for(INT j=0; j<XTilesH; ++j)
		{
			switch( m_matTerrain[i][j] )
			{
			case 0:
				break;
			default:
				tmpTerrain.SetPos(Vector2F(XTerrainSize*(i + 0.5f), XTerrainSize*(j + 0.5f )));
				if( i==0 )
				{
					tmpTerrain.SetColor(255, 0, 0);
				}
				else
				{
					tmpTerrain.SetColor(0, 255, 0);
				}
				tmpTerrain.Draw(pPainter);
				break;
			}
		}
	}
	tmpTerrain.Destroy();

	for_each(m_lstPlayers.begin(), m_lstPlayers.end(), DrawGameObj<Player>());
	for_each(m_lstArrows.begin(), m_lstArrows.end(), DrawGameObj<Arrow>());
	for_each(m_lstAnimals.begin(), m_lstAnimals.end(), DrawGameObj<Animal>());
}

BOOL Level::OverLapEdge( Vector2F &vPos, Vector2F &vSize )
{
	if( vPos.x - vSize.x / 2 < 0 && vPos.x + vSize.x / 2 > 0 ) return TRUE;
	if( vPos.x - vSize.x / 2 < XTotalW && vPos.x + vSize.x / 2 > XTotalW ) return TRUE;

	return FALSE;
}

Vector2F Level::TransPos( Vector2F &vOldPos )
{
	Vector2F vRt = vOldPos;
	if( vOldPos.x > XTotalW / 2 )
	{
		vRt.x -= XTotalW;
	}
	else
	{
		vRt.x += XTotalW;
	}

	return vRt;
}

Vector2F Level::GetProperPos( Vector2F &vPos )
{
	Vector2F vRt = vPos;
	if( vPos.x >= XTotalW ) vRt.x -= XTotalW;
	else if( vPos.x < 0 ) vRt.x += XTotalW;
	return vRt;
}
