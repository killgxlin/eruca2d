#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

#define MAX_UPDATES 200

GUI_RealScreen::GUI_RealScreen(const char *aname, SDL_Surface *surface)
: GUI_Screen(aname, surface)
{
	n_updates = 0;
	updates = new SDL_Rect[MAX_UPDATES];
}

GUI_RealScreen::~GUI_RealScreen(void)
{
	delete [] updates;
}

void GUI_RealScreen::FlushUpdates(void)
{
	if (n_updates)
	{
		screen_surface->UpdateRects(n_updates, updates);
		n_updates = 0;
	}
}

/* return true if r1 is inside r2 */

static int inside(const SDL_Rect *r1, const SDL_Rect *r2)
{
	return
		r1->x >= r2->x && r1->x + r1->w <= r2->x + r2->w &&
		r1->y >= r2->y && r1->y + r1->h <= r2->y + r2->h;
}

void GUI_RealScreen::UpdateRect(const SDL_Rect *r)
{
	if (r->x < 0 || r->y < 0 || 
		r->x + r->w > screen_surface->GetWidth() ||
		r->y + r->h > screen_surface->GetHeight())
	{
		fprintf(stderr, "Bad UpdateRect x=%d y=%d w=%d h=%d screen w=%d h=%d\n", 
			r->x, r->y, r->w, r->h,
			screen_surface->GetWidth(), screen_surface->GetHeight());
		abort();
	}
		
	int i;
	for (i=0; i<n_updates; i++)
	{
		/* if the new rect is inside one that is already */
		/* being updated, then just ignore it. */
		if (inside(r, &updates[i]))
		   	return;
		/* if the new rect contains a rect already being */
		/* updated, then replace the new rect. */
		/* FIXME: ideally, it should remove all rects which are inside */		 	
		if (inside(&updates[i], r))
		{
			updates[i] = *r;
			return;
	 	}
	 }

	updates[n_updates++] = *r;
	if (n_updates >= MAX_UPDATES)
		FlushUpdates();
}

void GUI_RealScreen::Update(int force)
{
	if (screen_surface->IsDoubleBuffered())
		force = 1;

	GUI_Screen::Update(force);

	if (screen_surface->IsDoubleBuffered())
		screen_surface->Flip();
}

