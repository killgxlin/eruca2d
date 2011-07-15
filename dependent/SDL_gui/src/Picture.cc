#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_Picture::GUI_Picture(const char *aname, int x, int y, int w, int h, GUI_Surface *an_image)
: GUI_Widget(aname, x, y, w, h), image(an_image)
{
	SetTransparent(1);
	image->IncRef();
	caption = 0;
}

GUI_Picture::~GUI_Picture()
{
	image->DecRef();
	if (caption) caption->DecRef();
}

void GUI_Picture::Update(int force)
{
	if (parent == 0)
		return;
	
	if (force)
	{
		if (flags & WIDGET_TRANSPARENT)
			parent->Erase(&area);
		if (image)
		{
			SDL_Rect r;
				
			r.w = image->GetWidth();
			r.h = image->GetHeight();
			r.x = area.x + (area.w - r.w) / 2;
			r.y = area.y + (area.h - r.h) / 2;
			parent->Draw(image, NULL, &r);
		}
	}
	if (caption)
		caption->DoUpdate(force);
}

void GUI_Picture::Erase(const SDL_Rect *rp)
{
	SDL_Rect dest = Adjust(rp);
	
	if (flags & WIDGET_TRANSPARENT)
		parent->Erase(&dest);
		
	if (image)
	{
		SDL_Rect sr, dr;

		sr.w = dr.w = image->GetWidth();
		sr.h = dr.h = image->GetHeight();
		dr.x = area.x + (area.w - dr.w) / 2;
		dr.y = area.y + (area.h - dr.h) / 2;
		sr.x = sr.y = 0;
		
		if (GUI_ClipRect(&sr, &dr, &dest))
			parent->Draw(image, &sr, &dr);
	}
}

int GUI_Picture::Event(const SDL_Event *event, int xoffset, int yoffset)
{
	if (caption)
	{
		if (caption->Event(event, xoffset + area.x, yoffset + area.y))
			return 1;
	}
	return GUI_Widget::Event(event, xoffset, yoffset);
}

void GUI_Picture::SetImage(GUI_Surface *an_image)
{
	if (GUI_ObjectKeep((GUI_Object **) &image, an_image))
		MarkChanged();
}

void GUI_Picture::SetCaption(GUI_Widget *a_caption)
{
	Keep(&caption, a_caption);
}

extern "C"
{

GUI_Widget *GUI_PictureCreate(const char *name, int x, int y, int w, int h, GUI_Surface *image)
{
	return new GUI_Picture(name, x, y, w, h, image);
}

int GUI_PictureCheck(GUI_Widget *widget)
{
	// FIXME not implemented
	return 0;
}

void GUI_PictureSetImage(GUI_Widget *widget, GUI_Surface *image)
{
	((GUI_Picture *) widget)->SetImage(image);
}

void GUI_PictureSetCaption(GUI_Widget *widget, GUI_Widget *caption)
{
	((GUI_Picture *) widget)->SetCaption(caption);
}

}
