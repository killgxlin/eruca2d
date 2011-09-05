#include "common.h"
#include "keyboard.h"

VOID KeyBoard::Update()
{
	SDL_Event event;
	while( SDL_PollEvent( &event ) ){
		switch( event.type ){
			case SDL_KEYDOWN:
				m_bKey[event.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				m_bKey[event.key.keysym.sym] = false;
				break;
		}
	}
}

KeyBoard g_keyboard;