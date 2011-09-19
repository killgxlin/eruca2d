#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

class Sprite;
class Painter;

class Player;
class Movable;


struct tagCollideRes 
{
	DWORD		dwDirFlag;
	Vector2F	vCollidePos;
};

struct tagPhysic
{
	Vector2F		m_vVel;
	bool			m_bLand;


	Movable*		m_pMover;

	virtual VOID	UpdatePhysic( FLOAT dt );

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

enum EGameObjType
{
	EGOT_Player		= 1,
	EGOT_Arrow		= 2,
	EGOT_Animal		= 3,
	EGOT_Terrain	= 4,
};


class GameObj
{
public:
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Draw(Painter* pScreen);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

	GameObj(const Vector2F &vPos, UINT32 uCollideDirFlag, EGameObjType eGOT)
		:m_vPos(vPos), m_uCollideDirFlag(uCollideDirFlag), m_eGot(eGOT){}
	virtual ~GameObj(){}
	VOID			SetPos(const Vector2F &vPos) { m_vPos = vPos; }
	VOID			SetCollideDirFlag(UINT32 uFlag) { m_uCollideDirFlag = uFlag; }
	Sprite*			GetSprite() const { return m_pSprite; }
protected:
	Sprite*			m_pSprite;
	// motion and collide
public:
	UINT32			GetCollideDirFlag() const	{ return m_uCollideDirFlag; }
	EGameObjType	GetType() const				{ return m_eGot; }
	AABBox			GetAABBox() const;
	AABBox			GetMoveBox() const;
	Vector2F		GetPos() const				{ return m_vPos; }
	Vector2F		GetPrePos() const			{ return m_vPrePos; }

	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
protected:
	Vector2F		m_vPos;				// 当前位置
	Vector2F		m_vPrePos;
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
	EGameObjType	m_eGot;				// 对象类型
};

class Terrain : public GameObj
{
public:
	Terrain(/*const Vector2F &vPos*/);
	~Terrain();

	virtual VOID Collide(Movable* pMover, tagCollideRes* pRes);
	float			m_fDist;
};


class Movable : public GameObj, public tagPhysic
{
public:
	Movable(const Vector2F &vPos, DWORD dwDirFlag, EGameObjType eGOT)
		:GameObj(vPos, dwDirFlag, eGOT){ m_Input.byData = 0; }

	virtual VOID	HandleInput() = 0;
	virtual VOID	Update(float dt);

	tagInputMap		m_Input;
};

class Player : public Movable
{
public:
	Player();
	~Player();

	virtual VOID	HandleInput();
	virtual VOID	UpdatePhysic(float dt);
	virtual VOID	Update(FLOAT dt);

	float			m_fJump;
	bool			m_bJmpPressed;

};

class Arrow : public Movable
{
public:
	Arrow();
	~Arrow();

	virtual VOID	HandleInput() { m_Input.byData = 0; }
	virtual VOID	UpdatePhysic( FLOAT dt );
	virtual VOID	Update(FLOAT dt);
//	virtual VOID	Draw(Painter* pScreen);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

};


#endif