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
	Painter painter;
	painter.Init(640, 480, "eruca2d");

	Timer timer;
	timer.Init();
	Timer timer2;
	timer2.Init();

	g_framerate.Init();

	g_text.Init();

	Level lvl;
	lvl.Load();
	
	while( !g_keyboard.m_bKey[SDLK_ESCAPE] )
	{
		timer.Update();
		timer2.Update();

		g_framerate.Update(timer2.GetIntervalU());

		g_keyboard.Update();

		lvl.Update(timer2.GetIntervalF());

		painter.Clear();

		lvl.Draw(&painter);

		char buffer[100];
		sprintf(buffer, "frame rate: %f", g_framerate.GetFrameRate());
		g_text.DrawText(&painter, Vector2(0, 40), buffer, SDL_MapRGB(painter.GetScreen()->format, 100, 100, 100));

		painter.Flush();

		timer.Update();

		INT nSleep = 10-timer.GetIntervalU();
		if(  nSleep < 0 ) nSleep = 0;
		SDL_Delay(nSleep);


	}

	lvl.UnLoad();

	g_text.Destroy();

	g_framerate.Destroy();

	timer2.Destroy();
	timer.Destroy();

	painter.Destroy();

	return 0;
}