#ifndef __TIMER_H__
#define __TIMER_H__

class Timer 
{
public:
	BOOL	Init()
	{
		_last_tick = SDL_GetTicks();
		_elapse = 0;

		return TRUE;
	}

	void	Update()
	{
		UINT32 cur_tick = SDL_GetTicks();

		_interval = cur_tick - _last_tick;
		_elapse += _interval;

		_last_tick = cur_tick;
	}

	void	Destroy()
	{
	}

	UINT32	GetIntervalU() const { return _interval; }
	FLOAT	GetIntervalF() const { return _interval / 1000.0f; }

	UINT32	GetElapseU() const { return _elapse; }
	FLOAT	GetElapseF() const { return _elapse / 1000.0f; }

private:
	UINT32	_last_tick;
	UINT32	_interval;
	UINT32	_elapse;

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

	VOID	ModCustomFPS(FLOAT fMod)	{ m_fCustomFPS = Cut(m_fCustomFPS + fMod, 10.0f, 1000.0f); }
	FLOAT	GetCustomFPS() const		{ return m_fCustomFPS; }
	FLOAT	GetFPS() const				{ return m_fCurFPS; }

	VOID	ModSpeedRate(FLOAT fMod)	{ m_fSpeedRate = Cut(m_fSpeedRate + fMod, 0.5f, 10.0f); }
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