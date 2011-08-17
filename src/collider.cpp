#include "common.h"
#include "collider.h"

#include "gameobj.h"


BOOL Collider::AddGameObj( GameObj* pObj )
{
	for(list<GameObj*>::iterator itr = m_lstObjs.begin();
		itr != m_lstObjs.end();
		++itr)
	{
		GameObj* pCaller = (*itr);
		GameObj* pArg = pObj;

		if( CalCollideOrder(pCaller, pArg) )
		{
			m_lstCollidePairs.push_back(make_pair(pCaller, pArg));
		}
	}

	m_lstObjs.push_back(pObj);

	return TRUE;
}

VOID Collider::DelGameObj( GameObj* pObj )
{
	list<GameObj*>::iterator go_itr = std::find(m_lstObjs.begin(), m_lstObjs.end(), pObj);
	if( go_itr != m_lstObjs.end() ) m_lstObjs.erase(go_itr);

	list<CollidePair>::iterator cp_itr = m_lstCollidePairs.begin();
	while( cp_itr != m_lstCollidePairs.end() )
	{
		list<CollidePair>::iterator cur_itr = cp_itr++;
		if( cur_itr->first == pObj || cur_itr->second == pObj )
		{
			m_lstCollidePairs.erase(cur_itr);
		}
	}
}

VOID Collider::Collide()
{
	tagCollideRes result;
	for(list<CollidePair>::iterator itr = m_lstCollidePairs.begin();
		itr != m_lstCollidePairs.end();
		++itr)
	{
		itr->first->Collide(itr->second, &result);
	}
}

BOOL Collider::CalCollideOrder( GameObj* &pCaller, GameObj* &pArg )
{
	if( pCaller->GetCollidePri() == pArg->GetCollidePri() && pCaller->GetCollidePri() == ECP_Static )
	{
		return FALSE;
	}
	if( pCaller->GetCollidePri() < pArg->GetCollidePri() )
	{
		GameObj* pTmp = pCaller;
		pCaller = pArg;
		pArg = pTmp;
	}

	return TRUE;
}
