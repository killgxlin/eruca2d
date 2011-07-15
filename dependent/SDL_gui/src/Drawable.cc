#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

static int Inside(int x, int y, SDL_Rect *r)
{
	return (x >= r->x) && (x < r->x + r->w) && (y >= r->y) && (y < r->y + r->h);
}

GUI_Drawable::GUI_Drawable(const char *aname, int x, int y, int w, int h)
: GUI_Object(aname)
{
	flags = 0;
	status_callback = 0;
	area.x = x;
	area.y = y;
	area.w = w;
	area.h = h;
}

GUI_Drawable::~GUI_Drawable(void)
{
}

int GUI_Drawable::GetWidth(void)
{
	return area.w;
}

int GUI_Drawable::GetHeight(void)
{
	return area.h;
}

SDL_Rect GUI_Drawable::Adjust(const SDL_Rect *rp)
{
	SDL_Rect r;
	
	assert(rp != NULL);
	
	r.x = rp->x + area.x;
	r.y = rp->y + area.y;
	r.w = rp->w;
	r.h = rp->h;
	return r;
}

void GUI_Drawable::Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr)
{
}

void GUI_Drawable::Fill(const SDL_Rect *dr, SDL_Color c)
{
}

void GUI_Drawable::Erase(const SDL_Rect *dr)
{
}

void GUI_Drawable::RemoveWidget(GUI_Widget *widget)
{
}

void GUI_Drawable::Notify(int mask)
{
	flag_delta = mask;
	if (status_callback)
		status_callback->Call(this);
}

void GUI_Drawable::WriteFlags(int andmask, int ormask)
{
	int oldflags = flags;	
	flags = (flags & andmask) | ormask;
	if (flags != oldflags)
		Notify(flags ^ oldflags);
}

void GUI_Drawable::SetFlags(int mask)
{
	WriteFlags(-1, mask);
}

void GUI_Drawable::ClearFlags(int mask)
{
	WriteFlags(~mask, 0);
}

int GUI_Drawable::Event(const SDL_Event *event, int xoffset, int yoffset)
{
/*	GUI_Drawable *screen = 0;  FIXME GUI_GetScreen(); */
	GUI_Drawable *focus = 0; /* FIXME screen->GetFocusWidget(); */

	switch (event->type)
	{
/*		
		case SDL_KEYDOWN:
			return GUI_DrawableKeyPressed(object, event->key.keysym.sym, event->key.keysym.unicode);
		case SDL_KEYUP:
			return GUI_DrawableKeyReleased(object, event->key.keysym.sym, event->key.keysym.unicode);
*/
		case SDL_MOUSEBUTTONDOWN:
		{
			int x = event->button.x - xoffset;
			int y = event->button.y - yoffset;
			if ((flags & WIDGET_DISABLED) == 0)
				if (Inside(x, y, &area))
					if (focus == 0 || focus == this)
						SetFlags(WIDGET_PRESSED);
			break;
		}		
		case SDL_MOUSEBUTTONUP:
		{
			int x = event->button.x - xoffset;
			int y = event->button.y - yoffset;
			if ((flags & WIDGET_DISABLED) == 0)
			{
				if (flags & WIDGET_PRESSED)
					if (Inside(x, y, &area))
						if (focus == 0 || focus == this)
							Clicked(x, y);
			}
			if (flags & WIDGET_PRESSED)
				ClearFlags(WIDGET_PRESSED);
			break;
		}
		case SDL_MOUSEMOTION:
		{		
			int x = event->motion.x - xoffset;
			int y = event->motion.y - yoffset;
			if (focus == 0 || focus == this)
			{
				if ((flags & WIDGET_DISABLED) == 0 && Inside(x, y, &area))
					SetFlags(WIDGET_INSIDE);
				else
					ClearFlags(WIDGET_INSIDE);
			}
			break;
		}
	}
	return 0;
}

void GUI_Drawable::Clicked(int x, int y)
{
}

void GUI_Drawable::Update(int force)
{
}

void GUI_Drawable::DoUpdate(int force)
{
	if (flags & WIDGET_CHANGED)
		force = 1;
	Update(force);
	flags &= ~WIDGET_CHANGED;
}

/* mark as changed so it will be drawn in the next update */

void GUI_Drawable::MarkChanged()
{
	flags |= WIDGET_CHANGED;
}

/* tile an image over an area on a widget */

void GUI_Drawable::TileImage(GUI_Surface *surface, const SDL_Rect *rp, int x_offset, int y_offset)
{
	SDL_Rect sr, dr;
	int xp, yp, bw, bh;

	assert(surface != NULL);
	assert(rp != NULL);
	
	bw = surface->GetWidth();
	bh = surface->GetHeight();
	
	for (xp=0; xp < rp->w; xp += sr.w)
	{
		dr.x = rp->x+xp;
		sr.x = (dr.x + x_offset) % bw;
		sr.w = dr.w = bw - sr.x;
		if (dr.x + dr.w > rp->x + rp->w)
			sr.w = dr.w = rp->x + rp->w - dr.x;
		for (yp=0; yp < rp->h; yp += sr.h)
		{
			dr.y = rp->y+yp;
			sr.y = (dr.y + y_offset) % bh;
			sr.h = dr.h = bh - sr.y;
			if (dr.y + dr.h > rp->y + rp->h)
				sr.h = dr.h = rp->y + rp->h - dr.y;
			Draw(surface, &sr, &dr);
		}
	}
}

void GUI_Drawable::Keep(GUI_Widget **target, GUI_Widget *source)
{
	if (*target != source)
	{
		if (source)
			source->IncRef();
		if (*target)
		{
			(*target)->SetParent(0);
			(*target)->DecRef();
		}
		if (source)
			source->SetParent(this);
		(*target) = source;
		MarkChanged();
	}
}

SDL_Rect GUI_Drawable::GetArea()
{
	return area;
}

void GUI_Drawable::SetPosition(int x, int y)
{
	area.x = x;
	area.y = y;
}

void GUI_Drawable::SetStatusCallback(GUI_Callback *callback)
{
	GUI_ObjectKeep((GUI_Object **) &status_callback, callback);
}

int GUI_Drawable::GetFlagDelta(void)
{
	return flag_delta;
}

int GUI_Drawable::GetFlags(void)
{
	return flags;
}
