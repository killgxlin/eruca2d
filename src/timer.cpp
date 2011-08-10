#include "common.h"
#include "timer.h"

#include "keyboard.h"

FrameRate g_framerate;

BOOL FrameRate::Init()
{
	m_uDt = 0;
	m_nFrames = 0;
	m_nFrames = 0;
	m_fSpeedRate = 1.0f;
	SetCustomFPS(100);
	return m_timer.Init();
}

VOID FrameRate::CountFrame( UINT32 dt )
{
	// ��֡��
	++m_nFrames;

	// ��ʱ��
	m_uDt += dt;

	// ��ʱ�����1��
	if( m_uDt >= 1000 )
	{
		// ����fps
		m_fCurFPS = m_nFrames * 1000.0f / m_uDt;

		// ���ʱ���֡��
		m_uDt = 0;
		m_nFrames = 0;
	}

	if( g_keyboard.m_bKey[SDLK_PAGEUP] )
	{
		SetCustomFPS(GetCustomFPS() + 10);
	}
	if( g_keyboard.m_bKey[SDLK_PAGEDOWN] )
	{
		SetCustomFPS(GetCustomFPS() - 10);
	}
	if( g_keyboard.m_bKey[SDLK_HOME] )
	{
		SetSpeedRate(GetSpeedRate() + 1.0f);
	}
	if( g_keyboard.m_bKey[SDLK_END] )
	{
		SetSpeedRate(GetSpeedRate() - 1.0f);
	}
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

VOID FrameRate::SetCustomFPS( FLOAT fCustom )
{
	m_fCustomFPS = Cut(fCustom, 10.0f, 200.0f);
}
