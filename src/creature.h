#ifndef CREATURE
#define CREATURE

class Creature
{
public:
	virtual bool	Init();
	virtual void	Update(float dt);
	virtual void	Destroy();


	virtual void	Draw();

	virtual Creature* Clone();

protected:
	// 碰撞部分
	// draw部分
	// 移动部分
};

class Player : public Creature
{

};

class Monster : public Creature
{

};

#endif