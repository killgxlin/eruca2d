#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

class KeyBoard
{
public:
	KeyBoard(){ ZeroMemory(m_bKey, sizeof(m_bKey)); }
	VOID	Update();
	
public:
	BOOL	m_bKey[SDLK_LALT];
};

extern KeyBoard g_keyboard;

#endif