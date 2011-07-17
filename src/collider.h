#ifndef COLLIDER
#define COLLIDER

enum ECollidDir
{
	ECF_Left	= 0x0001,
	ECF_Right	= 0x0002,
	ECF_Up		= 0x0004,
	ECF_Down	= 0x0008,
	ECF_All		= ECF_Left | ECF_Right | ECF_Up | ECF_Down,
};

enum ECollidType
{
	ECT_Player	= 0x0001,
	ECT_Ground	= 0x0002,
	ECT_Monster	= 0x0004,
	ECT_Build	= 0x0008,
};

struct tagCollidCfg
{
	Uint32	u32CollidDirSet;
	Uint32	u32CollidTypeSet;

	Uint32	u32CollidDirSelf;
	Uint32	u32CollidTypeSelf;

	tagCollidCfg()
	{
		u32CollidDirSelf = 0;
		u32CollidTypeSelf = 0;

		u32CollidDirSet = 0;
		u32CollidTypeSet = 0;
	}
};

class Collider
{
public:
	bool	Init();
	void	Destroy();

	void	Update(float dt);
};

#endif