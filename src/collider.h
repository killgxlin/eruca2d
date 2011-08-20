#ifndef __COLLIDER_H__
#define __COLLIDER_H__


class GameObj;

struct CollidePair : public pair<GameObj*, GameObj*>
{
	FLOAT	fLen2;
};


class Collider
{
	
public:
	BOOL	AddGameObj(GameObj* pObj);
	VOID	DelGameObj(GameObj* pObj);
	VOID	Collide();

private:
	BOOL	CalCollideOrder(GameObj* &pCaller, GameObj* &pArg);

	vector<CollidePair>	m_lstCollidePairs;
	list<GameObj*>		m_lstObjs;
};

#endif