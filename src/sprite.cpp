#include "common.h"

#include "sprite.h"
#include "window.h"
#include "cfg_file.h"
#include "res_mgr.h"

//------------------------------------------SpriteStatic-----------------------------------------------------------------
void SpriteStatic::Draw( Window* pScreen, int x, int y )
{
	SDL_Rect rect;
	rect.x = x % pScreen->GetW();
	rect.y = y % pScreen->GetH();
	rect.w = (_img->w / 2 + x) / 2;
	rect.h = (_img->h / 2 + y) / 2;

	SDL_BlitSurface(_img, NULL, pScreen->GetSurface(), &rect);
}

bool SpriteStatic::SetSurface( SDL_Surface* img )
{
	if( 0 == img ) return false;

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 32;
	rect.h = 32;
	_img = sResMgr.CreateSubSurface(img,&rect, false);

	return true;
}

//------------------------------------------SpriteMultChannel-----------------------------------------------------------------

bool SpriteMultChannel::Init( char* file_ )
{
	CfgFile file;
	if( !file.LoadFile(file_) ) return false;
	if( !file.LoadLine() ) return false;

	if( 0 != strcmp(file.GetPart(0), "image_file") ) 	return false;

	SDL_Surface* image = sResMgr.CreateSurface(file.GetPart(1));
	if( image == 0 ) return false;
	
	std::vector<Channel>	vec_channel;

	while( file.LoadLine() )
	{
		if( 0 == strcmp(file.GetPart(0), "channel_begin") )
		{
			Channel channel;
			std::vector<Frame> vec_frame;

			while( file.LoadLine() )
			{
				if( file.GetPart(0) == 0 ) continue;

				if( 0 == strcmp(file.GetPart(0), "channel_end") )
				{
					channel.frames = (Frame*)malloc(vec_frame.size() * sizeof(Frame));
					std::copy(vec_frame.begin(), vec_frame.end(), channel.frames);
					channel.frames_num = vec_frame.size();
					vec_channel.push_back(channel);

					break;
				}

				SDL_Rect rect;
				rect.x = atoi(file.GetPart(0));
				rect.y = atoi(file.GetPart(1));
				rect.w = atoi(file.GetPart(2));
				rect.h = atoi(file.GetPart(3));
				bool flip = atoi(file.GetPart(4)) != 0;

				Frame frame;
				frame.surface = sResMgr.CreateSubSurface(image, &rect, flip);
				frame.end = (float)atof(file.GetPart(5));

				vec_frame.push_back(frame);
			}

		}
	}

	SDL_FreeSurface(image);

	_channels = (Channel*)malloc(sizeof(Channel) * vec_channel.size());
	std::copy(vec_channel.begin(), vec_channel.end(), _channels);
	_channel_num = vec_channel.size();

	_cur_channel = -1;
	_cur_time = 0.0f;

	return true;
}

void SpriteMultChannel::Destroy()
{
	for( int i=0; i<_channel_num; ++i )
	{
		for( int j=0; j<_channels[i].frames_num; ++j )
		{
			SDL_FreeSurface(_channels[i].frames[j].surface);
		}
		free(_channels[i].frames);
	}

	free(_channels);
	_channels = 0;
}

void SpriteMultChannel::Update( float dt )
{
	if( !_play ) return;
	if( _cur_channel < 0 || _cur_channel >= _channel_num ) return;

	_cur_time += dt;
	if( _cur_time >= _channels[_cur_channel].frames[_channels[_cur_channel].frames_num-1].end )
	{
		_cur_time = 0.0f;
	}
}

void SpriteMultChannel::Draw( Window* pScreen, int x, int y )
{
	if( !_play ) return;
	if( _cur_channel < 0 || _cur_channel >= _channel_num ) return;

	Channel* cur_channel = &_channels[_cur_channel];

	int idx = 0;
	for(; idx<cur_channel->frames_num; ++idx) if( _cur_time < cur_channel->frames[idx].end ) break;

	SDL_Rect rect;
	SDL_Surface* surface = cur_channel->frames[idx].surface;
	if( surface == 0 ) return;

	rect.w = surface->w;
	rect.h = surface->h;
	rect.x = x % pScreen->GetW();
	rect.y = y % pScreen->GetH();
	
	SDL_BlitSurface(surface, 0, pScreen->GetSurface(), &rect);
}

bool SpriteMultChannel::PlayChannel( int channel_ )
{
	if( channel_ < 0 || channel_ >= _channel_num ) return false;

	if( _cur_channel != channel_ )
	{
		_cur_time = 0.0f;
	}

	_cur_channel = channel_;

	_play = true;

	return true;
}

void SpriteMultChannel::Stop()
{
	_play = false;
}
