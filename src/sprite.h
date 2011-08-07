#ifndef SPRITE_H
#define SPRITE_H

class Painter;
class GameObj;

class Sprite
{
public:
	virtual VOID	Animate(float dt){}
	virtual VOID	Draw(Painter* pPainter, const Vector2 &vPos);
	virtual AABBox	GetAABBox(const Vector2 &vPos);

public:
	Sprite(GameObj* pGameObj, INT nW, INT nH, UINT8 u8R, UINT8 u8G, UINT8 u8B):m_pGameObj(pGameObj), m_Size(nW, nH),m_u8R(u8R), m_u8G(u8G), m_u8B(u8B){}
	
protected:
	Size		m_Size;
	UINT8		m_u8R;
	UINT8		m_u8G;
	UINT8		m_u8B;
	GameObj*	m_pGameObj;
};

class SpritePlayer : public Sprite
{
public:
	VOID			Animate(float dt);
public:
	SpritePlayer(GameObj* pGameObj):Sprite(pGameObj, 20, 20, 120, 0, 90), m_nCounter(0){}

private:
	INT				m_nCounter;
};

class SpriteTile : public Sprite
{
public:
	SpriteTile(GameObj* pGameObj):Sprite(pGameObj, 20, 20, 120, 120, 120){}
};

#endif