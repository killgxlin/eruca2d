#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_gui.h"
#include "SDL_thread.h"
#include "SDL_ttf.h"

#ifdef WIN32
# include <windows.h>
#endif
#include <GL/gl.h>

#ifdef SDL_OPENGLBLIT
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

GUI_Surface *LoadImage(const char *filename)
{
	GUI_Surface *image = new GUI_Surface(filename);
	image->DisplayFormat();
//	image->SetColorKey(0);
//	GUI_Surface *image = new GUI_Surface(filename, SDL_SWSURFACE, 
//		temp->GetWidth(), temp->GetHeight(), 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
//		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
//		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
//	);
//	image->Blit(NULL, temp, NULL);
//	temp->DecRef();
	return image;
}

MyButton::MyButton(const char *aname, int x, int y, const char *text)
: GUI_Button(aname, x, y, 100, 50)
{
	if (font == 0) font = new GUI_FastFont("simplefont.png");
	if (image0 == 0) image0 = LoadImage("sm-button-disabled.png");
	if (image1 == 0) image1 = LoadImage("sm-button-normal.png");
	if (image2 == 0) image2 = LoadImage("sm-button-highlight.png");
	if (image3 == 0) image3 = LoadImage("sm-button-pressed.png");
	
	GUI_Widget *widget = new GUI_Label("label", 0, 0, 100, 50, font, text);
	SetCaption(widget);
	widget->DecRef();

	SetDisabledImage(image0);
	SetNormalImage(image1);
	SetHighlightImage(image2);
	SetPressedImage(image3);
}

/***************************************************************************/

class MyScreen : public GUI_RealScreen
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
: GUI_RealScreen(aname, surface)
{
	cursor = LoadImage("cursor.png");
//	cursor->DisplayFormat();
//	cursor->SetColorKey(0);
}

void MyScreen::Erase(const SDL_Rect *dr)
{
}

void MyScreen::Update(int force)
{
	GUI_RealScreen::Update(force);
	
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
	GUI_Font *font = new GUI_FastFont("simplefont.png");
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

void Setup(int w, int h)
{
	int value;

	printf("\n");
	printf( "Vendor     : %s\n", glGetString( GL_VENDOR ) );
	printf( "Renderer   : %s\n", glGetString( GL_RENDERER ) );
	printf( "Version    : %s\n", glGetString( GL_VERSION ) );
	printf( "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
	printf("\n");

	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
	printf( "SDL_GL_RED_SIZE: requetsed 4, got %d\n", value );
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
	printf( "SDL_GL_GREEN_SIZE: requested 4, got %d\n", value);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
	printf( "SDL_GL_BLUE_SIZE: requested 4, got %d\n", value );
	SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &value );
	printf( "SDL_GL_ALPHA_SIZE: requested 4, got %d\n", value );
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
	printf( "SDL_GL_DEPTH_SIZE: requested 16, got %d\n", value );
	SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
	printf( "SDL_GL_DOUBLEBUFFER: requested 1, got %d\n", value );

	/* Set the window manager title bar */
	SDL_WM_SetCaption( "SDL GL test", "testgl" );

	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	glOrtho( -2.0, 2.0, -2.0, 2.0, -20.0, 20.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
}

void Draw(void)
{
	GLenum gl_error;
	char *sdl_error;

	double color = 0.0;
	float cube[8][3]= {{ 0.5,  0.5, -0.5}, 
			   { 0.5, -0.5, -0.5},
			   {-0.5, -0.5, -0.5},
			   {-0.5,  0.5, -0.5},
			   {-0.5,  0.5,  0.5},
			   { 0.5,  0.5,  0.5},
			   { 0.5, -0.5,  0.5},
			   {-0.5, -0.5,  0.5}};

	/* Do our drawing, too. */
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin( GL_QUADS );
		glColor3f(1.0, 0.0, 0.0);
		glVertex3fv(cube[0]);
		glVertex3fv(cube[1]);
		glVertex3fv(cube[2]);
		glVertex3fv(cube[3]);
			
		glColor3f(0.0, 1.0, 0.0);
		glVertex3fv(cube[3]);
		glVertex3fv(cube[4]);
		glVertex3fv(cube[7]);
		glVertex3fv(cube[2]);
			
		glColor3f(0.0, 0.0, 1.0);
		glVertex3fv(cube[0]);
		glVertex3fv(cube[5]);
		glVertex3fv(cube[6]);
		glVertex3fv(cube[1]);
			
		glColor3f(0.0, 1.0, 1.0);
		glVertex3fv(cube[5]);
		glVertex3fv(cube[4]);
		glVertex3fv(cube[7]);
		glVertex3fv(cube[6]);

		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(cube[5]);
		glVertex3fv(cube[0]);
		glVertex3fv(cube[3]);
		glVertex3fv(cube[4]);

		glColor3f(1.0, 0.0, 1.0);
		glVertex3fv(cube[6]);
		glVertex3fv(cube[1]);
		glVertex3fv(cube[2]);
		glVertex3fv(cube[7]);
	glEnd( );
		
	glMatrixMode(GL_MODELVIEW);
	glRotatef(5.0, 1.0, 1.0, 1.0);

	color += 0.01;

	if( color >= 1.0 ) {
		color = 0.0;
	}

	/* Check for error conditions. */
	gl_error = glGetError( );

	if( gl_error != GL_NO_ERROR ) {
		fprintf( stderr, "testgl: OpenGL error: %d\n", gl_error );
	}

	sdl_error = SDL_GetError( );

	if( sdl_error[0] != '\0' ) {
		fprintf(stderr, "testgl: SDL error '%s'\n", sdl_error);
		SDL_ClearError();
	}
}

int main(int argc, char **argv)
{
//	int f = SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF;
	int f = SDL_OPENGLBLIT;
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
		
	// Configure GL
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	realscreen = SDL_SetVideoMode(w, h, d, f);
	if (realscreen == NULL)
		return 1;

	GUI_Screen *screen = new MyScreen("screen", realscreen);
	if (screen == NULL)
		return 1;
	GUI_SetScreen(screen);
		
	// tell SDL_gui what thread has access to the gui
	GUI_SetThread(SDL_ThreadID());

	/* Initialize the app */
	MyPanel *panel = new MyPanel(0, 0, w, h);
	screen->SetContents(panel);

	RunningAverage fps(30);
	
	Setup(w,h);
	
	Uint32 t0 = SDL_GetTicks();
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
			{
				if (event.key.keysym.sym == SDLK_s)	
					SDL_SaveBMP(realscreen, "screenshot.bmp");
				if (event.key.keysym.sym == SDLK_RETURN )
				{
					// ALT-Enter: toggle fullscreen 
					if (event.key.keysym.mod & KMOD_ALT)
					{
						SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
					}
				}
			}
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

		Draw();			

		GUI_Lock();
		screen->DoUpdate(1);
		GUI_Unlock();
		
		SDL_GL_SwapBuffers( );
	}
	
	screen->DecRef();
	SDL_ShowCursor(1);

	return 0;
}
#else
int main()
{
	printf("this demo needs SDL_OPENGLBLIT, which is available in\nversions 1.1.4 of SDL and higher.\n");
	return 0;
}
#endif
