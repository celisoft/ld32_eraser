#ifndef DOOR_H
#define DOOR_H

#include "position.h"
#include <string>
#include <SDL2/SDL.h>

#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

/**
 * \class Door
 * \brief Door game object
 **/
class Door
{
	private:
		Position pos;
		SDL_Surface* door_image;	
		SDL_Texture* door_texture;
		SDL_Rect sprite_rect;
		SDL_Rect door_rect;

	public:
		//Constructor
		Door(std::string pDoorPath="", int pX=0, int pY=0)
		{
			pos = Position(pX, pY);

			door_image = IMG_Load(pDoorPath.c_str());

			sprite_rect.w = 64;
			sprite_rect.h = 126;
			sprite_rect.x = 0;
			sprite_rect.y = 0;

			door_rect.w = sprite_rect.w;
			door_rect.h = sprite_rect.h;
			door_rect.x = pos.get_x() * 64;
			door_rect.y = pos.get_y() * 64;
		}

		//Getter for door_rect (will be used for collsion)
		SDL_Rect* get_rect(){ return &door_rect; }

		//Render the texture through given renderer
		void render(SDL_Renderer* pRenderer);

		//Initialize texture
		bool init_texture(SDL_Renderer* pRenderer);

		//Getter for door texture
		SDL_Texture* get_texture(){return door_texture;}
};

#endif
