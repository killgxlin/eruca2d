#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

//-----------------------------------------------------------------------------------------------

class Animal;
class Arrow;
class Sprite;
class Painter;
class Player;
class Terrain;
class Movable;
//-----------------------------------------------------------------------------------------------
#include "listener.h"
#include "collide.h"



//-----------------------------------------------------------------------------------------------

class GameObj
{
public:
	virtual VOID	Update(FLOAT dt);
	virtual VOID	Draw(Painter* pScreen);
	VOID			NormalCollide(GameObj* pRunner, tagCollideRes* pRes);
	VOID			BulletCollide(GameObj* pRunner, tagCollideRes* pRes);

	virtual BOOL	Init(UINT32 uCollideDirFlag);
	virtual VOID	Destroy();

	virtual	VOID	SetPos(const Vector2F &vPos) { m_vPos = vPos; }
	VOID			SetCollideDirFlag(UINT32 uFlag) { m_uCollideDirFlag = uFlag; }
	Sprite*			GetSprite() const { return m_pSprite; }
	VOID			SetColor(UINT8 u8R, UINT8 u8G, UINT8 u8B);
	VOID			SetDelete()					{ m_bDelete = TRUE; }
	BOOL			IsDeleted() const			{ return m_bDelete; }


protected:
	Sprite*			m_pSprite;
	BOOL			m_bDelete;

	// motion and collide
public:
	UINT32			GetCollideDirFlag() const	{ return m_uCollideDirFlag; }
	Square			GetAABBox() const;
	Vector2F		GetPos() const				{ return m_vPos; }
	Vector2F		GetPrePos() const			{ return m_vPrePos; }
	Vector2F		GetSize() const;

	virtual VOID	CheckTouch(Arrow* pArrow, tagCollideRes* pRes)		{}
	virtual VOID	CheckTouch(Player* pPlayer, tagCollideRes* pRes)	{}
	virtual VOID	CheckTouch(Animal* pAnimal, tagCollideRes* pRes)	{}
	virtual VOID	CheckTouch(Terrain* pTerrain, tagCollideRes* pRes)	{}


protected:
	Vector2F		m_vPos;				// 当前位置
	Vector2F		m_vPrePos;			// 之前位置
	UINT32			m_uCollideDirFlag;	// 碰撞的方向
};
//-----------------------------------------------------------------------------------------------

class Terrain : public GameObj
{
public:
	virtual BOOL	Init();
	virtual VOID	Destroy();

	float			m_fDist;
};
//-----------------------------------------------------------------------------------------------


class Movable : public GameObj, public tagPhysic
{
public:
	virtual BOOL	Init(DWORD dwDirFlag);
	virtual VOID	Destroy();

	virtual VOID	Update(float dt);

	tagCtrlData		m_Input;
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

	virtual VOID	CheckTouch(Terrain* pTerrain, tagCollideRes* pRes);
	virtual VOID	CheckTouch(Arrow* pArrow, tagCollideRes* pRes);
	virtual VOID	CheckTouch(Animal* pAnimal, tagCollideRes* pRes);

	float			m_fJump;
	BOOL			m_bJmpPressed;
	Vector2F		m_vArrowDir;
};
//-----------------------------------------------------------------------------------------------

class Arrow : public Movable
{
public:
	BOOL			Init(Player* pOwner);
	VOID			Destroy();

	virtual VOID	UpdatePhysic( FLOAT dt );
	virtual VOID	Update(FLOAT dt);

	virtual VOID	CheckTouch(Terrain* pTerrain, tagCollideRes* pRes);

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

	virtual VOID	CheckTouch(Terrain* pTerrain, tagCollideRes* pRes);
	virtual VOID	CheckTouch(Arrow* pArrow, tagCollideRes* pRes);

	float			m_fJump;
	BOOL			m_bJmpPressed;

};


#endif