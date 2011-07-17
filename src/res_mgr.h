#ifndef RESMGR
#define RESMGR


class ResMgr
{
public:
	bool			Init()	{	return true;}
	void			Destroy();

	SDL_Surface*	CreateSurface(const char* file_);
	SDL_Surface*	CreateSubSurface(SDL_Surface* src_, SDL_Rect* rect_, bool flip_);

	SDL_Surface*	GetSurface(const char* name_, unsigned long* hashcode_ = 0, bool flip_ = false);
	SDL_Surface*	GetSurface(unsigned long hashcode, bool flip_ = false);

};

extern ResMgr	sResMgr;

#endif
