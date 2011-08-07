#ifndef TEXT_H
#define TEXT_H

class Painter;

class Text
{
public:
	BOOL	Init();
	VOID	Destroy();

	BOOL	SetFont(const char* szPath);

	VOID DrawText(Painter* pScreen, const Vector2 &vPos, const char* szText, UINT32 uColor);

private:
	TTF_Font*	m_pFont;
};

extern Text g_text;

#endif