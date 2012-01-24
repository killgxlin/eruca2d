#ifndef __RES_MGR_H__
#define __RES_MGR_H__

//-------------------------------------------------------------------------------
struct tagActionProto
{
	const char* szFileName;
	INT			nNum;

	const char* szAlias;
	FLOAT		fDuration;
	bool		bFlip;
	FLOAT		fWidth;
};

struct tagAction
{
	typedef vector<SDL_Surface*>	SurfaceVec;
	SurfaceVec		vecSurfaces;
	FLOAT			fDuration;
	Vector2F		vSize;
};

class ResMgrEx
{
public:
	BOOL	Load();
	VOID	Unload();
	
	tagAction*	GetAction(const char* szAlias);
private:
	VOID	UnloadOneAction( tagAction* pUnload );
	BOOL	LoadOneAction(const tagActionProto* pProto);

	typedef map<INT32, tagAction*>			ActionMap;
	ActionMap								m_mapActions;
};

extern ResMgrEx g_resmgrex;

#endif