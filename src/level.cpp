#include "level.h"
#include <iostream>

/**
 * load
 * \param SDL_Rendered* 
 * \brief Load the level
 * \return boolean : load level status
 **/
bool Level::load(SDL_Renderer* pRenderer)
{
	//Initialize the background image	
	bg_image = IMG_Load((lvl_bg_path).c_str());

	txt_font = TTF_OpenFont((lvl_asset_path + "ThinPencilHandwriting.ttf").c_str(), 40);
	if(!txt_font)
	{
		std::cerr << "Cannot load the font" << std::endl;
	}

	//Initialize the ground image
	ground_image = IMG_Load((lvl_asset_path + "ground.png").c_str());

	//Initialize the bg sprite
	bg_rect.w = 1024;
	bg_rect.h = 768;
	bg_rect.x = 0;
	bg_rect.y = 0;

	//Initialize the ground sprite
	sprite_rect.w = 64;
	sprite_rect.h = 16;
	sprite_rect.x = 0;
	sprite_rect.y = 0;

	//Load the map
	if(!load_map(lvl_map_path))
	{
		std::cerr << "Cannot load level map: " + lvl_map_path << std::endl;
		return false;
	}

	//Load level textures
	if(!init_textures(pRenderer)) 
	{
		std::cerr << "Cannot initialize level textures" << std::endl;
		return false;
	}

	//Initialize the music
	lvl_music = Mix_LoadMUS((lvl_asset_path + "sfx/music.ogg").c_str());
	if(!lvl_music)
	{
		std::cerr << "Cannot load music" << std::endl;
		return false;
	}

	//Initialize the eraser sound
	sfx_eraser = Mix_LoadWAV((lvl_asset_path + "sfx/eraser.wav").c_str());
	if(sfx_eraser == nullptr)
	{
		std::cerr << "Cannot load sound eraser" << std::endl;  
		return false;
	}
	Mix_VolumeChunk(sfx_eraser, 60);

	//Initialize the die sound
	sfx_die_splash = Mix_LoadWAV((lvl_asset_path + "sfx/dead_splash.wav").c_str());
	if(sfx_die_splash == nullptr)
	{
		std::cerr << "Cannot load sound die splash" << std::endl;  
		return false;
	}
	Mix_VolumeChunk(sfx_die_splash, 20);

	//Initialize get time sound
	sfx_get_time = Mix_LoadWAV((lvl_asset_path + "sfx/timer.wav").c_str());
	if(sfx_get_time == nullptr)
	{
		std::cerr << "Cannot load sound get time" << std::endl;  
		return false;
	}
	Mix_VolumeChunk(sfx_get_time, 20);

	is_load = true;

	//Play background music
	play_bg_music();

	return true;
}

/**
 * unload level
 * /brief unload level
 * /return void
 **/
void Level::unload()
{
	//Destroy textures
	SDL_DestroyTexture(bg_texture);
	SDL_DestroyTexture(ground_texture);
	SDL_DestroyTexture(lvl_player.get_texture());
	SDL_DestroyTexture(lvl_door.get_texture());

	for(auto &lvl_pencil : lvl_pencils)
	{
		SDL_DestroyTexture(lvl_pencil.get_texture());
	}

	for(auto &lvl_spike : lvl_spikes)
	{
		SDL_DestroyTexture(lvl_spike.get_texture());
	}

	for(auto &lvl_plant : lvl_plants)
	{
		SDL_DestroyTexture(lvl_plant.get_texture());
	}

	for(auto &lvl_arachne : lvl_arachnes)
	{
		SDL_DestroyTexture(lvl_arachne.get_texture());
	}

	for(auto &lvl_ghost : lvl_ghosts)
	{
		SDL_DestroyTexture(lvl_ghost.get_texture());
	}

	for(auto &lvl_tbonus : lvl_tbonuses)
	{
		SDL_DestroyTexture(lvl_tbonus.get_texture());
	}

	for(auto &lvl_monster : lvl_monsters)
	{
		lvl_monster.dispose();
	}

	//Stop music
	Mix_HaltMusic();
	Mix_FreeMusic(lvl_music);

	//Free sounds
	Mix_FreeChunk(sfx_eraser);
	Mix_FreeChunk(sfx_die_splash);
	Mix_FreeChunk(sfx_get_time);

	TTF_CloseFont(txt_font);
	SDL_DestroyTexture(timer_texture);

	lvl_ground.clear();
	lvl_player.reborn();

	is_load = false;
}

/**
 * add_rect
 * \param pX (int) : Position X
 * \param pY (int) : Position Y
 * \brief Add rect to lvl_ground vector
 **/
void Level::add_rect(int pX, int pY)
{
	SDL_Rect tmp_rect;
	tmp_rect.w = 64;
	tmp_rect.h = 16;
	tmp_rect.x = pX * 64;
	tmp_rect.y = pY * 64;

	lvl_ground.push_back(tmp_rect);	
}

/**
 * load_map
 * \param pMapFilepath (std::string) : Map file path
 * \brief load the level map
 * \return boolean : load level status
 **/
bool Level::load_map(std::string pMapFilepath)
{
	bool has_door = false;
	bool has_player = false;

	std::string lvl_player_path = lvl_asset_path + "playersheet.png";
	std::string lvl_door_path = lvl_asset_path + "hole.png";
	std::string lvl_pencil_path = lvl_asset_path + "pencil.png";
	std::string lvl_spike_path = lvl_asset_path + "spike.png";
	std::string lvl_plant_path = lvl_asset_path + "plant.png";
	std::string lvl_arachne_path = lvl_asset_path + "arachne.png";
	std::string lvl_ghost_path = lvl_asset_path + "ghost.png";
	std::string lvl_monster_path = lvl_asset_path + "monster.png";
	std::string lvl_timebonus_path = lvl_asset_path + "timer.png";

	std::ifstream lvl_file(pMapFilepath);

	if(lvl_file.is_open())
	{
		std::string line;
		float line_idx{0};
		float col_idx{0};

		int monster_x1{0};
		int monster_x2{0};

		while(getline(lvl_file, line))
		{
			for(auto lChar : line)
			{
				switch(lChar)
				{
					case '*': //Ground
						add_rect(col_idx, line_idx);
						break;
					case 'P': //Player
						has_player = true;
						lvl_player = Player(lvl_player_path, col_idx, line_idx);
						break;
					case 'D': //Door
						has_door = true;
						lvl_door = Door(lvl_door_path, col_idx, line_idx);
						break;
					case 'A': //Arachnee
						{
							Arachne lvl_arachne = Arachne(lvl_arachne_path, col_idx, line_idx);
							lvl_arachnes.push_back(lvl_arachne);
						}
						break;
					case '[': //Monster start
						monster_x1 = col_idx;
						break;
					case ']': //Monster end
						monster_x2 = col_idx;
						break;
					case 'S': //Spike
						{
							Spike lvl_spike = Spike(lvl_spike_path, col_idx, line_idx);
							lvl_spikes.push_back(lvl_spike);
						}
						break;
					case 'F': //Fleur
						{
							Plantivorus lvl_plant = Plantivorus(lvl_plant_path, col_idx, line_idx);
							lvl_plants.push_back(lvl_plant);

						}
						break;
					case 'G': //Ghost
						{
							Ghost lvl_ghost = Ghost(lvl_ghost_path, col_idx, line_idx);
							lvl_ghosts.push_back(lvl_ghost);
						}
						break;
					case 'T': //Time bonus
						{
							TimeBonus lvl_tbonus = TimeBonus(lvl_timebonus_path, col_idx, line_idx);
							lvl_tbonuses.push_back(lvl_tbonus);
						}
						break;
					case 'C': //Crayon
						{
							Pencil lvl_pencil = Pencil(lvl_pencil_path, col_idx, line_idx);
							lvl_pencils.push_back(lvl_pencil);
						}
						break;
				}
				col_idx++;
			}
		
			if(monster_x1 != monster_x2)
			{
				Monster lvl_monster = Monster(lvl_monster_path, monster_x1, monster_x2, line_idx);
			       	lvl_monsters.push_back(lvl_monster);	
			}

			monster_x1 = 0;
			monster_x2 = 0;
			col_idx = 0;
			line_idx++;
		}
		lvl_file.close();
	
		if(!has_player)
		{
			std::cerr << "There is no player in this level map (" + pMapFilepath + ") !!" << std::endl;
			return false;
		}

		if(!has_door)
		{
			std::cerr << "There is no exit in this level map (" + pMapFilepath + ") !!" << std::endl;
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

/**
 * init_texture
 * \param pRenderer : Game Renderer
 * \brief Initialize textures level to be rendered
 * \return boolean : init level texture status
 **/
bool Level::init_textures(SDL_Renderer* pRenderer)
{
	bg_texture = SDL_CreateTextureFromSurface(pRenderer, bg_image);
	if(bg_texture <= 0)
	{
		std::cerr << "Invalid background texture" << std::endl;
		return false;
	}
	SDL_FreeSurface(bg_image);
	
	ground_texture = SDL_CreateTextureFromSurface(pRenderer, ground_image);
	if(ground_texture <= 0)
	{
		std::cerr << "Invalid ground texture" << std::endl;
		return false;
	}
	SDL_FreeSurface(ground_image);

	for(auto &lvl_pencil : lvl_pencils)
	{
		if(!lvl_pencil.init_texture(pRenderer))
		{
			std::cerr << "Invalid pencil texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_spike : lvl_spikes)
	{
		if(!lvl_spike.init_texture(pRenderer))
		{
			std::cerr << "Invalid spike texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_plant : lvl_plants)
	{
		if(!lvl_plant.init_texture(pRenderer))
		{
			std::cerr << "Invalid plantivorus texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_arachne : lvl_arachnes)
	{
		if(!lvl_arachne.init_texture(pRenderer))
		{
			std::cerr << "Invalid arachne texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_ghost : lvl_ghosts)
	{
		if(!lvl_ghost.init_texture(pRenderer))
		{
			std::cerr << "Invalid ghost texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_monster : lvl_monsters)
	{
		if(!lvl_monster.init_texture(pRenderer))
		{
			std::cerr << "Invalid monster texture" << std::endl;
			return false;
		}
	}

	for(auto &lvl_tbonus : lvl_tbonuses)
	{
		if(!lvl_tbonus.init_texture(pRenderer))
		{
			std::cerr << "Invalid time bonus texture" << std::endl;
			return false;
		}
	}

	if(!lvl_door.init_texture(pRenderer))
	{
		std::cerr << "Invalid door texture" << std::endl;
		return false;
	}

	if(!lvl_player.init_texture(pRenderer))
	{
		std::cerr << "Invalid player texture" << std::endl;
		return false;
	}

	return true;
}

/**
 * play_bg_music
 * \brief Launch the music
 * \return void
 **/
void Level::play_bg_music()
{
	Mix_PlayMusic(lvl_music, -1);
	Mix_VolumeMusic(15);
}

/**
 * check_ground_collision
 * \brief Check for collision with a given SDL_Rect
 * \return boolean : collision status
 **/
bool Level::check_ground_collision()
{
	for(auto lGroundRect : lvl_ground)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), &lGroundRect))
		{
			return true;			
		}
	}
	return false;
}

/**
 * check_time_bonus_collision
 * \brief Check for collision with timer bonuses
 * \return int : collision status
 **/
int Level::check_time_bonus_collision()
{
	int cpt{0};
	int idx{-1};
	
	for(auto &lvl_tbonus : lvl_tbonuses)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_tbonus.get_rect()))
		{
			idx = cpt;			
		}
		cpt++;
	}
	return idx;
}


/**
 * check_danger_collision
 * \brief Check collisions with dangerous things
 * \return boolean : collision status
 **/
bool Level::check_danger_collision()
{
	//Check pencils
	for(auto &lvl_spike : lvl_spikes)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_spike.get_rect()))
		{
			return true;			
		}
	}

	//Check with plantivorus
	for(auto &lvl_plant : lvl_plants)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_plant.get_rect()))
		{
			return true;			
		}
	}

	//Check with ghost
	for(auto &lvl_ghost : lvl_ghosts)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_ghost.get_rect()))
		{
			return true;			
		}
	}

	//Check with arachne
	for(auto &lvl_arachne : lvl_arachnes)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_arachne.get_rect()))
		{
			return true;			
		}
	}

	//Check with monster
	for(auto &lvl_monster : lvl_monsters)
	{
		if(SDL_HasIntersection(lvl_player.get_rect(), lvl_monster.get_rect()))
		{
			return true;
		}
	}

	return false;
}

/**
 * check_door_collision
 * \brief Check if player collides with the door
 * \return boolean : collision status
 **/
bool Level::check_door_collision()
{
	SDL_Rect* lRect = lvl_door.get_rect();
	if(SDL_HasIntersection(lvl_player.get_rect(), lRect))
	{
		return true;
	}
	return false;
}

/**
 * refresh_timer
 * \param pRendered : Game renderer
 * \return void
 **/
void Level::refresh_timer(SDL_Renderer* pRenderer)
{
	std::string current_txt = std::to_string(available_time);

	SDL_Surface* txt_image = TTF_RenderText_Blended_Wrapped(txt_font, current_txt.c_str(), txt_color, bg_rect.w - 5);
	timer_texture = SDL_CreateTextureFromSurface(pRenderer, txt_image);
	SDL_FreeSurface(txt_image);
	
	int lWidth{0};
	int lHeight{0};
	SDL_QueryTexture(timer_texture, nullptr, nullptr, &lWidth, &lHeight);
	
	timer_rect.w = lWidth;
	timer_rect.h = lHeight;
	timer_rect.x = 0;
	timer_rect.y = 0;

	timer_pos_rect.w = timer_rect.w;
	timer_pos_rect.h = timer_rect.h;
	timer_pos_rect.x = 5*bg_rect.w/6 + 80;
	timer_pos_rect.y = 5;
}

/**
 * display_no_more_time
 * \param pRendered : Game renderer
 * \brief Display the time's up picture
 * \return void
 **/
void Level::display_no_more_time(SDL_Renderer* pRenderer)
{
	SDL_RenderClear(pRenderer);	
	std::string image_path = lvl_asset_path + "pic_notime.png";
	SDL_Surface* image = IMG_Load(image_path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, image);
	if(texture > 0)
	{
		SDL_FreeSurface(image);
		SDL_RenderCopy(pRenderer, texture, nullptr, nullptr);
		SDL_RenderPresent(pRenderer);
		
		//Slow down cycles
		SDL_Delay(2000);
	}
}

/**
 * display_fail
 * \param pRenderer : Game renderer
 * \brief Display failure message
 * \return void
 **/
void Level::display_fail(SDL_Renderer* pRenderer)
{
	SDL_RenderClear(pRenderer);	
	std::string image_path = lvl_asset_path + "pic_fail.png";
	SDL_Surface* image = IMG_Load(image_path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, image);
	if(texture > 0)
	{
		SDL_FreeSurface(image);
		SDL_RenderCopy(pRenderer, texture, nullptr, nullptr);
		SDL_RenderPresent(pRenderer);
		
		//Slow down cycles
		SDL_Delay(2000);
	}
}

/**
 * render level
 * \param pRenderer : Game renderer
 * \brief Render the texture through given renderer
 * \return boolean : level render status
 **/
bool Level::render(SDL_Renderer* pRenderer)
{
	SDL_RenderCopy(pRenderer, bg_texture, &bg_rect, &bg_rect);

	for(auto lGroundRect : lvl_ground)
	{	
		SDL_RenderCopy(pRenderer, ground_texture, &sprite_rect, &lGroundRect);
	}

	for(auto &lvl_pencil : lvl_pencils)
	{
		lvl_pencil.render(pRenderer);
	}

	for(auto &lvl_spike : lvl_spikes)
	{
		lvl_spike.render(pRenderer);
	}

	for(auto &lvl_plant : lvl_plants)
	{
		lvl_plant.render(pRenderer);
	}

	for(auto &lvl_arachne : lvl_arachnes)
	{
		lvl_arachne.render(pRenderer);
	}

	for(auto &lvl_ghost : lvl_ghosts)
	{
		lvl_ghost.render(pRenderer);
	}

	for(auto &lvl_monster : lvl_monsters)
	{
		lvl_monster.render(pRenderer);
	}

	for(auto &lvl_tbonus : lvl_tbonuses)
	{
		lvl_tbonus.render(pRenderer);
	}

	lvl_door.render(pRenderer);

	current_time = SDL_GetTicks();

	if(current_time > next_time_refresh)
	{
		available_time--;
		if(available_time <= 0)
		{
			display_no_more_time(pRenderer);
			return false;
		}
		refresh_timer(pRenderer);		
		next_time_refresh = current_time + 1000;
	}
	SDL_RenderCopy(pRenderer, timer_texture, &timer_rect, &timer_pos_rect);

	if(current_time > next_fall_down)
	{
		if(lvl_player.is_jumping())
		{
			lvl_player.walk();
		}
		else
		{
			lvl_player.fall(lvl_ground);
		}
		next_fall_down = current_time + 80;
	}

	if(current_time > next_monster_move)
	{
		for(auto &lvl_monster : lvl_monsters)
		{
			lvl_monster.move();
		}
		next_monster_move = current_time + 180;
	}

	if(current_time > next_spikes_update)
	{
		for(auto &lvl_spike : lvl_spikes)
		{
			lvl_spike.switch_spikes();
		}
	
		next_spikes_update = current_time + 210;
	}

	if(current_time > next_plants_update)
	{
		for(auto &lvl_plant : lvl_plants)
		{
			lvl_plant.switch_position();
		}
	
		next_plants_update = current_time + 1000;
	}

	if(current_time > next_arachnes_update)
	{
		for(auto &lvl_arachne : lvl_arachnes)
		{
			lvl_arachne.switch_position();
		}
	
		for(auto &lvl_ghost : lvl_ghosts)
		{
			lvl_ghost.switch_position();
		}

		next_arachnes_update = current_time + 600;
	}
	
	//Check if the player collides with dangerous things
	if(check_danger_collision())
	{
		//Play sound only if something erasable is under the eraser
		Mix_PlayChannel(-1, sfx_die_splash, 0); 
		SDL_Delay(200);
		display_fail(pRenderer);
		return false;
	}

	//Check if the player is in front of the door
	if(check_door_collision())
	{
		is_finish = true;
	}

	int tbonus_idx = check_time_bonus_collision();
	if(tbonus_idx > -1)
	{
		//Play tic-tac sound
		Mix_PlayChannel(-1, sfx_get_time, 0); 

		lvl_tbonuses.erase(lvl_tbonuses.begin() + tbonus_idx);
		
		available_time = available_time + TIME_BONUS_VALUE;
		refresh_timer(pRenderer);		
	}

	lvl_player.render(pRenderer);
	return true;
}

/**
 * erase_under 
 * \param pMouseX : Mouse X position
 * \parm pMouseY : Mouse Y position 
 * \brief Erase everything under the eraser (erasing it!)
 * \return boolean : erase under status
 **/
bool Level::erase_under(int pMouseX, int pMouseY)
{
	//Create a rect from the mouse coordinates
	SDL_Rect mouse_rect;
	mouse_rect.w = 32;
	mouse_rect.h = 32;
	mouse_rect.x = pMouseX;
	mouse_rect.y = pMouseY;
	
	//Test the spikes
	int cpt = 0;
	int removal_id = -1;
	
	for(auto &lvl_spike : lvl_spikes)
	{
		if(SDL_HasIntersection(&mouse_rect, lvl_spike.get_rect()))
		{
			removal_id = cpt;			
		}
		cpt++;
	}

	if(removal_id > -1)
	{
		lvl_spikes.erase(lvl_spikes.begin() + removal_id);
		return true;
	}

	//Test the plantivorus
	cpt = 0;
	for(auto &lvl_plant : lvl_plants)
	{
		if(SDL_HasIntersection(&mouse_rect, lvl_plant.get_rect()))
		{
			removal_id = cpt;			
		}
		cpt++;
	}

	if(removal_id > -1)
	{
		lvl_plants.erase(lvl_plants.begin() + removal_id);
		return true;
	}

	//Test arachnes
	cpt = 0;
	for(auto &lvl_arachne : lvl_arachnes)
	{
		if(SDL_HasIntersection(&mouse_rect, lvl_arachne.get_rect()))
		{
			removal_id = cpt;			
		}
		cpt++;
	}

	if(removal_id > -1)
	{
		lvl_arachnes.erase(lvl_arachnes.begin() + removal_id);
		return true;
	}

	//Test monsters
	cpt = 0;
	for(auto &lvl_monster : lvl_monsters)
	{
		if(SDL_HasIntersection(&mouse_rect, lvl_monster.get_rect()))
		{
			removal_id = cpt;			
		}
		cpt++;
	}

	if(removal_id > -1)
	{
		lvl_monsters.erase(lvl_monsters.begin() + removal_id);
		return true;
	}

	//Test timers (mouahhaha)
	cpt = 0;
	for(auto &lvl_tbonus : lvl_tbonuses)
	{
		if(SDL_HasIntersection(&mouse_rect, lvl_tbonus.get_rect()))
		{
			removal_id = cpt;			
		}
		cpt++;
	}

	if(removal_id > -1)
	{
		lvl_tbonuses.erase(lvl_tbonuses.begin() + removal_id);
		return true;
	}


	return false;
}

/**
 * on_event
 * \param pEvent : Game event 
 * \brief Handle SDL events 
 * \return void
 **/
void Level::on_event(SDL_Event* pEvent)
{
	switch(pEvent->type)
	{
		case SDL_KEYDOWN:
			switch(pEvent->key.keysym.sym)
			{
				case SDLK_LEFT:
					lvl_player.move_x(-1);
					if(check_ground_collision() == true)
					{
						lvl_player.move_x(1);
					}
					break;
				case SDLK_RIGHT:
					lvl_player.move_x(1);
					if(check_ground_collision() == true)
					{
						lvl_player.move_x(-1);
					}
					break;
				case SDLK_UP:
					lvl_player.move_y(-2);
					if(check_ground_collision() == true)
					{
						lvl_player.move_y(1);
					}
					else
					{
						lvl_player.jump();
					}
					break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(erase_under(pEvent->motion.x, pEvent->motion.y))
			{
				//Play sound only if something erasable is under the eraser
				Mix_PlayChannel(-1, sfx_eraser, 0); 
			}
			break;
	}
}	
