#include "common.h"
#include "collider.h"

#include "gameobj.h"
#include "text.h"
#include "painter.h"

BOOL Collider::AddGameObj( GameObj* pObj )
{
	for(list<GameObj*>::iterator itr = m_lstObjs.begin();
		itr != m_lstObjs.end();
		++itr)
	{
		CollidePair pair;
		pair.first = (*itr);
		pair.second = pObj;

		if( CalCollideOrder(pair.first, pair.second) )
		{
			m_lstCollidePairs.push_back(pair);
		}
	}

	m_lstObjs.push_back(pObj);

	return TRUE;
}

VOID Collider::DelGameObj( GameObj* pObj )
{
	list<GameObj*>::iterator go_itr = std::find(m_lstObjs.begin(), m_lstObjs.end(), pObj);
	if( go_itr != m_lstObjs.end() ) m_lstObjs.erase(go_itr);

	list<vector<CollidePair>::iterator> lst2Del;

	vector<CollidePair>::iterator cp_itr = m_lstCollidePairs.begin();
	while( cp_itr != m_lstCollidePairs.end() )
	{
		vector<CollidePair>::iterator cur_itr = cp_itr++;
		if( cur_itr->first == pObj || cur_itr->second == pObj )
		{
			lst2Del.push_back(cur_itr);
		}
	}

	while( !lst2Del.empty() )
	{
		m_lstCollidePairs.erase(lst2Del.front());
		lst2Del.pop_front();
	}
}

bool cmp(const CollidePair &lhs, const CollidePair &rhs)
{
	return lhs.fLen2 < rhs.fLen2;
}

VOID Collider::Collide()
{
	std::sort(m_lstCollidePairs.begin(), m_lstCollidePairs.end(), cmp);

	tagCollideRes result;
	for(vector<CollidePair>::iterator itr = m_lstCollidePairs.begin();
		itr != m_lstCollidePairs.end();
		++itr)
	{
		itr->first->Collide(itr->second, &result);
	}

	g_text.AddText(g_painter.GetColor(255, 0, 0), "col num  : %d", m_lstCollidePairs.size());
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
