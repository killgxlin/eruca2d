#include "common.h"
#include "timer.h"

FrameRate g_framerate;

BOOL FrameRate::Init()
{
	m_uDt = 0;
	m_nFrames = 0;
	m_nFrames = 0;
	m_fSpeedRate = 1.0f;
	ModCustomFPS(1000);
	return m_timer.Init();
}

VOID FrameRate::CountFrame( UINT32 dt )
{
	// 记帧数
	++m_nFrames;

	// 计时间
	m_uDt += dt;

	// 若时间大于1秒
	if( m_uDt >= 1000 )
	{
		// 计算fps
		m_fCurFPS = m_nFrames * 1000.0f / m_uDt;

		// 清空时间和帧数
		m_uDt = 0;
		m_nFrames = 0;
	}

	if( g_keyboard.FetchKey(SDLK_PAGEUP) )
	{
		ModCustomFPS(100);
	}
	if( g_keyboard.FetchKey(SDLK_PAGEDOWN) )
	{
		ModCustomFPS(- 100);
	}
	if( g_keyboard.FetchKey(SDLK_HOME) )
	{
		ModSpeedRate(0.50f);
	}
	if( g_keyboard.FetchKey(SDLK_END) )
	{
		ModSpeedRate(-0.50f);
	}

	g_text.AddText(g_painter.GetColor(255, 0, 0), "FPS      : %4.2f", g_framerate.GetFPS());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "custFPS  : %4.2f", g_framerate.GetCustomFPS());
	g_text.AddText(g_painter.GetColor(255, 0, 0), "factor   : %4.2f", g_framerate.GetSpeedRate());
}

VOID FrameRate::BeginFrame()
{
	m_timer.Update();
}

VOID FrameRate::EndFrame()
{
	m_timer.Update();
}

VOID FrameRate::WaitFrame()
{
	INT nSleep = INT(ceil(1000 / m_fCustomFPS)) - m_timer.GetIntervalU();
	if( nSleep > 0 ) SDL_Delay(nSleep);
}

