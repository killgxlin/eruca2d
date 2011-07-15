#ifndef INPUT
#define INPUT

class Input
{
public:
	bool	Init()
	{
		for(int i=0; i<sizeof(_keys)/sizeof(_keys[0]); ++i)
			_keys[i] = 0;
		_key_num = 0;

		return true;
	}

	void	Destroy()	{}

	void	Update()
	{
		SDL_PumpEvents();

		Uint8* keys = SDL_GetKeyState(&_key_num);

		for(int i=0; i<_key_num; ++i)
			_keys[i] = keys[i];
	}

	bool	IsKeyDown(int key)
	{
		if( key >= _key_num || key < 0 ) return false;

		return _keys[key] != 0;
	}

private:
	Uint8	_keys[512];
	int		_key_num;
};

#endif