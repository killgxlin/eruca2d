#ifndef FRAMERATE
#define FRAMERATE

class FrameRate
{
public:
	bool	Init(INT framerate)
	{
		_framerate = framerate;
		return true;
	}

	void	Destroy()
	{
		_framerate = 1;
	}

	void	WaitFrame(void)
	{
		static Uint32 next_tick = 0;
		Uint32 this_tick;

		/* Wait for the next frame */
		this_tick = SDL_GetTicks(); 
		if ( this_tick < next_tick ) {
			SDL_Delay(next_tick-this_tick);
		}
		next_tick = this_tick + (1000/_framerate);
	}

private:
	int		_framerate;
};

class Timer 
{
public:
	bool	Init()
	{
		_last_tick = SDL_GetTicks();
		_elapse = 0;

		return true;
	}

	void	Update()
	{
		Uint32 cur_tick = SDL_GetTicks();
		
		_interval = cur_tick - _last_tick;
		_elapse += _interval;

		_last_tick = cur_tick;
	}

	void	Destroy()
	{
	}

	Uint32	GetIntervalU() const { return _interval; }
	float	GetIntervalF() const { return _interval / 1000.0f; }

	Uint32	GetElapseU() const { return _elapse; }
	float	GetElapseF() const { return _elapse / 1000.0f; }

private:
	Uint32	_last_tick;
	Uint32	_interval;
	Uint32	_elapse;

};

#endif