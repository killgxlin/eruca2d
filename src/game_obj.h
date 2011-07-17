#ifndef GAME_OBJ
#define GAME_OBJ

#include "collider.h"

class GameObj
{
public:
	bool			Init()
	{
		m_pSprite = new SpriteAnimate;
		m_pSprite->Init("sprites\\jump.txt");
		x = y = 100;
		
		vx = vy = 0;

		
	}
	void			Update(float dt);
	void			Destroy();
	void			Draw();

protected:
	tagCollidCfg	m_stCollidCfg;
	SpriteAnimate*	m_pSprite;

	int				x, y;
	int				vx, vy;
};

#endif