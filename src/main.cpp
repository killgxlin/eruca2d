#include "common.h"


#include "sprite.h"
#include "gameobj.h"
#include "level.h"
#include "timer.h"

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
	
	while( !g_keyboard.FetchKey(SDLK_ESCAPE) )
	{

		timer2.Update();

		g_framerate.CountFrame(timer2.GetIntervalU());

		g_framerate.BeginFrame();

		g_keyboard.Update();

		g_level.Update(timer2.GetIntervalF());

		g_painter.Clear();

		g_level.Draw(&g_painter);

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