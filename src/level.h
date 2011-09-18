#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "collider.h"

class Painter;
class Terrain;
class Player;



struct tagBlock
{
	Vector2N		vIdx;
	vector<Terrain*>	vecTiles;

	BOOL		Load(const Vector2N &vCenterIdx, const Vector2N &vOffset);
	VOID		UnLoad();

	VOID		Draw(Painter* pPainter);
	VOID		Update(float dt);
	VOID		Collide(GameObj* pObj);

};

class Level
{
public:
	BOOL		Init();
	VOID		Destroy();


	VOID		Update(FLOAT dt);
	VOID		Draw(Painter* pPainter);

	Player*		m_pPlayer;

	// something new :)
public:
	Vector2N	ConvertToBlockIdx(const Vector2F &vPos);
	VOID		RefreshBlocks(const Vector2N &vIdx);

	list<tagBlock*>	m_lstBlocks;
	Vector2N	m_vLastIdx;

	INT			m_nRefreshTimes;
	INT			m_nNewXIdx;

public:
	tagBlock*	m_matBlocks[3][3];

	template<typename T>
	VOID		ForEachBlock(T func)
	{
		static INT nIdx[] = {4, 1, 3, 5, 7, 0, 2, 6, 8};
		for(INT i=0; i<sizeof(nIdx) / sizeof(INT); ++i)
		{
			INT nX = nIdx[i] / 3;
			INT nY = nIdx[i] % 3;
			assert(m_matBlocks[nX][nY] != NULL);
			func(m_matBlocks[nX][nY]);
		}
	}
};

extern 	Level g_level;

#endif