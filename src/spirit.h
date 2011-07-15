#ifndef SPIRIT
#define SPIRIT

#include "collider.h"

class Spirit
{
public:
	virtual void	Update(float dt) = 0;
	virtual void	Draw(Window* pScreen, int x, int y) = 0;

protected:
	tagCollidCfg	m_stCollidCfg;
};

class SpiritStatic : public Spirit
{
public:
	void	Update(float dt){}
	void	Draw(Window* pScreen, int x, int y)
	{

		SDL_Rect rect;
		rect.x = x % pScreen->GetW();
		rect.y = y % pScreen->GetH();
		rect.w = (_img->w / 2 + x) / 2;
		rect.h = (_img->h / 2 + y) / 2;

		SDL_BlitSurface(_img, NULL, pScreen->GetSurface(), &rect);
	}

	bool	SetSurface(SDL_Surface* img)
	{
		if( 0 == img ) return false;

		_img = img;

		return true;
	}

private:
	SDL_Surface*	_img;
};

class SpiritAnimate : public Spirit
{
public:
	void	Update(float dt)
	{
		if( _ani_num == 0 ) return;
		if( !_play ) return;

		_cur_time += dt;
		if( _cur_time >= _ani[_ani_num-1].end ) 
			_cur_time -= _ani[_ani_num-1].end;

	}
	void	Draw(Window* pScreen, int x, int y)
	{
		if( _ani_num == 0 ) return;

		int idx = 0;
		for(; idx<_ani_num; ++idx)
			if( _cur_time < _ani[idx].end )
				break;
				
		SDL_Rect rect;
		rect.x = x % pScreen->GetW();
		rect.y = y % pScreen->GetH();
		rect.w = _ani[idx].rect.w;
		rect.h = _ani[idx].rect.h;

		SDL_BlitSurface(_img, &(_ani[idx].rect), pScreen->GetSurface(), &rect);
	}

	struct Animate
	{
		SDL_Rect		rect;
		float			end;
	};
#define MAX_ANI_LEN 10

	bool	Init(char* file_)	
	{
		_play = false;
		_cur_time = 0.0f;

		for(int i=0; i<MAX_ANI_LEN; ++i)
		{
			_ani[i].end = 99999.9f;
			_ani[i].rect.x = _ani[i].rect.y = _ani[i].rect.w = _ani[i].rect.h = 0;
		}
		_ani_num = 0;


		CfgFile file;
		if( !file.LoadFile(file_) ) return false;
		if( !file.LoadLine() ) return false;

		_img = IMG_Load(file.GetPart(0));

		if( 0 == _img )
		{
			char* err = IMG_GetError();
			return false;
		}

		SDL_SetColorKey(_img, SDL_SRCCOLORKEY, *(Uint8 *)(_img->pixels));

		while( file.LoadLine() )
		{
			int x = atoi(file.GetPart(0));
			int y = atoi(file.GetPart(1));
			int w = atoi(file.GetPart(2));
			int h = atoi(file.GetPart(3));
			
			float end = atof(file.GetPart(4));

			Set(x, y, w, h, end);			
		}




		return true;			
	}

	void	Set(int x_, int y_, int w_, int h_, float end_)
	{
		if( _ani_num != 0 && _ani[_ani_num-1].end >= end_ ) return;

		_ani[_ani_num].rect.x = x_;
		_ani[_ani_num].rect.y = y_;
		_ani[_ani_num].rect.w = w_;
		_ani[_ani_num].rect.h = h_;

		_ani[_ani_num].end = end_;
		++_ani_num;
	}

	void	Play() { _play = true; _cur_time = 0; }
	void	Stop() { _play = false; }

private:
	bool	_play;
	float	_cur_time;

	Animate	_ani[MAX_ANI_LEN];
	int		_ani_num;

	SDL_Surface*	_img;

};
/*
class SpiritPlayer : public SpiritAnimate
{
public:
	bool	Init(const char* file_)
	{
		if( !SpiritAnimate::Init(file_) )
			return false;

		m_stCollidCfg.dwCollidTypeSelf = ECT_Player;

		return true;
	}
};

class SpiritGround : public SpiritStatic
{
public:

};
*/
#endif