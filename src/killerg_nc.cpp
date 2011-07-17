#include "common.h"

#include "engine.h"
#include "res_mgr.h"
#include "window.h"
#include "sprite.h"
#include "input.h"
#include "frame_rate.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	// 初始化各个底层系统
	// sdl
	// window
	// resmgr
	// input

	Engine::Init();

	Input input;
	input.Init();

	Window window;
	window.Init(800, 600, "killerg2d");

	sResMgr.Init();
	
	FrameRate framerate;
	framerate.Init(60);

	Timer timer;
	timer.Init();

	SpriteMultChannel sprite_mult;
	sprite_mult.Init("data/sprites/mult_channel_sprite.txt");

	int x = 100, y = 100;
	int vx = 0, vy = 0;
	int model = 0;

	while( !input.IsKeyDown(SDLK_ESCAPE) )
	{
		timer.Update();

		input.Update();

		vx = vy = 0;

		if( input.IsKeyDown(SDLK_LEFT) )
			vx = -1;
		
		if( input.IsKeyDown(SDLK_RIGHT) )
			vx = 1;

		if( input.IsKeyDown(SDLK_UP) )
			vy = -1;

		if( input.IsKeyDown(SDLK_DOWN) )
			vy = 1;

		x += vx;
		y += vy;

		if( input.IsKeyDown(SDLK_x) )
		{
			static int channel = 0;
			sprite_mult.PlayChannel(channel++);
			if( channel >= sprite_mult.GetChannelNum() )
				channel = 0;
		}

		if( input.IsKeyDown(SDLK_y) )
			sprite_mult.Stop();

		sprite_mult.Update(timer.GetIntervalF());

		window.Clear();

		sprite_mult.Draw(&window, x, y);

		window.Flush();

		framerate.WaitFrame();
	}

	sprite_mult.Destroy();

	timer.Destroy();

	framerate.Destroy();

	sResMgr.Destroy();

	window.Destroy();

	input.Destroy();


	Engine::Destroy();


	
}

