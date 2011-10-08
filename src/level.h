#ifndef LEVEL_NEW_h
#define LEVEL_NEW_h


class Player;
class Arrow;
class Animal;

class Level
{
public:
	BOOL		OverLapEdge(Vector2F &vPos, Vector2F &vSize);
	Vector2F	TransPos(Vector2F &vOldPos);
	Vector2F	GetProperPos(Vector2F &vPos);
public:
	BOOL		Init();
	VOID		Destroy();

	VOID		Update(FLOAT dt);

	VOID		Draw(Painter* pPainter);

public:

	int				m_matTerrain[XTilesW][XTilesH];
	list<Player*>	m_lstPlayers;
	list<Arrow*>	m_lstArrows;
	list<Animal*>	m_lstAnimals;

};

extern Level g_level;

#endif