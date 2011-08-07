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
	BOOL	Init()
	{
		m_uDt = 0.0f;
		m_nFrames = 0;
		m_nFrames = 0;
		return TRUE;
	}
	VOID	Destroy(){}

	VOID	Update(UINT32 dt)
	{
		// 记帧数
		++m_nFrames;

		// 计时间
		m_uDt += dt;
		// 若时间大于1秒
		if( m_uDt >= 1000 )
		{
			// 计算fps
			m_fFrameRate = m_nFrames * 1000.0f / m_uDt;

			// 清空时间和帧数
			m_uDt = 0.0f;
			m_nFrames = 0;
		}

	}
	float		GetFrameRate() const	{ return m_fFrameRate; }
private:
	UINT32	m_uDt;
	INT		m_nFrames;
	float	m_fFrameRate;
};

extern FrameRate g_framerate;

#endif