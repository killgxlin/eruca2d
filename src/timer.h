#ifndef TIMER_H
#define TIMER_H

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

class FrameRate
{
public:
	BOOL	Init();
	VOID	Destroy(){ m_timer.Destroy(); }

	VOID	CountFrame(UINT32 dt);
	VOID	BeginFrame();
	VOID	EndFrame();
	VOID	WaitFrame();

	VOID	SetCustomFPS(FLOAT fCustom);
	FLOAT	GetCustomFPS() const		{ return m_fCustomFPS; }
	FLOAT	GetFPS() const				{ return m_fCurFPS; }

	VOID	SetSpeedRate(FLOAT fRate)	{ m_fSpeedRate = Cut(fRate, 0.5f, 10.0f); }
	FLOAT	GetSpeedRate() const		{ return m_fSpeedRate; }
private:
	UINT32	m_uDt;
	INT		m_nFrames;
	FLOAT	m_fCurFPS;

	FLOAT	m_fCustomFPS;
	FLOAT	m_fSpeedRate;

	Timer	m_timer;
};

extern FrameRate g_framerate;

#endif