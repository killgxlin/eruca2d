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
	// ��ײ����
	// draw����
	// �ƶ�����
};

class Player : public Creature
{

};

class Monster : public Creature
{

};

#endif