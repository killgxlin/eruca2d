#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_Label::GUI_Label(const char *aname, int x, int y, int w, int h, GUI_Font *afont, const char *s)
: GUI_Widget(aname, x, y, w, h), font(afont)
{
	SetTransparent(1);
	
	textcolor.r = 255;
	textcolor.g = 255;
	textcolor.b = 255;

	font->IncRef();
	text = font->RenderQuality(s, textcolor);
}

GUI_Label::~GUI_Label()
{
	text->DecRef();
	font->DecRef();
}

void GUI_Label::Update(int force)
{
	if (parent==0)
		return;
	
	if (force)
	{
		if (flags & WIDGET_TRANSPARENT)
			parent->Erase(&area);
		if (text)
		{
			SDL_Rect dr;
			SDL_Rect sr;
			SDL_Rect clip;
			
			clip = parent->GetArea();
			sr.w = dr.w = text->GetWidth();
			sr.h = dr.h = text->GetHeight();
			clip.x = clip.y = sr.x = sr.y = 0;
			
			switch (flags & WIDGET_HORIZ_MASK)
			{
				case WIDGET_HORIZ_CENTER:
				case WIDGET_HORIZ_STRETCH: /* FIXME: stretch not implemented */
					dr.x = area.x + (area.w - dr.w) / 2;
					break;
				case WIDGET_HORIZ_LEFT:
					dr.x = area.x;
					break;
				case WIDGET_HORIZ_RIGHT:
					dr.x = area.x + area.w - dr.w;
					break;
			}
			switch (flags & WIDGET_VERT_MASK)
			{
				case WIDGET_VERT_CENTER:
				case WIDGET_VERT_STRETCH: /* FIXME: stretch not implemented */
					dr.y = area.y + (area.h - dr.h) / 2;
					break;
				case WIDGET_VERT_TOP:
					dr.y = area.y;
					break;
				case WIDGET_VERT_BOTTOM:
					dr.y = area.y + area.h - dr.h;
					break;
			}
			if (GUI_ClipRect(&sr, &dr, &clip))
				parent->Draw(text, &sr, &dr);
		}
	}
}

void GUI_Label::SetFont(GUI_Font *afont)
{
	if (GUI_ObjectKeep((GUI_Object **) &font, afont))
		MarkChanged();
	// FIXME: should re-draw the text
}

void GUI_Label::SetTextColor(int r, int g, int b)
{
	textcolor.r = r;
	textcolor.g = g;
	textcolor.b = b;
	/* FIXME: should re-draw the text in the new color */
}

void GUI_Label::SetText(const char *s)
{
	if (text) text->DecRef();
	text = font->RenderQuality(s, textcolor);
	MarkChanged();
}

extern "C"
{

GUI_Widget *GUI_LabelCreate(const char *name, int x, int y, int w, int h, GUI_Font *font, const char *text)
{
	return new GUI_Label(name, x, y, w, h, font, text);
}

int GUI_LabelCheck(GUI_Widget *widget)
{
	// FIXME not implemented
	return 0;
}

void GUI_LabelSetFont(GUI_Widget *widget, GUI_Font *font)
{
	((GUI_Label *) widget)->SetFont(font);
}
void GUI_LabelSetTextColor(GUI_Widget *widget, int r, int g, int b)
{
	((GUI_Label *) widget)->SetTextColor(r,g,b);
}

void GUI_LabelSetText(GUI_Widget *widget, const char *text)
{
	((GUI_Label *) widget)->SetText(text);
}

}
