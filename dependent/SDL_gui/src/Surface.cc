#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_Surface::~GUI_Surface(void)
{
	if (surface)
		SDL_FreeSurface(surface);
}

GUI_Surface::GUI_Surface(const char *aname, SDL_Surface *image)
: GUI_Object(aname)
{
	assert(image != NULL);

	surface = image;
}

GUI_Surface::GUI_Surface(const char *fn)
: GUI_Object(fn)
{
	surface = IMG_Load(fn);
	if (surface == NULL)
		throw GUI_Exception("failed to load image '%s'", fn);
}

GUI_Surface::GUI_Surface(const char *aname, int f, int w, int h, int d, int rm, int bm, int gm, int am)
: GUI_Object(aname)
{
	surface = SDL_AllocSurface(f, w, h, d, rm, bm, gm, am);
	if (surface == NULL)
		throw GUI_Exception("failed to allocate surface (f=%d, w=%d, h=%d, d=%d)", f, w, h, d);
}

void GUI_Surface::Blit(SDL_Rect *src_r, GUI_Surface *dst, SDL_Rect *dst_r)
{
	SDL_BlitSurface(surface, src_r, dst->surface, dst_r);
}

void GUI_Surface::UpdateRect(int x, int y, int w, int h)
{
	SDL_UpdateRect(surface, x, y, w, h);
}

void GUI_Surface::UpdateRects(int n, SDL_Rect *rects)
{
	SDL_UpdateRects(surface, n, rects);
}

void GUI_Surface::Fill(SDL_Rect *r, Uint32 c)
{
	SDL_FillRect(surface, r, c);
}

int GUI_Surface::GetWidth(void)
{
	return surface->w;
}

int GUI_Surface::GetHeight(void)
{
	return surface->h;
}

SDL_Surface *GUI_Surface::GetSurface(void)
{
	return surface;
}

Uint32 GUI_Surface::MapRGB(int r, int g, int b)
{
	return SDL_MapRGB(surface->format, r, g, b);
}

int GUI_Surface::IsDoubleBuffered(void)
{
	return (surface->flags & SDL_DOUBLEBUF) != 0;
}

int GUI_Surface::IsHardware(void)
{
	return (surface->flags & SDL_HWSURFACE) != 0;
}

void GUI_Surface::Flip(void)
{
	SDL_Flip(surface);
}

void GUI_Surface::DisplayFormat(void)
{
	SDL_Surface *temp = SDL_DisplayFormat(surface);
	if (!temp)
		throw GUI_Exception("failed to format surface for display: %s", SDL_GetError());
	SDL_FreeSurface(surface);
	surface = temp;
}

void GUI_Surface::SetAlpha(Uint32 flag, Uint8 alpha)
{
	SDL_SetAlpha(surface, flag, alpha);
}

void GUI_Surface::SetColorKey(Uint32 c)
{
	if (SDL_SetColorKey(surface, SDL_RLEACCEL | SDL_SRCCOLORKEY, c) < 0)
		throw GUI_Exception("failed to set color key for surface: %s", SDL_GetError());
}

void GUI_Surface::SaveBMP(const char *filename)
{
	SDL_SaveBMP(surface, "screenshot.bmp");
}

extern "C"
{

GUI_Surface *GUI_SurfaceLoad(const char *fn)
{
	return new GUI_Surface(fn);
}

GUI_Surface *GUI_SurfaceCreate(const char *aname, int f, int w, int h, int d, int rm, int gm, int bm, int am)
{
	return new GUI_Surface(aname, f, w, h, d, rm, gm, bm, am);
}

void GUI_SurfaceBlit(GUI_Surface *src, SDL_Rect *src_r, GUI_Surface *dst, SDL_Rect *dst_r)
{
	src->Blit(src_r, dst, dst_r);
}

void GUI_SurfaceUpdateRects(GUI_Surface *surface, int n, SDL_Rect *rects)
{
	surface->UpdateRects(n, rects);
}

void GUI_SurfaceUpdateRect(GUI_Surface *surface, int x, int y, int w, int h)
{
	surface->UpdateRect(x,y,w,h);
}

void GUI_SurfaceFill(GUI_Surface *surface, SDL_Rect *r, Uint32 c)
{
	surface->Fill(r, c);
}

int GUI_SurfaceGetWidth(GUI_Surface *surface)
{
	return surface->GetWidth();
}

int GUI_SurfaceGetHeight(GUI_Surface *surface)
{
	return surface->GetHeight();
}

Uint32 GUI_SurfaceMapRGB(GUI_Surface *surface, int r, int g, int b)
{
	return surface->MapRGB(r, g, b);
}

};
