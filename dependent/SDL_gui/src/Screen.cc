#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

#define MAX_UPDATES 200

GUI_Screen::GUI_Screen(const char *aname, SDL_Surface *surface)
: GUI_Drawable(aname, 0, 0, surface->w, surface->h)
{
	screen_surface = new GUI_Surface("screen", surface);
	background = 0;
	contents = 0;
	focus_widget = 0;
	background_color = 0;
}

GUI_Screen::~GUI_Screen(void)
{
	if (background) background->DecRef();
	if (focus_widget) focus_widget->DecRef();
	if (contents) contents->DecRef();
	if (screen_surface) screen_surface->DecRef();
}

GUI_Surface *GUI_Screen::GetSurface(void)
{
	return screen_surface;
}

void GUI_Screen::FlushUpdates(void)
{
}

void GUI_Screen::UpdateRect(const SDL_Rect *r)
{
}

void GUI_Screen::Draw(GUI_Surface *image, const SDL_Rect *src_r, const SDL_Rect *dst_r)
{
	SDL_Rect sr, dr;
	SDL_Rect *srp, *drp;

	assert(image != 0);
	
	if (src_r)
	{
		sr = *src_r;
		srp = &sr;
	}
	else
		srp = NULL;
	if (dst_r)
	{
		dr = *dst_r;
		drp = &dr;
	}
	else
		drp = NULL;

	if (flags & SCREEN_DEBUG_BLIT)
	{
		printf("Screen_draw: %p:", image);
		if (src_r)
			printf("[%d,%d,%d,%d]", srp->x, srp->y, srp->w, srp->h);
		else
			printf("NULL");
		printf(" -> %p:", screen_surface);
		if (drp)
			printf("[%d,%d,%d,%d] (%d,%d)\n", drp->x, drp->y, drp->w, drp->h, drp->x + drp->w, drp->y + drp->h);
		else
			printf("NULL\n");
	}
	image->Blit(srp, screen_surface, drp);
	if (!screen_surface->IsDoubleBuffered())
		UpdateRect(drp);
}

void GUI_Screen::Fill(const SDL_Rect *dst_r, SDL_Color c)
{
	Uint32 color = screen_surface->MapRGB(c.r, c.g, c.b);
	SDL_Rect r = *dst_r;
	
	screen_surface->Fill(&r, color);
	if (!screen_surface->IsDoubleBuffered())
		UpdateRect(&r);
}

void GUI_Screen::Erase(const SDL_Rect *area)
{
	if (background)
		TileImage(background, area, 0, 0);
	else
	{
		SDL_Rect r;
		SDL_Rect *rp;
		if (area)
		{
			r = *area;
			rp = &r;
		}
		else
			rp = NULL;
		screen_surface->Fill(rp, background_color);
	}

	if (!screen_surface->IsDoubleBuffered())
		UpdateRect(area);
}

void GUI_Screen::Update(int force)
{
	if (force)
		Erase(&area);
	
	if (contents)
		contents->DoUpdate(force);

	FlushUpdates();
}

int GUI_Screen::Event(const SDL_Event *event, int xoffset, int yoffset)
{
	if (event->type == SDL_QUIT)
	{
		GUI_SetRunning(0);
		return 1;
	}
	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_ESCAPE)
		{
			GUI_SetRunning(0);
			return 1;
		}
	}
	if (contents)
		if (contents->Event(event, xoffset, yoffset))
			return 1;
	return GUI_Drawable::Event(event, xoffset, yoffset);
}

void GUI_Screen::RemoveWidget(GUI_Widget *widget)
{
	if (widget == contents)
		Keep(&contents, NULL);
}

void GUI_Screen::SetContents(GUI_Widget *widget)
{
	Keep(&contents, widget);
}

void GUI_Screen::SetBackground(GUI_Surface *image)
{
	if (GUI_ObjectKeep((GUI_Object **) &background, image))
		MarkChanged();
}

void GUI_Screen::SetBackgroundColor(SDL_Color c)
{
	Uint32 color;
	
	color = screen_surface->MapRGB(c.r, c.g, c.b);
	if (color != background_color)
	{
		background_color = color;
		MarkChanged();
	}
}

void GUI_Screen::SetFocusWidget(GUI_Widget *widget)
{
	assert(widget != NULL);
	
	if (focus_widget != widget)
	{
		ClearFocusWidget();
		widget->SetFlags(WIDGET_HAS_FOCUS);
		widget->IncRef();
		focus_widget = widget;
	}
}

void GUI_Screen::ClearFocusWidget()
{
	if (focus_widget)
	{
		focus_widget->ClearFlags(WIDGET_HAS_FOCUS);
		focus_widget->DecRef();
		focus_widget = 0;
	}
}

GUI_Widget *GUI_Screen::GetFocusWidget()
{
	return focus_widget;
}

extern "C"
{

GUI_Screen *GUI_ScreenCreate(int w, int h, int d, int f)
{
	d = SDL_VideoModeOK(w, h, d, f);
	if (d == 0)
		return NULL;

	SDL_Surface *temp = SDL_SetVideoMode(w, h, d, f);
	if (temp == NULL)
		return NULL;

	return new GUI_RealScreen("screen", temp);
}

void GUI_ScreenSetContents(GUI_Screen *screen, GUI_Widget *widget)
{
	screen->SetContents(widget);
}

void GUI_ScreenSetBackground(GUI_Screen *screen, GUI_Surface *image)
{
	screen->SetBackground(image);
}

void GUI_ScreenSetFocusWidget(GUI_Screen *screen, GUI_Widget *widget)
{
	screen->SetFocusWidget(widget);
}

void GUI_ScreenClearFocusWidget(GUI_Screen *screen)
{
	screen->ClearFocusWidget();
}

void GUI_ScreenSetBackgroundColor(GUI_Screen *screen, SDL_Color c)
{
	screen->SetBackgroundColor(c);
}

GUI_Widget *GUI_ScreenGetFocusWidget(GUI_Screen *screen)
{
	return screen->GetFocusWidget();
}

}
