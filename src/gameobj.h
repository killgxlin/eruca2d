#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

class Sprite;
class Painter;

struct tagCollideRes 
{
	DWORD		dwDirFlag;
	Vector2F	vCollidePos;
};

union tagInputMap
{
	struct  
	{
		BYTE	bJump:1,
				bLeft:1,
				bRight:1,
				bZoomIn:1,
				bZoomOut:1,
				bFire:1;
	};
	BYTE		byData;
};

class GameObj
{
public:
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Draw(Painter* pScreen);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

	GameObj(const Vector2F &vPos, UINT32 uCollideDirFlag)
		:m_vPos(vPos), m_uCollideDirFlag(uCollideDirFlag){}
	virtual ~GameObj(){}
	VOID			SetPos(const Vector2F &vPos) { m_vPos = vPos; }
	VOID			SetCollideDirFlag(UINT32 uFlag) { m_uCollideDirFlag = uFlag; }
	Sprite*			GetSprite() const { return m_pSprite; }
protected:
	Sprite*			m_pSprite;
	// motion and collide
public:
	UINT32			GetCollideDirFlag() const	{ return m_uCollideDirFlag; }
	AABBox			GetAABBox() const;
	AABBox			GetMoveBox() const;
	Vector2F		GetPos() const				{ return m_vPos; }
	Vector2F		GetPrePos() const			{ return m_vPrePos; }

	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
protected:
	Vector2F		m_vPos;				// 当前位置
	Vector2F		m_vPrePos;
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
};

class Terrain : public GameObj
{
public:
	Terrain(/*const Vector2F &vPos*/);
	~Terrain();

	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);
	float		m_fDist;
};

class Player;

struct tagPhysic
{
	Vector2F		m_vVel;
	Vector2F		m_vAcc;
	bool			m_bLand;
	float			m_fJump;
	bool			m_bJmpPressed;

	Player*			m_pPlayer;

	VOID			UpdatePhysic( FLOAT dt );

};

class Movable : public GameObj, public tagPhysic
{
public:
	Movable(const Vector2F &vPos, DWORD dwDirFlag)
		:GameObj(vPos, dwDirFlag){}
};

class Player : public Movable
{
	friend struct tagPlayerListener;

public:
	Player();
	~Player();
	VOID			Listen();
	virtual VOID	Update(FLOAT dt);

	tagInputMap		m_Input;
};


#endif