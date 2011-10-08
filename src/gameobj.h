#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__
//-----------------------------------------------------------------------------------------------

class Sprite;
class Painter;
class Player;
class Movable;

//-----------------------------------------------------------------------------------------------

struct tagCollideRes 
{
	DWORD		dwDirFlag;
	Vector2F	vCollidePos;
};

struct tagPhysic
{
	Vector2F		m_vVel;
	BOOL			m_bLand;
	BOOL			m_bFaceRight;
	BOOL			m_bHitWall;

	Movable*		m_pMover;

	virtual VOID	UpdatePhysic( FLOAT dt );
	virtual BOOL	Init(Movable* pMover);
	virtual VOID	Destroy();
};


union tagInputMap
{
	struct  
	{
		UINT16	bJump:1,
				bLeft:1,
				bRight:1,
				bZoomIn:1,
				bZoomOut:1,
				bFire:1,
				bUp:1,
				bDown:1,
				bConstruct:1;
	};
	BYTE		u16Data;
};

struct InputListener
{
	Movable*		m_pMover;
public:
	
	virtual VOID	Listen(tagInputMap &input)
	{
		input.u16Data = 0;
	}
};

struct KeyBoardListener : public InputListener
{
public:
	virtual VOID	Listen(tagInputMap &input)
	{
		InputListener::Listen(input);

		input.bJump	= g_keyboard.GetKey(SDLK_UP);
		input.bLeft	= g_keyboard.GetKey(SDLK_LEFT);
		input.bRight	= g_keyboard.GetKey(SDLK_RIGHT);
		input.bZoomIn	= g_keyboard.GetKey(SDLK_z);
		input.bZoomOut= g_keyboard.GetKey(SDLK_x);
		input.bFire	= g_keyboard.FetchKey(SDLK_LCTRL);
		input.bConstruct = g_keyboard.FetchKey(SDLK_c);
		input.bUp = g_keyboard.GetKey(SDLK_e);
		input.bDown = g_keyboard.GetKey(SDLK_r);
	}
};

struct DummyListener : public InputListener
{
public:
	virtual VOID	Listen(tagInputMap &input)
	{
		InputListener::Listen(input);
	}
};

struct AIListener : public InputListener
{
public:
	virtual VOID	Listen(tagInputMap &input);
};

enum EGameObjType
{
	EGOT_Player		= 1,
	EGOT_Arrow		= 2,
	EGOT_Animal		= 3,
	EGOT_Terrain	= 4,
};

//-----------------------------------------------------------------------------------------------

class GameObj
{
public:
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Draw(Painter* pScreen);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

	virtual BOOL	Init(const Vector2F &vPos, UINT32 uCollideDirFlag, EGameObjType eGOT);
	virtual VOID	Destroy();

	virtual	VOID	SetPos(const Vector2F &vPos) { m_vPos = vPos; }
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
	Vector2F		GetSize() const;
	Vector2F		GetPrePos() const			{ return m_vPrePos; }

	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
	VOID			SetDelete()					{ m_bDelete = TRUE; }
	BOOL			IsDeleted() const			{ return m_bDelete; }
protected:
	Vector2F		m_vPos;				// 当前位置
	Vector2F		m_vPrePos;
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
	EGameObjType	m_eGot;				// 对象类型
	BOOL			m_bDelete;
};
//-----------------------------------------------------------------------------------------------

class Terrain : public GameObj
{
public:
	virtual BOOL	Init();
	virtual VOID	Destroy();

	virtual VOID	Collide(Movable* pMover, tagCollideRes* pRes);
	float			m_fDist;
};
//-----------------------------------------------------------------------------------------------


class Movable : public GameObj, public tagPhysic
{
public:
	virtual BOOL	Init(const Vector2F &vPos, DWORD dwDirFlag, EGameObjType eGOT);
	virtual VOID	Destroy();

	virtual VOID	Update(float dt);

	tagInputMap		m_Input;
	InputListener*	m_pListener;
};
//-----------------------------------------------------------------------------------------------

class Player : public Movable
{
public:
	BOOL			Init();
	VOID			Destroy();

	virtual VOID	UpdatePhysic(float dt);
	virtual VOID	Update(FLOAT dt);

	float			m_fJump;
	BOOL			m_bJmpPressed;
	FLOAT			m_fBear;
};
//-----------------------------------------------------------------------------------------------

class Arrow : public Movable
{
public:
	BOOL			Init(Player* pOwner);
	VOID			Destroy();

	virtual VOID	UpdatePhysic( FLOAT dt );
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);

	Player*			m_pOwner;
	Vector2F		m_vLastVel;
};
//-----------------------------------------------------------------------------------------------

class Animal : public Movable
{
public:
	BOOL			Init();
	VOID			Destroy();

	virtual VOID	UpdatePhysic( FLOAT dt );
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Collide(GameObj* pRunner, tagCollideRes* pRes);
	float			m_fJump;
	BOOL			m_bJmpPressed;

};


#endif