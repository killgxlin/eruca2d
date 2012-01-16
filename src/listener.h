#ifndef __LISTENER_H__
#define __LISTENER_H__

union tagCtrlData
{
	typedef UINT16 DataType;
	struct  
	{
		DataType	bJump:1,
					bLeft:1,
					bRight:1,
					bZoomIn:1,
					bZoomOut:1,
					bFire:1,
					bUp:1,
					bDown:1,
					bConstruct:1;
	};
	DataType	u16Data;
};

struct InputListener
{
	Movable*		m_pMover;
public:
	InputListener(Movable* pMover):m_pMover(pMover){}
	virtual ~InputListener(){ m_pMover = NULL; }
	virtual VOID	Listen(tagCtrlData &input)
	{
		input.u16Data = 0;
	}
};

struct KeyBoardListener : public InputListener
{
public:
	KeyBoardListener(Movable* pMover):InputListener(pMover){}
	virtual VOID	Listen(tagCtrlData &input)
	{
		InputListener::Listen(input);

		input.bJump	= g_keyboard.GetKey(SDLK_UP);
		input.bLeft	= g_keyboard.GetKey(SDLK_LEFT);
		input.bRight	= g_keyboard.GetKey(SDLK_RIGHT);
		input.bZoomIn	= g_keyboard.GetKey(SDLK_z);
		input.bZoomOut= g_keyboard.GetKey(SDLK_x);
		input.bFire	= g_keyboard.GetButton(SDL_BUTTON_LEFT);
		input.bConstruct = g_keyboard.FetchButton(SDL_BUTTON_RIGHT);
		input.bUp = g_keyboard.GetKey(SDLK_e);
		input.bDown = g_keyboard.GetKey(SDLK_r);
	}
};

struct DummyListener : public InputListener
{
public:
	DummyListener(Movable* pMover):InputListener(pMover){}
	virtual VOID	Listen(tagCtrlData &input)
	{
		InputListener::Listen(input);
	}
};

struct AIListener : public InputListener
{
public:
	AIListener(Movable* pMover):InputListener(pMover){}
	virtual VOID	Listen(tagCtrlData &input);
};
#endif