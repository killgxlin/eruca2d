#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

class Sprite;
class Painter;

struct tagCollideRes 
{
	DWORD	dwDirFlag;
	FLOAT	fDeep;
};

class GameObj
{
public:
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Draw(Painter* pScreen);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

	GameObj(const Vector2F &vPos, INT nCollidePri, UINT32 uCollideDirFlag)
		:m_vPos(vPos), m_nCollidePri(nCollidePri), m_uCollideDirFlag(uCollideDirFlag){}
	virtual ~GameObj(){}
	VOID			SetPos(const Vector2F &vPos) { m_vPos = vPos; }
	VOID			SetCollideDirFlag(UINT32 uFlag) { m_uCollideDirFlag = uFlag; }

protected:
	Sprite*			m_pSprite;
	// motion and collide
public:
	INT				GetCollidePri() const		{ return m_nCollidePri; }
	UINT32			GetCollideDirFlag() const	{ return m_uCollideDirFlag; }
	AABBox			GetAABBox() const;
	Vector2F		GetPos() const				{ return m_vPos; }

	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
protected:
	Vector2F		m_vPos;				// 当前位置
	INT				m_nCollidePri;		// 碰撞权限
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
};

class Tile : public GameObj
{
public:
	Tile(/*const Vector2F &vPos*/);
	~Tile();

	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);
};

class Player : public GameObj
{
public:
	Player();
	~Player();
	virtual VOID	Update(FLOAT dt);

	Vector2F		m_vVel;
};

#endif