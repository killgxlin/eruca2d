#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "collider.h"

class Painter;
class Tile;
class Player;



struct tagBlock
{
	Vector2N	vIdx;
	list<Tile*>	lstTiles;

	BOOL Load(const Vector2N &vCenterIdx, const Vector2N &vOffset);

	VOID		UnLoad();
	VOID		Draw(Painter* pPainter);


};

class Level
{
public:
	BOOL	Init();
	VOID	Destroy();


	VOID	Update(FLOAT dt);
	VOID	Draw(Painter* pPainter);

	Player*		m_pPlayer;

	// something new :)
public:
	Vector2N	ConvertToBlockIdx(const Vector2F &vPos);
	VOID		RefreshBlocks(const Vector2N &vIdx);
	BOOL AddObj(Tile* pObj);
	VOID DelObj(Tile* pObj);

	list<tagBlock*>	m_lstBlocks;
	Vector2N	m_vLastIdx;
	list<Tile*>	m_lstTiles;
	Collider	m_collider;

	INT			m_nRefreshTimes;
	INT			m_nNewXIdx;

};

extern 	Level g_level;

#endif