#ifndef ENGINE
#define ENGINE

class Engine
{
public:
	static bool	Init()
	{
		if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
			printf("Could not initialize SDL: %s.\n", SDL_GetError());
			return false;
		}
		return true;
	}
	static void	Destroy()
	{
		SDL_Quit();
	}

};

#endif