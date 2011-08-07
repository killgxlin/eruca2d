#ifndef GAMEOBJ_H
#define GAMEOBJ_H

class Sprite;
class Painter;

class GameObj
{
public:
	virtual VOID	Update(float dt);
	virtual VOID	Draw(Painter* pScreen);
	virtual UINT32	Collide(GameObj* pRunner);

	GameObj(const Vector2 &vPos, INT nCollidePri, UINT32 uCollideDirFlag)
		:m_vPos(vPos), m_nCollidePri(nCollidePri), m_uCollideDirFlag(uCollideDirFlag){}
	virtual ~GameObj(){}
	VOID			SetPos(const Vector2 &vPos) { m_vPos = vPos; }

protected:
	Sprite*			m_pSprite;
	// motion and collide
public:
	INT				GetCollidePri() const		{ return m_nCollidePri; }
	UINT32			GetCollideDirFlag() const	{ return m_uCollideDirFlag; }
	AABBox			GetAABBox() const;
	Vector2			GetPos() const				{ return m_vPos; }
protected:
	Vector2			m_vPos;				// 当前位置
	INT				m_nCollidePri;		// 碰撞权限
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
};

class Tile : public GameObj
{
public:
	Tile(/*const Vector2 &vPos*/);
	~Tile();

	virtual UINT32	Collide(GameObj* pRunner);
};

class Player : public GameObj
{
public:
	Player();
	~Player();
	virtual VOID	Update(float dt);

	Vector2		m_vVel;
};

#endif