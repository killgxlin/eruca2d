#ifndef COLLIDER_H
#define COLLIDER_H


class GameObj;



class Collider
{
	typedef pair<GameObj*, GameObj*> CollidePair;
public:
	BOOL	AddGameObj(GameObj* pObj);
	VOID	DelGameObj(GameObj* pObj);
	VOID	Collide();

private:
	BOOL	CalCollideOrder(GameObj* &pCaller, GameObj* &pArg);

	list<CollidePair>	m_lstCollidePairs;
	list<GameObj*>		m_lstObjs;
};

#endif