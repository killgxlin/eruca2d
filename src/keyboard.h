#ifndef KEYBOARD_H
#define KEYBOARD_H

class KeyBoard
{
public:
	KeyBoard(){ ZeroMemory(m_bKey, sizeof(m_bKey)); }
	VOID	Update();
	
public:
	bool	m_bKey[SDLK_LALT];
};

extern KeyBoard g_keyboard;

#endif