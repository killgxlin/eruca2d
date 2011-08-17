#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

class KeyBoard
{
public:
	KeyBoard(){ ZeroMemory(m_bKey, sizeof(m_bKey)); }
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
public:
	BOOL	m_bKey[SDLK_LAST];
};

extern KeyBoard g_keyboard;

#endif