#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_ScrollBar::GUI_ScrollBar(const char *aname, int x, int y, int w, int h)
: GUI_Widget(aname, x, y, w, h)
{
	SDL_Rect in;

	in.x = 4;
	in.y = 4;
	in.w = w-8;
	in.h = w-8;
	
	SetTransparent(1);

	background = new GUI_Surface("bg", SDL_SWSURFACE, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	knob = new GUI_Surface("knob", SDL_SWSURFACE, w, w, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	page_step = h / 10;
	position = 0;
	tracking_on = 0;
	tracking_start = 0;
	tracking_pos = 0;
	
	knob->Fill(NULL, 0x00FFFFFF);
	knob->Fill(&in,  0x004040FF);

	background->Fill(NULL, 0x00FFFFFF);
	in.h = h-8;
	background->Fill(&in,  0xFF000000);
	moved_callback = 0;
}

GUI_ScrollBar::~GUI_ScrollBar(void)
{
	knob->DecRef();
	background->DecRef();
	if (moved_callback) moved_callback->DecRef();
}

void GUI_ScrollBar::Update(int force)
{
	if (parent == 0)
		return;
	
	if (force)
	{
		if (flags & WIDGET_TRANSPARENT)
			parent->Erase(&area);
		
		if (background)
			parent->Draw(background, NULL, &area);
		
		if (knob)
		{
			SDL_Rect sr, dr;
		
			sr.w = dr.w = knob->GetWidth();
			sr.h = dr.h = knob->GetHeight();
			
			dr.x = area.x;
			dr.y = area.y + position;
			sr.x = 0;
			sr.y = 0;
			
			parent->Draw(knob, &sr, &dr);
		}
	}
}

void GUI_ScrollBar::Erase(const SDL_Rect *rp)
{
	SDL_Rect dest;
	
	assert(parent != NULL);
	assert(rp != NULL);
	
	dest = Adjust(rp);
	
	if (flags & WIDGET_TRANSPARENT)
		parent->Erase(&dest);
	if (background)
		parent->TileImage(background, &dest, 0, 0);
}

int GUI_ScrollBar::Event(const SDL_Event *event, int xoffset, int yoffset)
{
	switch (event->type)
	{
		case SDL_MOUSEMOTION:
		{
			int y = event->motion.y - area.y;
			if (tracking_on)
			{
				position = tracking_pos + y - tracking_start;
				if (position < 0)
					position = 0;
				if (position > area.h - knob->GetHeight())
					position = area.h - knob->GetHeight();
				MarkChanged();
				if (moved_callback)
					moved_callback->Call(this);
				return 1;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (flags & WIDGET_INSIDE)
			{
				int y = event->button.y - area.y;

				/* if the cursor is inside the knob, start tracking */
				if (y >= position && y < position + knob->GetHeight())
				{
					tracking_on = 1;
					tracking_start = y;
					tracking_pos = position;
				}
				return 1;
			}
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			int y = event->button.y - area.y;
			if (tracking_on)
			{
				tracking_on = 0;
			}
			else if (flags & WIDGET_INSIDE)
			{
				/* up */
				if (y < position)
				{
					position -= page_step;
					if (position < 0)
						position = 0;
				}
				/* down */
				else if (y >= position + knob->GetHeight())
				{
					position += page_step;
					if (position > area.h - knob->GetHeight())
						position = area.h - knob->GetHeight();
				}
				if (moved_callback)
					moved_callback->Call(this);
				MarkChanged();
				return 1;
			}
			break;
		}
	}
	return GUI_Drawable::Event(event, xoffset, yoffset);
}

void GUI_ScrollBar::SetKnobImage(GUI_Surface *image)
{
	if (GUI_ObjectKeep((GUI_Object **) &knob, image))
		MarkChanged();
}

void GUI_ScrollBar::SetBackgroundImage(GUI_Surface *image)
{
	if (GUI_ObjectKeep((GUI_Object **) &background, image))
		MarkChanged();
}

int GUI_ScrollBar::GetPosition(void)
{
	return position;
}

void GUI_ScrollBar::SetPosition(int value)
{
	// FIXME not implemented
}

void GUI_ScrollBar::SetPageStep(int value)
{
	// FIXME not implemented
}

void GUI_ScrollBar::SetMovedCallback(GUI_Callback *callback)
{
	GUI_ObjectKeep((GUI_Object **) &moved_callback, callback);
}

extern "C"
{

GUI_Widget *GUI_ScrollBarCreate(const char *name, int x, int y, int w, int h)
{
	return new GUI_ScrollBar(name, x, y, w, h);
}

int GUI_ScrollBarCheck(GUI_Widget *widget)
{
	// FIXME not implemented
	return 0;
}

void GUI_ScrollBarSetKnobImage(GUI_Widget *widget, GUI_Surface *image)
{
	((GUI_ScrollBar *) widget)->SetKnobImage(image);
}

void GUI_ScrollBarSetBackgroundImage(GUI_Widget *widget, GUI_Surface *image)
{
	((GUI_ScrollBar *) widget)->SetBackgroundImage(image);
}

void GUI_ScrollBarSetPosition(GUI_Widget *widget, int value)
{
	((GUI_ScrollBar *) widget)->SetPosition(value);
}

void GUI_ScrollBarSetPageStep(GUI_Widget *widget, int value)
{
	((GUI_ScrollBar *) widget)->SetPageStep(value);
}

void GUI_ScrollBarSetMovedCallback(GUI_Widget *widget, GUI_Callback *callback)
{
	((GUI_ScrollBar *) widget)->SetMovedCallback(callback);
}

int GUI_ScrollBarGetPosition(GUI_Widget *widget)
{
	return ((GUI_ScrollBar *) widget)->GetPosition();
}

}
