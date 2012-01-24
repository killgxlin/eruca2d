#ifndef __SPRITE_H__
#define __SPRITE_H__
//-----------------------------------------------------------------------------------------------

class Painter;
class GameObj;
//-----------------------------------------------------------------------------------------------

class Sprite
{
public:
	virtual VOID	Animate(FLOAT dt){}
	virtual VOID	Draw(Painter* pPainter, const Vector2F &vPos);
	virtual Square	GetAABBox(const Vector2F &vPos);

public:
	Sprite(GameObj* pGameObj, INT nW, INT nH, UINT8 u8R, UINT8 u8G, UINT8 u8B):m_pGameObj(pGameObj), m_Size(nW, nH),m_u8R(u8R), m_u8G(u8G), m_u8B(u8B), m_fSizeFactor(1.0f){}
	virtual ~Sprite() { m_pGameObj = NULL; }
	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B) { m_u8R = u8R; m_u8G = u8G; m_u8B = u8B; }

	Vector2F		GetSize() const { return m_Size*m_fSizeFactor; }
	FLOAT			GetSizeFactor() const { return m_fSizeFactor; }
	VOID			SetSizeFactor(FLOAT fSizeFactor)
	{
		m_fSizeFactor = Cut(fSizeFactor, 0.1f, 10.f);
	}

protected:
	FLOAT		m_fSizeFactor;
	Vector2F	m_Size;
	UINT8		m_u8R;
	UINT8		m_u8G;
	UINT8		m_u8B;
	GameObj*	m_pGameObj;
};

//-----------------------------------------------------------------------------------------------
class SpriteAction : public Sprite
{
public:
	virtual VOID	Animate(FLOAT dt);
	virtual VOID	Draw(Painter* pPainter, const Vector2F &vPos);
	virtual Square	GetAABBox(const Vector2F &vPos);

	BOOL			Start(const char* szAlias, INT nCount = -1);

	SpriteAction(GameObj* pGameObj):Sprite(pGameObj, 0, 0, 0, 0, 0), m_pCurAction(NULL){}
protected:
	VOID			RegAction(tagAction* pAction, const char* szAlias);

private:
	typedef map<INT32, tagAction*>	ActionMap;
	ActionMap			m_mapActions;	

	tagAction*			m_pCurAction;
	FLOAT				m_fCurCuration;
	INT32				m_nCurAliasCrc;
	INT					m_nCount;
};

//-----------------------------------------------------------------------------------------------

class SpriteDirAction : public SpriteAction
{
public:
	SpriteDirAction(GameObj* pGameObj):SpriteAction(pGameObj){}

	BOOL	Start(const char* szAction, BOOL bRight, INT nCount = -1)
	{
		char buffer[100] = {0};
		sprintf(buffer, "%s_%s", szAction, bRight ? "r" : "l");

		return SpriteAction::Start(buffer, nCount);
	}
};


//-----------------------------------------------------------------------------------------------
class SpritePlayer : public SpriteDirAction
{
public:
	VOID Draw( Painter* pPainter, const Vector2F &vPos );
	SpritePlayer(GameObj* pGameObj):SpriteDirAction(pGameObj)
	{
		RegAction(g_resmgrex.GetAction("rockman_stand_r"),	"stand_r");
		RegAction(g_resmgrex.GetAction("rockman_run_r"),	"run_r");
		RegAction(g_resmgrex.GetAction("rockman_jump_r"),	"jump_r");
		RegAction(g_resmgrex.GetAction("rockman_fall_r"),	"fall_r");
		RegAction(g_resmgrex.GetAction("rockman_land_r"),	"land_r");
		RegAction(g_resmgrex.GetAction("rockman_stand_l"),	"stand_l");
		RegAction(g_resmgrex.GetAction("rockman_run_l"),	"run_l");
		RegAction(g_resmgrex.GetAction("rockman_jump_l"),	"jump_l");
		RegAction(g_resmgrex.GetAction("rockman_fall_l"),	"fall_l");
		RegAction(g_resmgrex.GetAction("rockman_land_l"),	"land_l");
	}
};

//-----------------------------------------------------------------------------------------------

class SpriteTerrain : public SpriteAction
{
public:
	SpriteTerrain(GameObj* pGameObj):SpriteAction(pGameObj)
	{
		RegAction(g_resmgrex.GetAction("brick_ice"), "brick");
		Start("brick");
	}
};
//-----------------------------------------------------------------------------------------------

class SpriteArrow : public Sprite
{
public:
	SpriteArrow(GameObj* pGameObj):Sprite(pGameObj, XArrowSize, XArrowSize, 255, 255, 255){}
	virtual VOID	Draw(Painter* pPainter, const Vector2F &vPos);
	virtual Square	GetAABBox(const Vector2F &vPos);
private:
	Vector2F		m_vVelUnit;
};
//-----------------------------------------------------------------------------------------------

class SpriteAnimal : public SpriteDirAction
{
public:
	SpriteAnimal(GameObj* pGameObj) : SpriteDirAction(pGameObj)	{}
};

#endif