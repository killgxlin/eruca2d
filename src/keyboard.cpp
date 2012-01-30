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
			case SDL_MOUSEBUTTONDOWN:
				m_byMouse |= SDL_BUTTON(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				m_byMouse &= !(SDL_BUTTON(event.button.button));
				break;
		}
	}
	INT nX, nY;
	SDL_GetMouseState(&nX, &nY);
	m_vMousePos.x = nX;
	m_vMousePos.y = nY;
	g_painter.WinToWorld(&m_vMousePos);
	g_text.AddText(XColorR, "pos of mouse  :%4.2f ,%4.2f", m_vMousePos.x, m_vMousePos.y);
	g_text.AddText(XColorR, "state of mouse:%d, %d ,%d", GetButton(SDL_BUTTON_LEFT), GetButton(SDL_BUTTON_MIDDLE), GetButton(SDL_BUTTON_RIGHT));
}

KeyBoard g_keyboard;