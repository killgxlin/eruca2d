#include <stdlib.h>
#include <math.h>

#include "SDL_gui.h"
#include "SDL_image.h"
#include "SDL_thread.h"

#ifndef M_PI
#define M_PI 3.1415926
#endif

#define SPEED 100.0

static SDL_Surface *sprite;
static SDL_Surface *realscreen;

class MyButton : public GUI_Button
{
	protected:
		static GUI_Font *font;
		static GUI_Surface *image0;
		static GUI_Surface *image1;
		static GUI_Surface *image2;
		static GUI_Surface *image3;
	public:
		MyButton(const char *aname, int x, int y, const char *text);
};

GUI_Font *MyButton::font = 0;
GUI_Surface *MyButton::image0 = 0;
GUI_Surface *MyButton::image1 = 0;
GUI_Surface *MyButton::image2 = 0;
GUI_Surface *MyButton::image3 = 0;

class MySurface : public GUI_Surface
{
	public:
		MySurface(const char *filename);
};

MySurface::MySurface(const char *filename)
: GUI_Surface(filename)
{
	SDL_SetAlpha(surface, SDL_SRCALPHA|SDL_RLEACCEL, 0);
//	image->SetColorKey(0);
//	image->DisplayFormat();
}

MyButton::MyButton(const char *aname, int x, int y, const char *text)
: GUI_Button(aname, x, y, 100, 50)
{
	if (font == 0) font = new GUI_TrueTypeFont("bboron.ttf", 20);
	if (image0 == 0) image0 = new MySurface("sm-button-disabled.png");
	if (image1 == 0) image1 = new MySurface("sm-button-normal.png");
	if (image2 == 0) image2 = new MySurface("sm-button-highlight.png");
	if (image3 == 0) image3 = new MySurface("sm-button-pressed.png");
	
	GUI_Widget *widget = new GUI_Label("label", 0, 0, 100, 50, font, text);
	SetCaption(widget);
	widget->DecRef();

	SetDisabledImage(image0);
	SetNormalImage(image1);
	SetHighlightImage(image2);
	SetPressedImage(image3);
}

/***************************************************************************/

class MyScreen : public GUI_Screen
{
	protected:
		GUI_Surface *cursor;
		int cursor_x;
		int cursor_y;
	public:
		MyScreen(const char *aname, SDL_Surface *surface);
		virtual void Erase(const SDL_Rect *dr);
		virtual void Update(int force);
		int Event(const SDL_Event *event, int xoffset, int yoffset);
};

MyScreen::MyScreen(const char *aname, SDL_Surface *surface)
: GUI_Screen(aname, surface)
{
	cursor = new GUI_Surface("cursor.png");
	cursor->SetColorKey(0);
	cursor->DisplayFormat();
}

void MyScreen::Erase(const SDL_Rect *dr)
{
}

void MyScreen::Update(int force)
{
	GUI_Screen::Update(force);
	
	SDL_Rect dr;
	dr.x = cursor_x;
	dr.y = cursor_y;
	dr.w = cursor->GetWidth();
	dr.h = cursor->GetHeight();
	
	Draw(cursor, NULL, &dr);
	FlushUpdates();
}

int MyScreen::Event(const SDL_Event *event, int xoffset, int yoffset)
{
	if (event->type == SDL_MOUSEMOTION)
	{
		cursor_x = event->motion.x;
		cursor_y = event->motion.y;
	}
	return GUI_Screen::Event(event, xoffset, yoffset);
}

/***************************************************************************/

class MyPanel : public GUI_Panel
{
	protected:
		GUI_Label *label;
	public:
		MyPanel(int x, int y, int w, int h);
		void ShowFramesPerSecond(double fps);
};

MyPanel::MyPanel(int x, int y, int w, int h)
: GUI_Panel("panel", x, y, w, h)
{
	GUI_Font *font = new GUI_TrueTypeFont("bboron.ttf", 12);
	label = new GUI_Label("fps", 0, 0, w, 12, font, "fps");
	AddWidget(label);
	label->SetAlign(WIDGET_HORIZ_LEFT | WIDGET_VERT_TOP);
	font->DecRef();
	
	int i, j;
	for (i=0; i<5; i++)
	{
		for (j=0; j<4; j++)
		{
			GUI_Widget *widget = new MyButton("button", 20+j*150, 15+i*75, "Test");
			AddWidget(widget);
			widget->DecRef();
		}
	}
}

void MyPanel::ShowFramesPerSecond(double fps)
{
	char buffer[80];
	sprintf(buffer, "FPS: %6.1f", fps);
	label->SetText(buffer);
}

/***************************************************************************/

class RunningAverage
{
	protected:
		int count, size, index;
		double sum;
		double *history;
	public:
		RunningAverage(int n);
		virtual ~RunningAverage(void);
		void AddPoint(double value);
		double Calc(void);
};

RunningAverage::RunningAverage(int n)
{
	size = n;
	count = index = 0;
	history = new double[size];
	sum = 0;
}

RunningAverage::~RunningAverage(void)
{
	delete [] history;
}

void RunningAverage::AddPoint(double value)
{
	if (count >= size)
		sum -= history[index];
	history[index] = value;
	if (count < size) count++;
	sum += value;
	if (++index >= size)
		index = 0;
}

double RunningAverage::Calc(void)
{
	return sum / count;
}

/***************************************************************************/

#define NUM_SPRITES 100

struct spriteinfo
{
	double x, y;
	double angle;
} Sprites[NUM_SPRITES];

void MoveSprites(double t, int w, int h)
{
	int i;
	
	for (i=0; i<NUM_SPRITES; i++)
	{
		double x = Sprites[i].x + t * SPEED * cos(Sprites[i].angle * M_PI / 180.0);
		double y = Sprites[i].y - t * SPEED * sin(Sprites[i].angle * M_PI / 180.0);

		if (x < 0 || x > w - sprite->w)
		{
			Sprites[i].angle = 180 - Sprites[i].angle;
			if (Sprites[i].angle < 0) Sprites[i].angle += 360;
			if (Sprites[i].angle > 360) Sprites[i].angle -= 360;
		}
		else
			Sprites[i].x = x;

		if (y < 0 || y > h - sprite->h)
			Sprites[i].angle = 360 - Sprites[i].angle;
		else
			Sprites[i].y = y;
	}
}

void DrawSprites(void)
{
	int i;
	SDL_Rect dr;
	
	for (i=0; i<NUM_SPRITES; i++)
	{
		dr.x = (int) Sprites[i].x;
		dr.y = (int) Sprites[i].y;
		dr.w = sprite->w;
		dr.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, realscreen, &dr);
	}
}

int main(int argc, char **argv)
{
	int f = SDL_HWSURFACE; // | SDL_FULLSCREEN | SDL_DOUBLEBUF;
	int w = 640;
	int h = 480;
	int d = 32;

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

	d = SDL_VideoModeOK(w, h, d, f);
	if (d == 0)
		return 1;
	printf("Depth %d selected\n", d);
		
	realscreen = SDL_SetVideoMode(w, h, d, f);
	if (realscreen == NULL)
		return 1;

	GUI_Screen *screen = new MyScreen("screen", realscreen);
	if (screen == NULL)
		return 1;
	GUI_SetScreen(screen);
		
	// load and prepare the sprite
	SDL_Surface *temp = IMG_Load("smiley.png");
	if (temp == NULL)
		return 1;
	Uint32 c = SDL_MapRGB(temp->format, 255, 255, 255);
	if (SDL_SetColorKey(temp, SDL_RLEACCEL | SDL_SRCCOLORKEY, c) < 0)
		return 1;
	sprite = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	if (sprite == NULL)
		return 1;

	int i;
	for (i=0; i<NUM_SPRITES; i++)
	{
		Sprites[i].x = rand() % (w-sprite->w);
		Sprites[i].y = rand() % (h-sprite->h);
		Sprites[i].angle = rand() % 360;
	}

	// tell SDL_gui what thread has access to the gui
	GUI_SetThread(SDL_ThreadID());

	Uint32 t0 = SDL_GetTicks();

	/* Initialize the app */
	MyPanel *panel = new MyPanel(0, 0, w, h);
	screen->SetContents(panel);

	RunningAverage fps(30);
	
	SDL_Event event;

	SDL_ShowCursor(0);
	GUI_SetRunning(1);	
	
	/* update the screen */
	screen->DoUpdate(1);

	/* process events */
	while (GUI_GetRunning())
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
				if (event.key.keysym.sym == SDLK_s)	
					SDL_SaveBMP(realscreen, "screenshot.bmp");
			GUI_Lock();
			screen->Event(&event, 0, 0);
			GUI_Unlock();
		}

		Uint32 t1;
		do t1 = SDL_GetTicks(); while (t0 == t1);
		double t = (t1 - t0) / 1000.0;
		t0 = t1;

		fps.AddPoint(1/t);
		panel->ShowFramesPerSecond(fps.Calc());

		MoveSprites(t, w, h);

		/* update the screen */
		SDL_FillRect(realscreen, NULL, 0);

		DrawSprites();			

		GUI_Lock();
		screen->DoUpdate(1);
		GUI_Unlock();
		
		SDL_Flip(realscreen);
	}
	
	screen->DecRef();
	SDL_ShowCursor(1);

	return 0;
}
