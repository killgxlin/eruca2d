#ifndef SPIRIT
#define SPIRIT


class Window;

class Sprite
{
public:
	virtual void	Update(float dt) = 0;
	virtual void	Draw(Window* pScreen, int x, int y) = 0;
};

class SpriteStatic : public Sprite
{
public:
	void	Update(float dt){}
	void	Draw(Window* pScreen, int x, int y);

	bool	SetSurface(SDL_Surface* img);

private:
	SDL_Surface*	_img;
};

class SpriteMultChannel : public Sprite
{
public:
	void	Update(float dt);
	void	Draw(Window* pScreen, int x, int y);

	struct Frame
	{
		SDL_Surface		*surface;
		float			end;;
	};

	struct Channel 
	{
		Frame*			frames;
		int				frames_num;
	};

	bool	Init(char* file_);
	void	Destroy();
	bool	PlayChannel(int channel_);
	int		GetChannelNum() const { return _channel_num; }
	void	Stop();

private:

	bool		_play;
	float		_cur_time;

	int			_cur_channel;
	int			_channel_num;
	Channel*	_channels;
};


#endif