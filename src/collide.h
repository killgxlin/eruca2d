#ifndef __COLLIDE_H__
#define __COLLIDE_H__

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


#endif