#include "common.h"
#include "res_mgr.h"
#include "SDL_rotozoom.h"
#include "SDL_image.h"
VOID ResMgrEx::UnloadOneAction( tagAction* pUnload )
{
	for(tagAction::SurfaceVec::iterator itr = pUnload->vecSurfaces.begin();
		itr != pUnload->vecSurfaces.end();
		++itr)
	{
		SDL_FreeSurface(*itr);
	}
	pUnload->vecSurfaces.clear();
}

BOOL ResMgrEx::LoadOneAction( const tagActionProto* pProto )
{
	INT32 nAliasCrc = ssh_crc32(pProto->szAlias);
	ActionMap::iterator itr = m_mapActions.find(nAliasCrc);
	if( itr != m_mapActions.end() )
	{
		return FALSE;
	}

	tagAction* pNew = new tagAction;
	pNew->fDuration = pProto->fDuration;
	float fZoom = -1.0f;
	char buffer[100] = {0};
	UINT32 u32ColorKey = 0;

	for(INT j=1; j<=pProto->nNum; ++j)
	{
		if( pProto->nNum != 1 )
		{
			sprintf_s(buffer, pProto->szFileName, j);
		}
		else
		{
			sprintf_s(buffer, pProto->szFileName);
		}

		SDL_Surface* pSrc = IMG_Load(buffer);
		if( pSrc == NULL ) 
		{
			UnloadOneAction(pNew);
			return FALSE;
		}

		// 第一次设置
		if( fZoom < 0.0f )
		{
			// 若需要指定尺寸
			if( pProto->fWidth > 0.0f  )
			{
				fZoom = pProto->fWidth / pSrc->w;
			}
			// 默认大小
			else
			{
				fZoom = 1.0f;
			}

			// colorkey
			u32ColorKey = ((UINT32*)pSrc->pixels)[0];

			pNew->vSize.x = fZoom * pSrc->w;
			pNew->vSize.y = pNew->vSize.x;
			/*pNew->vSize.y = fZoom * pSrc->h;*/
		}

		SDL_SetColorKey(pSrc, SDL_SRCCOLORKEY, u32ColorKey);

		SDL_Surface* pDst = zoomSurface(pSrc, pProto->bFlip ? -fZoom : fZoom, fZoom, SMOOTHING_ON);
		SDL_FreeSurface(pSrc);

		pNew->vecSurfaces.push_back(pDst);
	}

	m_mapActions.insert(make_pair(nAliasCrc, pNew));
	return TRUE;
}

tagActionProto arrActionPro[] = {
	// rockman right actions
	{	"rockman/stand%02d.bmp",	6,	"rockman_stand_r",	0.5,	false,	XPlayerSize	},
	{	"rockman/run%02d.bmp",		11,	"rockman_run_r",	1,		false,	XPlayerSize	},
	{	"rockman/jmp%02d.bmp",		6,	"rockman_jump_r",	1,		false,	XPlayerSize	},
	{	"rockman/fall%02d.bmp",		4,	"rockman_fall_r",	1,		false,	XPlayerSize	},
	{	"rockman/land%02d.bmp",		2,	"rockman_land_r",	0.5,	false,	XPlayerSize	},

	// rockman left actions
	{	"rockman/stand%02d.bmp",	6,	"rockman_stand_l",	0.5,	true,	XPlayerSize	},
	{	"rockman/run%02d.bmp",		11,	"rockman_run_l",	1,		true,	XPlayerSize	},
	{	"rockman/jmp%02d.bmp",		6,	"rockman_jump_l",	1,		true,	XPlayerSize	},
	{	"rockman/fall%02d.bmp",		4,	"rockman_fall_l",	1,		true,	XPlayerSize	},
	{	"rockman/land%02d.bmp",		2,	"rockman_land_l",	0.5,	true,	XPlayerSize	},

	// terrain action
	{	"image/brick_ice.bmp",		1,	"brick_ice",		-1.0f,	false,	XTerrainSize},

};

BOOL ResMgrEx::Load()
{
	INT nNum = sizeof(arrActionPro) / sizeof(arrActionPro[0]);
	for(INT i=0; i<nNum; ++i)
	{
		if( !LoadOneAction(&arrActionPro[i]) )
			return FALSE;
	}

	return TRUE;
}

VOID ResMgrEx::Unload()
{
	for(ActionMap::iterator itr = m_mapActions.begin();
		itr != m_mapActions.end();
		++itr)
	{
		UnloadOneAction(itr->second);
	}
	m_mapActions.clear();
}

tagAction* ResMgrEx::GetAction( const char* szAlias )
{
	INT32 nAliasCrc = ssh_crc32(szAlias);
	ActionMap::iterator itr = m_mapActions.find(nAliasCrc);
	if( itr == m_mapActions.end() )
	{
		return NULL;
	}
	return itr->second;
}

ResMgrEx g_resmgrex;