#include <stdlib.h>
#include "SDL_gui.h"
#include "SDL_thread.h"

static GUI_Font *font;

typedef struct
{
	const char *name;
	GUI_Surface *surface;
} ImageEntry;

ImageEntry images[] =
{
	{ "md-button-normal.png",    0 },
#define BUTTON_NORMAL images[0].surface
	{ "md-button-highlight.png",    0 },
#define BUTTON_HIGHLIGHT images[1].surface
	{ "md-button-pressed.png",    0 },
#define BUTTON_PRESSED images[2].surface
	{ "md-button-disabled.png",    0 },
#define BUTTON_DISABLED images[3].surface
	{ "floppy-off-normal.png",    0 },
#define FLOPPY_ON_NORMAL images[4].surface
	{ "floppy-off-highlight.png",    0 },
#define FLOPPY_ON_HIGHLIGHT images[5].surface
	{ "floppy-on-normal.png",    0 },
#define FLOPPY_OFF_NORMAL images[6].surface
	{ "floppy-on-highlight.png",    0 },
#define FLOPPY_OFF_HIGHLIGHT images[7].surface
	{ "sm-button-normal.png",    0 },
#define SM_BUTTON_NORMAL images[8].surface
	{ "sm-button-highlight.png",    0 },
#define SM_BUTTON_HIGHLIGHT images[9].surface
	{ "sm-button-pressed.png",    0 },
#define SM_BUTTON_PRESSED images[10].surface
	{ "sm-button-disabled.png",    0 },
#define SM_BUTTON_DISABLED images[11].surface
	{ "tiny-button.png",    0 },
#define TINY_BUTTON images[12].surface
	{ 0, 0 }
};

int LoadImages()
{
	ImageEntry *entry = images;
	while (entry->name != NULL)
	{
		entry->surface = GUI_SurfaceLoad(entry->name);
		if (entry->surface == NULL)
			return -1;
		entry++;
	}
	return 0;
}

GUI_Widget *SmallButton(const char *name, int x, int y, GUI_Font *font, char *text, GUI_CallbackFunction *func, void *data)
{
	GUI_Widget *button = GUI_ButtonCreate(name, x, y, 100, 50);
	if (font != NULL && text != NULL)
	{
		GUI_Widget *label = GUI_LabelCreate("label", 0, 0, 100, 50, font, text);
		GUI_ButtonSetCaption(button, label);
		GUI_ObjectDecRef((GUI_Object *) label);		
	}
	if (func != NULL)
	{
		GUI_Callback *cb = GUI_CallbackCreate(func, NULL, data);
		GUI_ButtonSetClick(button, cb);
		GUI_ObjectDecRef((GUI_Object *) cb);
	}
	GUI_ButtonSetNormalImage(button, SM_BUTTON_NORMAL);
	GUI_ButtonSetHighlightImage(button, SM_BUTTON_HIGHLIGHT);
	GUI_ButtonSetPressedImage(button, SM_BUTTON_PRESSED);
	GUI_ButtonSetDisabledImage(button, SM_BUTTON_DISABLED);
	
	return button;
}

static SDL_Thread *progress_thread;

static double progress = 0.0;
static GUI_Widget *progress_bar = NULL;

static int progress_function(void *ptr)
{
	while (progress_bar != NULL)
	{
		progress += 0.01;
		if (progress > 1.0)
			progress = 0;
		GUI_Lock();
		if (progress_bar != NULL)
			GUI_ProgressBarSetPosition(progress_bar, progress);
		GUI_Unlock();
		GUI_TriggerUpdate();
		
		SDL_Delay(50);
	}
	return 0;
}

GUI_Widget *CreatePage1(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("box", x, y, w, h);
	GUI_Widget *button;
	GUI_Widget *label;
	int i, j;
	
	/* put a label at the top of the page */

	label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 1: A Bunch of Buttons");
	GUI_ContainerAdd(box, label);	
	GUI_ObjectDecRef((GUI_Object *) label);

	for (i=0; i<6; i++)
	{
		for (j=0; j<3; j++)
		{
			/* create a button and add it to the page */
			
			button = GUI_ButtonCreate("button", 20+i*100, 50+j*100, 100, 100);
			
			if (i==j)
				GUI_WidgetSetEnabled(button, 0);
			GUI_ButtonSetNormalImage(button, BUTTON_NORMAL);
			GUI_ButtonSetHighlightImage(button, BUTTON_HIGHLIGHT);
			GUI_ButtonSetPressedImage(button, BUTTON_PRESSED);
			GUI_ButtonSetDisabledImage(button, BUTTON_DISABLED);
			
			GUI_ContainerAdd(box, button);
			GUI_ObjectDecRef((GUI_Object *) button);
		}
	}
	return box;
}

GUI_Widget *CreatePage2(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("box", x, y, w, h);
	GUI_Widget *label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 2: A Picture and Some Labels");
	GUI_Surface *image;
	GUI_Widget *picture;

	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);

	image = GUI_SurfaceLoad("picture1.jpg");	
	picture = GUI_PictureCreate("picture", 0, 50, w, 300, image);
	GUI_ContainerAdd(box, picture);
	GUI_ObjectDecRef((GUI_Object *) picture);
	GUI_ObjectDecRef((GUI_Object *) image);
	
	label = GUI_LabelCreate("label", 0, 350, w, 40, font, "Lake Superior Shoreline");
	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);
	
	label = GUI_LabelCreate("label", 0, 390, w, 40, font, "Copper Harbor, MI");
	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);
	
	return box;
}

GUI_Widget *CreatePage3(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("box", x, y, w, h);
	GUI_Widget *label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 3: Some Toggle Buttons");
	GUI_Widget *button;

	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);

	button = GUI_ToggleButtonCreate("button", 100, 100, 80, 80);
	GUI_ToggleButtonSetOnNormalImage(button, FLOPPY_ON_NORMAL);
	GUI_ToggleButtonSetOnHighlightImage(button, FLOPPY_ON_HIGHLIGHT);
	GUI_ToggleButtonSetOffNormalImage(button, FLOPPY_OFF_NORMAL);
	GUI_ToggleButtonSetOffHighlightImage(button, FLOPPY_OFF_HIGHLIGHT);
	GUI_WidgetSetState(button, 0);

	GUI_ContainerAdd(box, button);
	GUI_ObjectDecRef((GUI_Object *) button);

	button = GUI_ToggleButtonCreate("button", 100, 200, 80, 80);
	GUI_ToggleButtonSetOnNormalImage(button, FLOPPY_ON_NORMAL);
	GUI_ToggleButtonSetOnHighlightImage(button, FLOPPY_ON_HIGHLIGHT);
	GUI_ToggleButtonSetOffNormalImage(button, FLOPPY_OFF_NORMAL);
	GUI_ToggleButtonSetOffHighlightImage(button, FLOPPY_OFF_HIGHLIGHT);
	GUI_WidgetSetState(button, 1);

	GUI_ContainerAdd(box, button);
	GUI_ObjectDecRef((GUI_Object *) button);

	return box;
}

GUI_Widget *CreatePage4(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("box", x, y, w, h);
	GUI_Widget *label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 4: A Text Entry and a Progress Bar");
	GUI_Widget *widget;

	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);

	widget = GUI_TextEntryCreate("button", 100, 100, 200, 50, font, 20);
	GUI_TextEntrySetText(widget, "Text1");
	GUI_ContainerAdd(box, widget);
	GUI_ObjectDecRef((GUI_Object *) widget);
	
	progress_bar = GUI_ProgressBarCreate("button", 100, 300, 200, 30);
	GUI_ContainerAdd(box, progress_bar);

	progress_thread = SDL_CreateThread(progress_function, NULL);
	
	return box;
}

GUI_Widget *scrollpanel;
GUI_Widget *scrollbar;

void scrollbar_moved(void *ptr)
{
	int p = GUI_ScrollBarGetPosition(scrollbar);
	GUI_PanelSetYOffset(scrollpanel, p);
}

GUI_Widget *CreatePage5(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("box", x, y, w, h);
	GUI_Widget *label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 5: Scroll Bar and Attached Panel");
	GUI_Widget *widget;
	GUI_Callback *cb;
	GUI_Surface *bg;
	int i;
	
	GUI_ContainerAdd(box, label);
	GUI_ObjectDecRef((GUI_Object *) label);

	scrollpanel = GUI_PanelCreate("panel", 200, 100, 200, 200);
	GUI_ContainerAdd(box, scrollpanel);
	
	widget = GUI_LabelCreate("label", 50, 0, 100, 50, font, "label");
	GUI_ContainerAdd(scrollpanel, widget);
	GUI_ObjectDecRef((GUI_Object *) widget);
		
	for (i=0; i<5; i++)
	{
		widget = SmallButton("test", 50, 50+i*50, font, "Test", NULL, NULL);
		GUI_ContainerAdd(scrollpanel, widget);
		GUI_ObjectDecRef((GUI_Object *) widget);
	}

	scrollbar = GUI_ScrollBarCreate("scrollbar", 400, 100, 30, 200);
	GUI_ScrollBarSetKnobImage(scrollbar, TINY_BUTTON);
	
	cb = GUI_CallbackCreate(scrollbar_moved, NULL, NULL);
	GUI_ScrollBarSetMovedCallback(scrollbar, cb);
	GUI_ObjectDecRef((GUI_Object *) cb);
	
	GUI_ContainerAdd(box, scrollbar);
	
	bg = GUI_SurfaceLoad("bg2.jpg");
	GUI_PanelSetBackground(scrollpanel, bg);
	GUI_ObjectDecRef((GUI_Object *) bg);	
		
	return box;
}

GUI_Widget *CreatePage6(int x, int y, int w, int h)
{
	GUI_Widget *page = GUI_PanelCreate("page", x, y, w, h);
	GUI_Widget *box;
	GUI_Widget *button;
	GUI_Widget *label;
	GUI_Layout *layout;
	char text[32];
	int i;
	
	/* put a label at the top of the page */

	label = GUI_LabelCreate("label", 0, 0, w, 50, font, "Page 6: A Panel With VBox Layout");
	GUI_ContainerAdd(page, label);	
	GUI_ObjectDecRef((GUI_Object *) label);

	box = GUI_PanelCreate("box", 0, 50, w, h-50);
	GUI_ContainerAdd(page, box);

	layout = GUI_VBoxLayoutCreate();
	GUI_PanelSetLayout(box, layout);
	GUI_ObjectDecRef((GUI_Object *) layout);

	for (i=0; i<6; i++)
	{
		sprintf(text, "Button %d", i+1);
		
		button = SmallButton(text, 0, 0, font, text, NULL, NULL);
		GUI_ContainerAdd(box, button);
		GUI_ObjectDecRef((GUI_Object *) button);
	}
	GUI_ObjectDecRef((GUI_Object *) box);
	return page;
}

GUI_Widget *CreateContents(int x, int y, int w, int h)
{
	GUI_Widget *box = GUI_PanelCreate("main", x, y, w, h);
	GUI_Widget *cards;
	GUI_Widget *button;
	GUI_Widget *page;

	/* create the card panel */
	
	cards = GUI_CardStackCreate("cards", 0, 0, w, h-50);
	GUI_ContainerAdd(box, cards);

	page = CreatePage1(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);
	
	page = CreatePage2(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);

	page = CreatePage3(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);

	page = CreatePage4(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);
		
	page = CreatePage5(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);

	page = CreatePage6(0, 0, w, h-50);
	GUI_ContainerAdd(cards, page);
	GUI_ObjectDecRef((GUI_Object *) page);

/*	GUI_CardStackShowIndex(cards, 4); */
	
	/* create the "next" button */

	button = SmallButton("next", w-100, h-50, font, "Next Page", (GUI_CallbackFunction *) GUI_CardStackNext, cards);
	GUI_ContainerAdd(box, button);
	GUI_ObjectDecRef((GUI_Object *) button);
	
	/* create the "prev" button */

	button = SmallButton("prev", 0, h-50, font, "Prev Page", (GUI_CallbackFunction *) GUI_CardStackPrev, cards);
	GUI_ContainerAdd(box, button);
	GUI_ObjectDecRef((GUI_Object *) button);
	
	/* release temp ref */
	
	GUI_ObjectDecRef((GUI_Object *) cards);
		
	return box;
}

int init(int w, int h)
{	
	GUI_Screen *screen = GUI_GetScreen();
	GUI_Widget *mainpage;
	GUI_Surface *background;
	
	font = GUI_FontLoadTrueType("bboron.ttf", 18);
	if (font == NULL)
		return -1;

	if (LoadImages() < 0)
		return -1;

	/* load the background */

	background = GUI_SurfaceLoad("background.jpg");
	GUI_ScreenSetBackground(screen, background);
	GUI_ObjectDecRef((GUI_Object *) background);	
	
	/* create the main contents for the screen */

	mainpage = CreateContents(0,0,w,h);
	GUI_ScreenSetContents(screen, mainpage);
	GUI_ObjectDecRef((GUI_Object *) mainpage);

	GUI_ObjectDecRef((GUI_Object *) font);
	font = 0;
	
	return 0;
}

int main(int argc, char **argv)
{
	int v = SDL_HWSURFACE;
	int w = 640;
	int h = 480;
	int d = 16;
	int loop;
	GUI_Screen *screen;
	
	for (loop=1; loop<argc; loop++)
	{
		if(strcmp(argv[loop], "-fullscreen") == 0)
			v |= SDL_FULLSCREEN;
		if(strcmp(argv[loop], "-window") == 0)
			v &= ~SDL_FULLSCREEN;
		else if(strcmp(argv[loop], "-width") == 0)
			w = atoi(argv[++loop]);
		else if(strcmp(argv[loop], "-height") == 0)
			h = atoi(argv[++loop]);
		else if(strcmp(argv[loop], "-bpp") == 0)
			d = atoi(argv[++loop]);
	}

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;
	atexit(SDL_Quit);

	/* Initialize the TTF library */
	if (TTF_Init() < 0)
		return 1;
	atexit(TTF_Quit);

	/* Initialize the GUI library */
	if (GUI_Init() < 0)
		return 1;
	atexit(GUI_Quit);

	/* create and install the screen */
	screen = GUI_ScreenCreate(w, h, d, v);
	if (screen == NULL)
		return 1;
	GUI_SetScreen(screen);

	/* Initialize the app */
	if (init(w, h) == 0)	
		GUI_Run();
	
	progress_bar = NULL;
	GUI_ObjectDecRef((GUI_Object *) screen);		
	
	return 0;
}
