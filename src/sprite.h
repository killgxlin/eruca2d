#ifndef __SPRITE_H__
#define __SPRITE_H__

class Painter;
class GameObj;

class Sprite
{
public:
	virtual VOID	Animate(FLOAT dt){}
	virtual VOID	Draw(Painter* pPainter, const Vector2F &vPos);
	virtual AABBox	GetAABBox(const Vector2F &vPos);

public:
	Sprite(GameObj* pGameObj, INT nW, INT nH, UINT8 u8R, UINT8 u8G, UINT8 u8B):m_pGameObj(pGameObj), m_Size(nW, nH),m_u8R(u8R), m_u8G(u8G), m_u8B(u8B), m_fSizeFactor(1.0f){}
	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B) { m_u8R = u8R; m_u8G = u8G; m_u8B = u8B; }

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

class SpritePlayer : public Sprite
{
public:
	VOID			Animate(FLOAT dt);
public:
	SpritePlayer(GameObj* pGameObj):Sprite(pGameObj, XPlayerSize, XPlayerSize, 120, 0, 90), m_nCounter(0){}

private:
	INT				m_nCounter;
};

class SpriteTile : public Sprite
{
public:
	SpriteTile(GameObj* pGameObj):Sprite(pGameObj, XTileSize, XTileSize, 120, 120, 120){}
};

#endif