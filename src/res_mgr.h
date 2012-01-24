#ifndef __RES_MGR_H__
#define __RES_MGR_H__

struct tagAnimateProto
{
	typedef vector<SDL_Surface*>	VecSurface;
	struct tagActionProto
	{
		VecSurface		acts[EAD_End];
		FLOAT			fLastTime;
	};
	tagActionProto		arrSurfaces[EAT_End];
	Vector2F			vBox;
};

struct tagAniInfo
{
	EActType	eActType;
	const char* szName;
	INT			nNum;
	FLOAT		fLastTime;
};

class ResMgr
{
public:
	typedef map<INT32, tagAnimateProto*>	MapAniProto;
	typedef map<INT32, SDL_Surface*>		MapSurface;

	BOOL				Load();
	VOID				UnLoad();
	const tagAnimateProto*	GetAnimateProto(const char* szName);
	const SDL_Surface*		GetImageSurface(const char* szName);

private:
	tagAnimateProto*	LoadOneAnimateProto( const tagAniInfo* pAniInfo, INT nNum );
	VOID				UnLoadOneAnimateProto(tagAnimateProto* pProto);

	SDL_Surface*		LoadOneImageSurface( const char* szName );
	VOID				UnLoadOneImageSurface( SDL_Surface* pImage );

	MapAniProto			m_mapProtos;
	MapSurface			m_mapSurfaces;
};

extern ResMgr g_resmgr;

#endif