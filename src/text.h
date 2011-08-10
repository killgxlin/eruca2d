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

private:
	VOID	DrawText(const Vector2F &vPos, UINT32 uColor, const char* szFormat, ...);

	TTF_Font*		m_pFont;
	struct tagUnit
	{
		UINT32	uColor;
		string	strMsg;
	};

	list<tagUnit>	m_lstUnits;;
};

extern Text g_text;

#endif