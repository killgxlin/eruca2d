#ifndef __TEXT_H__
#define __TEXT_H__

class Painter;

class Text
{
public:
	BOOL	Init();
	VOID	Destroy();

	BOOL	SetFont(const char* szPath);

	VOID	AddText(UINT32 uColor, const char* szFormat, ...);
	VOID	DrawTextAll();

	VOID	DrawSwitch() { m_bDrawText = !m_bDrawText; }

	VOID	DrawText(const Vector2F &vPos, UINT32 uColor, const char* szFormat, ...);

private:

	TTF_Font*		m_pFont;
	struct tagUnit
	{
		UINT32	uColor;
		string	strMsg;
	};

	list<tagUnit>	m_lstUnits;
	BOOL			m_bDrawText;
};

extern Text g_text;

#endif