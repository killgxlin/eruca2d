#include "common.h"
#include "res_mgr.h"

#include "util.h"
#include "SDL_rotozoom.h"

ResMgr	sResMgr;

void ResMgr::Destroy()
{
}

SDL_Surface* ResMgr::CreateSurface( const char* file_ )
{
	char lwr_file[200];
	strncpy(lwr_file, file_, 200);
	strtolower(lwr_file);

	SDL_Surface* sub_surface = IMG_Load(lwr_file);
	SDL_SetColorKey(sub_surface, SDL_SRCCOLORKEY, *(Uint8 *)(sub_surface->pixels));

	return sub_surface;
}

SDL_Surface* ResMgr::CreateSubSurface( SDL_Surface* src_, SDL_Rect* rect_, bool flip_ )
{
	SDL_Surface* sub_surface = SDL_CreateRGBSurface(src_->flags, rect_->w, rect_->h, src_->format->BitsPerPixel, 0, 0, 0, 0);
	if( sub_surface == 0 ) return 0;

	SDL_BlitSurface(src_, rect_, sub_surface, 0);

	if( flip_ )
	{
		SDL_Surface* flip_surface = zoomSurface(sub_surface, -1.0f, 1.0f, false);
		SDL_FreeSurface(sub_surface);

		sub_surface = flip_surface;
	}

	SDL_SetColorKey(sub_surface, SDL_SRCCOLORKEY, *(Uint8 *)(sub_surface->pixels));

	return sub_surface;
}
