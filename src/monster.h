#ifndef MONSTER_H
#define MONSTER_H

#include "position.h"
#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include <string>

/**
 * \class Monster
 * \brief Monster enemy
 **/
class Monster
{
	private:
		Position pos;
		std::string monster_path; 

		SDL_Surface* monster_image;
		SDL_Texture* monster_texture;
		SDL_Rect sprite_rect;
		SDL_Rect monster_rect;

		int direction;
		int x_min;
		int x_max;

		//Move on X axis 
		void move_x(int step);

	public:
		static const int RIGHT = 0;
		static const int LEFT = 1;
		
		static const int POS_0 = 0*64;
		static const int POS_1 = 1*64;
		static const int POS_2 = 2*64;

		static const int STEP_X = 64;
		static const int STEP_Y = 64;

		//Constructor
		Monster(std::string pPath, int pXmin, int pXmax, int pY)
		{
			pos = Position(pXmin, pY);

			direction = RIGHT;
			x_min = pXmin;
			x_max = pXmax;

			monster_image = IMG_Load(pPath.c_str());

			sprite_rect.w = 64;
			sprite_rect.h = 64;
			sprite_rect.x = POS_0;
			sprite_rect.y = 0;

			monster_rect.w = sprite_rect.w;
			monster_rect.h = sprite_rect.h;
			monster_rect.x = pXmin * 64;
			monster_rect.y = pY * 64;
		}

		//Getter for monster_rect (will be used for collsion)
		SDL_Rect* get_rect(){ return &monster_rect; }

		//Initialize the monster texture
		bool init_texture(SDL_Renderer* pRenderer);

		//Destroy
		void dispose();

		//Set the monster position to the given position
		void set_pos(Position pPosition);
		
		//Move
		void move();
		
		//Render the texture through given renderer
		void render(SDL_Renderer* pRenderer);

};

#endif
