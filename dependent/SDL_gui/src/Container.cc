#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_gui.h"

#define WIDGET_LIST_SIZE 16
#define WIDGET_LIST_INCR 16

GUI_Container::GUI_Container(const char *aname, int x, int y, int w, int h)
: GUI_Widget(aname, x, y, w, h)
{
	SetFlags(WIDGET_TRANSPARENT);
	n_widgets = 0;
	s_widgets = WIDGET_LIST_SIZE;
	widgets = new GUI_Widget *[s_widgets];
	x_offset = 0;
	y_offset = 0;
	background = 0;
	bgcolor.r = bgcolor.g = bgcolor.b = 0;
}

GUI_Container::~GUI_Container(void)
{
	if (background) background->DecRef();
	while (n_widgets)
		RemoveWidget(widgets[n_widgets-1]);
}

int GUI_Container::ContainsWidget(GUI_Widget *widget)
{
	if (!widget)
		return 0;
	
	int i;
	for (i=0; i<n_widgets; i++)
		if (widgets[i] == widget)
			return 1;
	return 0;
}

void GUI_Container::AddWidget(GUI_Widget *widget)
{
	if (!widget || ContainsWidget(widget))
		return;

	// IncRef early, to prevent reparenting from freeing the child
	widget->IncRef();
	
	// reparent if necessary
	GUI_Drawable *parent = widget->GetParent();
	if (parent)
		parent->RemoveWidget(widget);
	widget->SetParent(this);

	// expand the array if necessary
	if (n_widgets >= s_widgets)
	{
		int i;
		GUI_Widget **new_widgets;
		
		s_widgets += WIDGET_LIST_INCR;
		new_widgets = new GUI_Widget *[s_widgets];

		for (i=0; i<n_widgets; i++)
			new_widgets[i] = widgets[i];
		delete [] widgets;
		widgets = new_widgets;
	}
	widgets[n_widgets++] = widget;
	UpdateLayout();
}

void GUI_Container::RemoveWidget(GUI_Widget *widget)
{
	int i, j;
	
	assert(widget->GetParent() == this);

	widget->SetParent(0);
		
	for (i=0, j=0; i<n_widgets; i++)
	{
		if (widgets[i] == widget)
			widget->DecRef();
		else
			widgets[j++] = widgets[i];
	}
	n_widgets = j;
	UpdateLayout();
}

void GUI_Container::UpdateLayout(void)
{
}

int GUI_Container::GetWidgetCount()
{
	return n_widgets;	
}

GUI_Widget *GUI_Container::GetWidget(int index)
{
	if (index <0 || index >= n_widgets)
		return NULL;
	return widgets[index];
}

void GUI_Container::Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr)
{
	if (parent)
	{
		SDL_Rect dest = Adjust(dr);
		SDL_Rect src;
		
		if (sr != NULL)
			src = *sr;
		else
		{
			src.x = src.y = 0;
			src.w = image->GetWidth();
			src.h = image->GetHeight();
		}
		
		dest.x -= x_offset;
		dest.y -= y_offset;
		if (GUI_ClipRect(&src, &dest, &area))
			parent->Draw(image, &src, &dest);
	}
}

void GUI_Container::Fill(const SDL_Rect *dr, SDL_Color c)
{
	if (parent)
	{
		SDL_Rect dest = Adjust(dr);
		dest.x -= x_offset;
		dest.y -= y_offset;
		parent->Fill(&dest, c);
	}
}

void GUI_Container::Erase(const SDL_Rect *rp)
{
	if (parent)
	{
		assert(rp != NULL);

		SDL_Rect dest = Adjust(rp);
		dest.x -= x_offset;
		dest.y -= y_offset;
	
		if (GUI_ClipRect(NULL, &dest, &area))
		{
			if (flags & WIDGET_TRANSPARENT)
				parent->Erase(&dest);
	
			if (background)
				parent->TileImage(background, &dest, x_offset, y_offset);
			else if ((flags & WIDGET_TRANSPARENT) == 0)
				parent->Fill(&dest, bgcolor);
		}
	}
}

void GUI_Container::SetBackground(GUI_Surface *surface)
{
	if (GUI_ObjectKeep((GUI_Object **) &background, surface))
		MarkChanged();
}

void GUI_Container::SetBackgroundColor(SDL_Color c)
{
	bgcolor = c;
	MarkChanged();
}

void GUI_Container::SetXOffset(int value)
{
	x_offset = value;
	MarkChanged();
}

void GUI_Container::SetYOffset(int value)
{
	y_offset = value;
	MarkChanged();
}

extern "C"
{

int GUI_ContainerContains(GUI_Widget *container, GUI_Widget *widget)
{
	return ((GUI_Container *) container)->ContainsWidget(widget);
}

void GUI_ContainerAdd(GUI_Widget *container, GUI_Widget *widget)
{
	return ((GUI_Container *) container)->AddWidget(widget);
}

void GUI_ContainerRemove(GUI_Widget *container, GUI_Widget *widget)
{
	return ((GUI_Container *) container)->RemoveWidget(widget);
}

int GUI_ContainerGetCount(GUI_Widget *container)
{
	return ((GUI_Container *) container)->GetWidgetCount();
}

GUI_Widget *GUI_ContainerGetChild(GUI_Widget *container, int index)
{
	return ((GUI_Container *) container)->GetWidget(index);
}

}
