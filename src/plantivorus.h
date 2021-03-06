#ifndef PLANTIVORUS_H
#define PLANTIVORUS_H

#include "position.h"
#include <string>
#include <SDL2/SDL.h>

#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

/**
 * \class Plantivorus
 * \brief A dangerous plant (enemy)
 **/
class Plantivorus
{
	private:
		Position pos;
		SDL_Surface* plantivorus_image;	
		SDL_Texture* plantivorus_texture;
		SDL_Rect sprite_rect;
		SDL_Rect plantivorus_rect;

	public:
		static const int POS_0 = 0;
		static const int POS_1 = 64;
		static const int POS_2 = 128;

		//Constructor
		Plantivorus(std::string pPlantPath="", int pX=0, int pY=0)
		{
			pos = Position(pX, pY);

			plantivorus_image = IMG_Load(pPlantPath.c_str());

			sprite_rect.w = 96;
			sprite_rect.h = 64;
			sprite_rect.x = 0;
			sprite_rect.y = 0;

			plantivorus_rect.w = sprite_rect.w;
			plantivorus_rect.h = sprite_rect.h;
			plantivorus_rect.x = pos.get_x() * 64;
			plantivorus_rect.y = pos.get_y() * 64;
		}

		//Getter for plantivorus_rect (will be used for collsion)
		SDL_Rect* get_rect(){ return &plantivorus_rect; }

		//Render the texture through given renderer
		void render(SDL_Renderer* pRenderer);

		//Switch spikes length
		void switch_position();

		//Initialize texture
		bool init_texture(SDL_Renderer* pRenderer);

		//Getter for door texture
		SDL_Texture* get_texture(){return plantivorus_texture;}
};

#endif
