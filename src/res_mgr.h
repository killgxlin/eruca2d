#ifndef RESMGR
#define RESMGR

enum EResType
{
	ERT_Begin		= 0,

	ERT_Alien		= 0,
	ERT_Explosion	= 1,
	ERT_Player		= 2,
	ERT_Shot		= 3,

	ERT_End			= 4,

	ERT_Num			= 4,
};

struct RESARRAY
{
	EResType	eType;
	const char*	path;
} ResArray[] = {
	{ ERT_Alien,		"data\\images\\alien.gif" },
	{ ERT_Explosion,	"data\\images\\explosion.gif" },
	{ ERT_Player,		"data\\images\\player.gif" },
	{ ERT_Shot,			"data\\images\\shot.gif" },
};

class ResMgr
{
public:
	bool	Init()	
	{
		for(int i=ERT_Begin; i<ERT_End; ++i)
			_images[i] = 0;

		for(int i=0; i<sizeof(ResArray)/sizeof(RESARRAY); ++i)
			if( !LoadData(ResArray[i].eType, ResArray[i].path) ) return false;

		return true;
	}
	void	Destroy() 
	{
		for(int i=ERT_Begin; i<ERT_End; ++i)
			ReleaseData(i);
	}

	bool	LoadData(int type_, const char* path_)
	{
		if( type_ < ERT_Begin || type_ >= ERT_End ) return false;
		
		if( _images[type_] != 0 ) 
			ReleaseData(type_);

		_images[type_] = IMG_Load(path_);

		if( 0 == _images[type_] ) return false;

		SDL_SetColorKey(_images[type_], SDL_SRCCOLORKEY, *(Uint8 *)(_images[type_]->pixels));

		return true;
	}

	void	ReleaseData(int type_)
	{
		if( type_ < ERT_Begin || type_ >= ERT_End ) return;

		if( 0 == _images[type_] ) return;

		SDL_FreeSurface(_images[type_]);
		_images[type_] = 0;
	}
	
	SDL_Surface*	GetSurface(int type_)
	{
		if( type_ < ERT_Begin || type_ >= ERT_End ) return 0;

		return _images[type_];
	}

private:
	SDL_Surface*	_images[ERT_Num];
};

#endif