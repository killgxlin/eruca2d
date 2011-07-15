#ifndef WINDOW
#define WINDOW

class Window
{
public:
	Window():_screen(0){}
	bool	Init(int w, int h, const char* title)
	{
		Destroy();

		_screen = SDL_SetVideoMode(w, h, 8, SDL_SWSURFACE|SDL_DOUBLEBUF);
		if( 0 == _screen ) return false;

		if( title != 0 )
			SDL_WM_SetCaption(title, 0);
		return true;
	}

	void	Destroy()
	{
		if( 0 != _screen )
			SDL_FreeSurface( _screen );
		_screen = 0;
	}

	void	Flush()
	{
		if( 0 == _screen ) return;

		SDL_Flip(_screen);
	}

	void	Clear()
	{
		SDL_FillRect(_screen, 0, SDL_MapRGB(_screen->format, 0, 0, 0));
	}

	SDL_Surface*	GetSurface()	{ return _screen; }
	int				GetH() const	{ return _screen->h; }
	int				GetW() const	{ return _screen->w; }


private:
	SDL_Surface*	_screen;
};


#endif