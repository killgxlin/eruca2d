#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

class KeyBoard
{
public:
	KeyBoard(){ memset(m_bKey, 0, sizeof(m_bKey)); m_byMouse = 0; }
	VOID	Update();
	
	BOOL	FetchKey(UINT16 uKey)
	{
		BOOL bPress = m_bKey[uKey];
		m_bKey[uKey] = FALSE;
		return bPress;
	}
	BOOL	GetKey(UINT16 uKey)
	{
		return m_bKey[uKey];
	}
	BOOL	FetchButton(BYTE byButton)
	{
		BOOL bPress = GetButton(byButton);
		m_byMouse &= ~(SDL_BUTTON(byButton));
		return bPress;
	}
	BOOL	GetButton(BYTE byButton)
	{
		return (m_byMouse & SDL_BUTTON(byButton)) != 0;
	}
public:
	BOOL		m_bKey[SDLK_LAST];
	BYTE		m_byMouse;
	Vector2F	m_vMousePos;
};

extern KeyBoard g_keyboard;

#endif