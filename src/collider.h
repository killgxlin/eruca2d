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
	DWORD	dwCollidDirSet;
	DWORD	dwCollidTypeSet;

	DWORD	dwCollidDirSelf;
	DWORD	dwCollidTypeSelf;

	tagCollidCfg()
	{
		dwCollidDirSelf = 0;
		dwCollidTypeSelf = 0;

		dwCollidDirSet = 0;
		dwCollidTypeSet = 0;
	}
};

#endif