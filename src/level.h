#ifndef LEVEL_NEW_h
#define LEVEL_NEW_h


class Player;
class Arrow;
class Animal;

struct tagTerrain 
{
	bool		bExist;
	bool		bCollide;
};

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
	DWORD		CalcCollideFlag( INT i, INT j );
public:

	tagTerrain		m_matTerrain[XTilesW][XTilesH];
	list<Player*>	m_lstPlayers;
	list<Arrow*>	m_lstArrows;
	list<Animal*>	m_lstAnimals;

public:
	VOID		SetCenter(const Vector2F &vPos)
	{
		m_vCenter = vPos;
		m_vOtherCenter = m_vCenter;
		if( m_vCenter.x + XScreenW / m_fZoomRate / 2 >= XTotalW )
		{
			m_vOtherCenter.x -= XTotalW;
		}
		else if( m_vCenter.x - XScreenW / m_fZoomRate / 2 < 0 )
		{
			m_vOtherCenter.x += XTotalW;
		}
	}

private:
	Vector2F	m_vCenter;
	Vector2F	m_vOtherCenter;
	FLOAT		m_fZoomRate;


};

extern Level g_level;

#endif