#include "common.h"


#include "sprite.h"
#include "painter.h"
#include "gameobj.h"
#include "level.h"
#include "keyboard.h"
#include "timer.h"
#include "text.h"

INT APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   INT       nCmdShow)
{
	g_painter.Init(XScreenW, XScreenH, "eruca2d");

	Timer timer2;
	timer2.Init();

	g_framerate.Init();

	g_text.Init();

	g_level.Init();
	
	while( !g_keyboard.m_bKey[SDLK_ESCAPE] )
	{

		timer2.Update();

		g_framerate.CountFrame(timer2.GetIntervalU());

		g_framerate.BeginFrame();

		g_keyboard.Update();

		g_level.Update(timer2.GetIntervalF());

		g_painter.Clear();

		g_level.Draw(&g_painter);

		g_text.AddText(g_painter.GetColor(255, 255, 255), "FPS    : %4.2f", g_framerate.GetFPS());
		g_text.AddText(g_painter.GetColor(255, 255, 255), "custFPS: %4.2f", g_framerate.GetCustomFPS());
		g_text.AddText(g_painter.GetColor(255, 255, 255), "factor : %4.2f", g_framerate.GetSpeedRate());
		g_text.AddText(g_painter.GetColor(255, 255, 255), "draw ps: %4.2f", g_painter.GetDrawPerSec());
		
		g_text.DrawTextAll();

		g_painter.Flush();

		g_framerate.EndFrame();

		g_framerate.WaitFrame();

		g_painter.Update(timer2.GetIntervalU());
	}

	g_level.Destroy();

	g_text.Destroy();

	g_framerate.Destroy();

	timer2.Destroy();
	g_painter.Destroy();

	return 0;
}