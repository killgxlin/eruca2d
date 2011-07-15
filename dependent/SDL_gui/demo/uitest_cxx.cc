#include <stdlib.h>
#include "SDL_gui.h"
#include "SDL_thread.h"

static GUI_Font *font;

typedef struct
{
	char *name;
	GUI_Surface *surface;
} ImageEntry;

ImageEntry images[] =
{
	{ "md-button-normal.png",    NULL },
#define BUTTON_NORMAL images[0].surface
	{ "md-button-highlight.png",    NULL },
#define BUTTON_HIGHLIGHT images[1].surface
	{ "md-button-pressed.png",    NULL },
#define BUTTON_PRESSED images[2].surface
	{ "md-button-disabled.png",    NULL },
#define BUTTON_DISABLED images[3].surface
	{ "floppy-off-normal.png",    NULL },
#define FLOPPY_ON_NORMAL images[4].surface
	{ "floppy-off-highlight.png",    NULL },
#define FLOPPY_ON_HIGHLIGHT images[5].surface
	{ "floppy-on-normal.png",    NULL },
#define FLOPPY_OFF_NORMAL images[6].surface
	{ "floppy-on-highlight.png",    NULL },
#define FLOPPY_OFF_HIGHLIGHT images[7].surface
	{ "sm-button-normal.png",    NULL },
#define SM_BUTTON_NORMAL images[8].surface
	{ "sm-button-highlight.png",    NULL },
#define SM_BUTTON_HIGHLIGHT images[9].surface
	{ "sm-button-pressed.png",    NULL },
#define SM_BUTTON_PRESSED images[10].surface
	{ "sm-button-disabled.png",    NULL },
#define SM_BUTTON_DISABLED images[11].surface
	{ "tiny-button.png",    NULL },
#define TINY_BUTTON images[12].surface
	{ NULL, NULL }
};

int LoadImages()
{
	ImageEntry *entry = images;
	while (entry->name != NULL)
	{
		entry->surface = new GUI_Surface(entry->name);
		if (entry->surface == NULL)
			return -1;
		entry++;
	}
	return 0;
}

/***************************************************************************/

class SmallButton : public GUI_Button
{
	public:
		SmallButton(const char *aname, int x, int y, GUI_Font *font, char *text);
};

SmallButton::SmallButton(const char *aname, int x, int y, GUI_Font *font, char *text)
: GUI_Button(aname, x, y, 100, 50)
{
	if (font != NULL && text != NULL)
	{
		GUI_Widget *label = new GUI_Label("label", 0, 0, 100, 50, font, text);
		SetCaption(label);
		label->DecRef();
	}
	SetNormalImage(SM_BUTTON_NORMAL);
	SetHighlightImage(SM_BUTTON_HIGHLIGHT);
	SetPressedImage(SM_BUTTON_PRESSED);
	SetDisabledImage(SM_BUTTON_DISABLED);
}

/***************************************************************************/

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

/***************************************************************************/

class Page : public GUI_Panel
{
	public:
		Page(const char *aname, int x, int y, int w, int h, const char *title);
};

Page::Page(const char *aname, int x, int y, int w, int h, const char *title)
: GUI_Panel(aname, x, y, w, h)
{
	GUI_Widget *label = new GUI_Label("title", 0, 0, w, 50, font, title);
	AddWidget(label);	
	label->DecRef();
}

/***************************************************************************/

class Page1 : public Page
{
	public:
		Page1(int x, int y, int w, int h);
};

Page1::Page1(int x, int y, int w, int h)
: Page("page1", x, y, w, h, "Page 1: A Bunch of Buttons")
{
	int i, j;
	for (i=0; i<6; i++)
	{
		for (j=0; j<3; j++)
		{
			/* create a button and add it to the page */
			
			GUI_Button *button = new GUI_Button("button", 20+i*100, 50+j*100, 100, 100);
			
			if (i==j)
				button->SetEnabled(0);
			button->SetNormalImage(BUTTON_NORMAL);
			button->SetHighlightImage(BUTTON_HIGHLIGHT);
			button->SetPressedImage(BUTTON_PRESSED);
			button->SetDisabledImage(BUTTON_DISABLED);
			
			AddWidget(button);
			button->DecRef();
		}
	}
}

/***************************************************************************/

class Page2 : public Page
{
	public:
		Page2(int x, int y, int w, int h);
};

Page2::Page2(int x, int y, int w, int h)
: Page("page2", x, y, w, h, "Page 2: A Picture and Some Labels")
{
	GUI_Surface *image = GUI_SurfaceLoad("picture1.jpg");	
	GUI_Widget *picture = GUI_PictureCreate("picture", 0, 50, w, 300, image);
	AddWidget(picture);
	picture->DecRef();
	image->DecRef();
	
	GUI_Widget *label = new GUI_Label("label", 0, 350, w, 40, font, "Lake Superior Shoreline");
	AddWidget(label);
	label->DecRef();
	
	label = new GUI_Label("label", 0, 390, w, 40, font, "Copper Harbor, MI");
	AddWidget(label);
	label->DecRef();
}

/***************************************************************************/

class Page3 : public Page
{
	public:
		Page3(int x, int y, int w, int h);
};

Page3::Page3(int x, int y, int w, int h)
: Page("page3", x, y, w, h, "Page 3: Some Toggle Buttons")
{
	GUI_ToggleButton *button = new GUI_ToggleButton("button", 100, 100, 80, 80);
	button->SetOnNormalImage(FLOPPY_ON_NORMAL);
	button->SetOnHighlightImage(FLOPPY_ON_HIGHLIGHT);
	button->SetOffNormalImage(FLOPPY_OFF_NORMAL);
	button->SetOffHighlightImage(FLOPPY_OFF_HIGHLIGHT);
	button->SetState(0);

	AddWidget(button);
	button->DecRef();

	button = new GUI_ToggleButton("button", 100, 200, 80, 80);
	button->SetOnNormalImage(FLOPPY_ON_NORMAL);
	button->SetOnHighlightImage(FLOPPY_ON_HIGHLIGHT);
	button->SetOffNormalImage(FLOPPY_OFF_NORMAL);
	button->SetOffHighlightImage(FLOPPY_OFF_HIGHLIGHT);
	button->SetState(1);

	AddWidget(button);
	button->DecRef();
}

/***************************************************************************/

class Page4 : public Page
{
	public:
		Page4(int x, int y, int w, int h);
};

Page4::Page4(int x, int y, int w, int h)
: Page("page4", x, y, w, h, "Page 4: A Text Entry and a Progress Bar")
{
	GUI_TextEntry *entry = new GUI_TextEntry("entry1", 100, 100, 200, 50, font, 20);
	entry->SetText("Text1");
	AddWidget(entry);
	entry->DecRef();
	
	progress_bar = new GUI_ProgressBar("pbar", 100, 300, 200, 30);
	AddWidget(progress_bar);

	progress_thread = SDL_CreateThread(progress_function, NULL);
}

GUI_Panel *scrollpanel;
GUI_ScrollBar *scrollbar;

void scrollbar_moved(void *ptr)
{
	int p = scrollbar->GetPosition();
	scrollpanel->SetYOffset(p);
}

/***************************************************************************/

class Page5 : public Page
{
	public:
		Page5(int x, int y, int w, int h);
};

Page5::Page5(int x, int y, int w, int h)
: Page("page5", x, y, w, h, "Page 5: Scroll Bar and Attached Panel")
{
	scrollpanel = new GUI_Panel("panel", 200, 100, 200, 200);
	AddWidget(scrollpanel);
	
	GUI_Widget *widget = new GUI_Label("label", 50, 75, 100, 50, font, "label");
	scrollpanel->AddWidget(widget);
	widget->DecRef();
		
	widget = new SmallButton("test", 50, 125, font, "Test");
	scrollpanel->AddWidget(widget);
	widget->DecRef();
	
	scrollbar = new GUI_ScrollBar("scrollbar", 400, 100, 30, 200);
	scrollbar->SetKnobImage(TINY_BUTTON);
	
	AddWidget(scrollbar);

/*
	GUI_Callback *cb = new GUI_Callback(scrollbar_moved, NULL, NULL);
	scrollbar->SetMovedCallback(cb);
	cb->DecRef();
*/

	GUI_Surface *bg = new GUI_Surface("bg2.jpg");
	GUI_PanelSetBackground(scrollpanel, bg);
	GUI_ObjectDecRef((GUI_Object *) bg);	
}

/***************************************************************************/

class Page6 : public Page
{
	public:
		Page6(int x, int y, int w, int h);
};

Page6::Page6(int x, int y, int w, int h)
: Page("page6", x, y, w, h, "Page 6: A Panel with VBox Layout")
{
	GUI_Panel *box = new GUI_Panel("box", 0, 50, w, h-50);
	AddWidget(box);

	GUI_Layout *layout = new GUI_VBoxLayout("layout");
	box->SetLayout(layout);
	layout->DecRef();

	int i;
	for (i=0; i<6; i++)
	{
		char text[32];
		sprintf(text, "Button %d", i+1);
		
		GUI_Widget *button = new SmallButton(text, 0, 0, font, text);
		box->AddWidget(button);
		button->DecRef();
	}
	box->DecRef();
}

/***************************************************************************/

class Demo : public GUI_Panel
{
	protected:
		GUI_CardStack *cards;
	public:
		Demo(int x, int y, int w, int h);
		virtual ~Demo(void);
		void Next(GUI_Object *sender);
		void Prev(GUI_Object *sender);
};

Demo::Demo(int x, int y, int w, int h)
: GUI_Panel("demo", x, y, w, h)
{
	/* create the card panel */
	
	cards = new GUI_CardStack("cards", 0, 0, w, h-50);
	AddWidget(cards);

	GUI_Widget *page = new Page1(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();
	
	page = new Page2(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();

	page = new Page3(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();

	page = new Page4(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();
		
	page = new Page5(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();

	page = new Page6(0, 0, w, h-50);
	cards->AddWidget(page);
	page->DecRef();

	GUI_EventHandler<Demo> *cb;
	
	/* create the "next" button */
	GUI_Button *button = new SmallButton("next", w-100, h-50, font, "Next Page");
	AddWidget(button);
	cb = new GUI_EventHandler<Demo>(this, &Demo::Next);
	button->SetClick(cb);
	cb->DecRef();
	button->DecRef();
	
	/* create the "prev" button */

	button = new SmallButton("prev", 0, h-50, font, "Prev Page");
	AddWidget(button);
	cb = new GUI_EventHandler<Demo>(this, &Demo::Prev);
	button->SetClick(cb);
	cb->DecRef();
	button->DecRef();
}
	
Demo::~Demo(void)
{
	cards->DecRef();
}

void Demo::Next(GUI_Object *sender)
{
	cards->Next();
}

void Demo::Prev(GUI_Object *sender)
{
	cards->Prev();
}

/***************************************************************************/

int init(int w, int h)
{	
	GUI_Screen *screen = GUI_GetScreen();
	
	font = GUI_FontLoadTrueType("bboron.ttf", 18);
	if (font == NULL)
		return -1;

	if (LoadImages() < 0)
		return -1;

	/* load the background */

	GUI_Surface *bg = GUI_SurfaceLoad("background.jpg");
	screen->SetBackground(bg);
	bg->DecRef();
	
	/* create the main contents for the screen */

	GUI_Widget *mainpage = new Demo(0,0,w,h);
	screen->SetContents(mainpage);
	mainpage->DecRef();

	return 0;
}

int main(int argc, char **argv)
{
	int v = SDL_HWSURFACE;
	int w = 640;
	int h = 480;
	int d = 16;
	int loop;
	
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

	/* Initialze the GUI library */
	if (GUI_Init() < 0)
		return 1;
	atexit(GUI_Quit);

	/* create and install the screen */
	GUI_Screen *screen = GUI_ScreenCreate(w, h, d, v);
	if (screen == NULL)
		return 1;
	GUI_SetScreen(screen);
//	screen->SetFlags(SCREEN_DEBUG_BLIT);
	
	/* Initialize the app */
	if (init(w, h) == 0)	
		GUI_Run();
	
	progress_bar = NULL;
	
	return 0;
}
