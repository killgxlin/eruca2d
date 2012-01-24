#include "common.h"
#include "res_mgr.h"
#include "SDL_rotozoom.h"
#include "SDL_image.h"

ResMgr g_resmgr;

tagAniInfo playerAni[] = {
	{	EAT_Stand,	"rockman/stand%02d.bmp",	6,	0.5	},
	{	EAT_Run,	"rockman/run%02d.bmp",		11,	1	},
	{	EAT_Jump,	"rockman/jmp%02d.bmp",		6,	1	},
	{	EAT_Fall,	"rockman/fall%02d.bmp",		4,	1	},
	{	EAT_Land,	"rockman/land%02d.bmp",		2,	0.5	},
};



BOOL ResMgr::Load()
{
	if( IMG_Init(~0) == 0 ) return FALSE;

	{
		SDL_Surface* pSurface = LoadOneImageSurface("image/brick_ice.png");
		if( pSurface == NULL ) return FALSE;
		m_mapSurfaces.insert(make_pair(ssh_crc32("icebrick"), pSurface));
	}

	{
		tagAnimateProto* pAni = LoadOneAnimateProto(playerAni, sizeof(playerAni) / sizeof(playerAni[0]));
		if( pAni == NULL ) return FALSE;
		m_mapProtos.insert(make_pair(ssh_crc32("rockman"), pAni));
	}

	return TRUE;
}

tagAnimateProto* ResMgr::LoadOneAnimateProto( const tagAniInfo* pAniInfo, INT nNum )
{
	tagAnimateProto* pNew = new tagAnimateProto;

	float fZoom = 0.0f;
	INT nInfoLen = nNum;
	for(INT i=0; i<nInfoLen; ++i)
	{
		char buffer[100] = {0};
		for(INT j=1; j<=pAniInfo[i].nNum; ++j)
		{
			sprintf_s(buffer, pAniInfo[i].szName, j);
			SDL_Surface* pSrc = SDL_LoadBMP(buffer);
			SDL_SetColorKey(pSrc, SDL_SRCCOLORKEY, SDL_MapRGB(pSrc->format, 0, 0, 0));
			if( pSrc == NULL ) return NULL;

			if( fZoom == 0.0f )
			{
				fZoom = float(XPlayerSize) / pSrc->w;
			}

			SDL_Surface* pRight = zoomSurface(pSrc, fZoom, fZoom, SMOOTHING_ON);
			SDL_Surface* pLeft = zoomSurface(pSrc, -fZoom, fZoom, SMOOTHING_ON);
			if( pRight == NULL || pLeft == NULL ) return NULL;

			SDL_FreeSurface(pSrc);

			pNew->arrSurfaces[pAniInfo[i].eActType].acts[EAD_Right].push_back(pRight);
			pNew->arrSurfaces[pAniInfo[i].eActType].acts[EAD_Left].push_back(pLeft);
		}
		pNew->arrSurfaces[pAniInfo[i].eActType].fLastTime = pAniInfo[i].fLastTime;
	}
	SDL_Surface* pSurface = pNew->arrSurfaces[EAT_Stand].acts[EAD_Right][0];
	pNew->vBox = Vector2F(pSurface->w, pSurface->h);
	return pNew;
}

VOID ResMgr::UnLoadOneAnimateProto( tagAnimateProto* pProto )
{
	for(INT i=EAT_Stand; i<EAT_End; ++i)
	{
		for(INT j=EAD_Right; j<EAD_End; ++j)
		{
			for(INT nSize = pProto->arrSurfaces[i].acts[j].size()-1; nSize >= 0; --nSize)
			{
				SDL_FreeSurface(pProto->arrSurfaces[i].acts[j][nSize]);
			}
			pProto->arrSurfaces[i].acts[j].clear();
		}
	}

	delete pProto;
}

VOID ResMgr::UnLoad()
{
	for(MapAniProto::iterator itr=m_mapProtos.begin(); itr!=m_mapProtos.end(); ++itr)
	{
		UnLoadOneAnimateProto(itr->second);
	}
	m_mapProtos.clear();

	for(MapSurface::iterator itr=m_mapSurfaces.begin(); itr!=m_mapSurfaces.end(); ++itr)
	{
		UnLoadOneImageSurface(itr->second);
	}
	m_mapSurfaces.clear();

	IMG_Quit();
}

const tagAnimateProto* ResMgr::GetAnimateProto( const char* szName )
{
	MapAniProto::iterator itr = m_mapProtos.find(ssh_crc32(szName));
	if( itr == m_mapProtos.end() ) return NULL;

	return itr->second;
}

const SDL_Surface* ResMgr::GetImageSurface( const char* szName )
{
	MapSurface::iterator itr = m_mapSurfaces.find(ssh_crc32(szName));
	if( itr == m_mapSurfaces.end() ) return NULL;

	return itr->second;
}

SDL_Surface* ResMgr::LoadOneImageSurface( const char* szName )
{
	SDL_Surface* pSurface = IMG_Load(szName);
	
	return pSurface;
}

VOID ResMgr::UnLoadOneImageSurface( SDL_Surface* pImage )
{
	SDL_FreeSurface(pImage);
}

