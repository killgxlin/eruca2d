#include "common.h"


#include "sprite.h"
#include "painter.h"
#include "gameobj.h"
#include "level.h"
#include "keyboard.h"
#include "timer.h"
#include "text.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	g_painter.Init(640, 480, "eruca2d");

	Timer timer2;
	timer2.Init();

	g_framerate.Init();

	g_text.Init();

	Level lvl;
	lvl.Load();
	
	while( !g_keyboard.m_bKey[SDLK_ESCAPE] )
	{

		timer2.Update();

		g_framerate.CountFrame(timer2.GetIntervalU());

		g_framerate.BeginFrame();

		g_keyboard.Update();

		lvl.Update(timer2.GetIntervalF());

		g_painter.Clear();

		lvl.Draw(&g_painter);

		g_text.DrawText(Vector2(0, 20), 255, 255, 255, "FPS   : %4.2f", g_framerate.GetFPS());
		g_text.DrawText(Vector2(0, 40), 255, 255, 255, "factor: %4.2f", g_framerate.GetSpeedRate());

		g_painter.Flush();

		g_framerate.EndFrame();

		g_framerate.WaitFrame();

	}

	lvl.UnLoad();

	g_text.Destroy();

	g_framerate.Destroy();

	timer2.Destroy();
	g_painter.Destroy();

	return 0;
}