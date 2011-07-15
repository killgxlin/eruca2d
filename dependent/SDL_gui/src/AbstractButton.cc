#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_AbstractButton::GUI_AbstractButton(const char *aname, int x, int y, int w, int h)
: GUI_Widget(aname, x, y, w, h)
{
	SetTransparent(1);
	caption = 0;
	click = 0;
}

GUI_AbstractButton::~GUI_AbstractButton()
{
	if (caption) caption->DecRef();
	if (click) click->DecRef();	
}

GUI_Surface *GUI_AbstractButton::GetCurrentImage()
{
	return 0;
}

void GUI_AbstractButton::Update(int force)
{
	if (parent==0)
		return;

	if (force)
	{
		GUI_Surface *surface = GetCurrentImage();
	
		if (flags & WIDGET_TRANSPARENT)
			parent->Erase(&area);
		if (surface)
			parent->Draw(surface, NULL, &area);
	}			

	if (caption)
		caption->DoUpdate(force);
}

void GUI_AbstractButton::Fill(const SDL_Rect *dr, SDL_Color c)
{
}

void GUI_AbstractButton::Erase(const SDL_Rect *dr)
{
}

void GUI_AbstractButton::Notify(int mask)
{
	MarkChanged();
	GUI_Drawable::Notify(mask);
}

void GUI_AbstractButton::Clicked(int x, int y)
{
	if (click)
		click->Call(this);
}

void GUI_AbstractButton::RemoveWidget(GUI_Widget *widget)
{
	if (widget == caption)
		Keep(&caption, NULL);
}

void GUI_AbstractButton::SetCaption(GUI_Widget *widget)
{
	Keep(&caption, widget);
}

void GUI_AbstractButton::SetClick(GUI_Callback *callback)
{
	GUI_ObjectKeep((GUI_Object **) &click, callback);
}
