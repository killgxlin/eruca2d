#include "common.h"
#include "keyboard.h"

VOID KeyBoard::Update()
{
	SDL_Event event;
	while( SDL_PollEvent( &event ) ){
		switch( event.type ){
			case SDL_KEYDOWN:
				m_bKey[event.key.keysym.sym] = TRUE;
				break;
			case SDL_KEYUP:
				m_bKey[event.key.keysym.sym] = FALSE;
				break;
		}
	}
	INT nX, nY;
	m_byMouse = SDL_GetMouseState(&nX, &nY);
	m_vMousePos.x = nX;
	m_vMousePos.y = nY;
	g_painter.SDLToWorld(&m_vMousePos);
	g_text.AddText(g_painter.GetColor(255, 0, 0), "pos of mouse  :%4.2f ,%4.2f", m_vMousePos.x, m_vMousePos.y);
}

KeyBoard g_keyboard;