#ifndef LEVEL_H
#define LEVEL_H

#include "collider.h"

class Painter;
class Tile;
class Player;

class Level
{
public:
	BOOL	Load();
	VOID	UnLoad();


	VOID	Update(float dt);
	VOID	Draw(Painter* pPainter);

	list<Tile*>	m_lstTiles;
	Player*		m_pPlayer;
	Collider	m_collider;
};

#endif