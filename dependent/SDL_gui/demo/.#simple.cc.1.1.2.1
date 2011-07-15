#include <stdlib.h>
#include "SDL_gui.h"
#include "SDL_thread.h"

int init(int w, int h)
{	
	GUI_Screen *screen = GUI_GetScreen();
	
	/* load the background */

	try
	{
		GUI_Surface *bg = new GUI_Surface("background.jpg");
		screen->SetBackground(bg);
		bg->DecRef();
	}
	catch (GUI_Exception e)
	{
		fprintf(stderr, "Error: %s\n", e.GetMessage());
//		return -1;
	}
	
	// load the font 
	
	GUI_Font *font;
	try
	{
		font = GUI_FontLoadTrueType("bboron.ttf", 48);
	}
	catch (GUI_Exception e)
	{
		fprintf(stderr, "Error: %s\n", e.GetMessage());
		return -1;
	}
	
	/* create the main contents for the screen */

	GUI_Widget *label = new GUI_Label("label", 0,0,w,h, font, "Test");
	screen->SetContents(label);
	label->DecRef();

	font->DecRef();
	
	return 0;
}

int main(int argc, char **argv)
{
	int v = SDL_HWSURFACE; // | SDL_FULLSCREEN | SDL_DOUBLEBUF;
	int w = 640;
	int h = 480;
	int d = 16;

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
	GUI_Screen *screen = GUI_ScreenCreate(w, h, d, v);
	if (screen == NULL)
		return 1;
	GUI_SetScreen(screen);
	
	/* Initialize the app */	
	if (init(w, h) == 0)	
		GUI_Run();
	
	return 0;
}
