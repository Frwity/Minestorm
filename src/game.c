#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "2d_tools.h"
#include "my_functions.h"
#include "game.h"
#include "player.h"
#include "mine.h"
#include "mother_ship.h"
#include "draw.h"

void create_game(t_game* game)
{
	if (game == NULL)
		return;
	TTF_Init();
	if (game->state != GAME_RESTART)
 		game->state = GAME_START;

	state = &game->state;
	
	game->current_time = SDL_GetTicks();
    game->time = 0;
    game->delta_time = (game->time > 0) ? ((game->current_time - game->time) / 1000.f) : 1.0f / 60.0f;
    game->time = game->current_time;

	game->player = create_player();
	game->player2 = create_player();
	
	game->mother_ship = create_mother_ship();

	g_closest_player = game->player;
	
	game->level = 1;
	game->next_level = 0;
	
	game->nb_fmine = 14;	
	game->nb_mmine = 0;	
	game->nb_firemine = 7;	
	game->nb_mfiremine = 0;

	game->no_fmine = false;
	game->no_mmine = false;
	game->no_firemine = false;
	game->no_mfiremine = false;

	game->total_mine = (game->nb_fmine + game->nb_mmine + game->nb_firemine + game->nb_mfiremine);
	game->sp = create_spawnpoint(game->total_mine);

	game->aabb_display = false;
	game->next_level_setup = false;
	game->two_player = false;

	game->font = TTF_OpenFont("media/open_sans.ttf", 24);
	game->background = IMG_Load("media/background.png");
	game->foreground = IMG_Load("media/foreground.png");
}

t_spawnpoint* create_spawnpoint(int nb)
{
	t_spawnpoint* sp = malloc(nb * sizeof(t_spawnpoint));
	
	for (int i = 0; i < nb; ++i)
	{
		sp[i].to_reach.x = 80 + (rand() % (WIDTH - 160));
		sp[i].to_reach.y = 80 + (rand() % (HEIGHT - 200));
		sp[i].point.x = 80 + ((WIDTH - 120) / nb * i); 
		sp[i].point.y = 150; 
		sp[i].is_used = false;
		sp[i].in_pos = false;
		sp[i].is_moving = false;
		sp[i].to_display = false;
	}

	return sp;
}

void spawnpoint_render(SDL_Renderer* renderer, t_spawnpoint* sp, int nb_sp)
{	
	t_circle c;
	for (int i = 0; i < nb_sp; ++i)
	{
		if (!sp[i].to_display)
			continue;
		c.center.x = sp[i].point.x;
		c.center.y = sp[i].point.y;
		c.r = 1.5;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		draw_circle(renderer, c);
	}
}

void spawnpoint_setup(t_spawnpoint* sp, t_mother_ship* ms, int nb_sp, bool* setup_end)
{
	for (int i = 0; i < nb_sp; ++i)
	{
		if (sp[i].is_moving || sp[i].in_pos)
			continue;
		if (sp[i].point.x < ms->ref.center.x)
		{
			sp[i].is_moving = true;
			sp[i].to_display = true;
		}
	}
	for (int i = 0; i < nb_sp; ++i)
		if (!sp[i].in_pos)
			return;
	*setup_end = true;
}

void spawnpoint_moving(t_spawnpoint* sp, int nb_sp, float delta_time)
{
	for (int i = 0; i < nb_sp; ++i)
	{
		if (sp[i].is_moving && !sp[i].in_pos)
		{
			sp[i].point.x -= unit_vector(sub_vector(sp[i].point, sp[i].to_reach)).x * delta_time * 200;
			sp[i].point.y -= unit_vector(sub_vector(sp[i].point, sp[i].to_reach)).y * delta_time * 200;
		}
		if (is_vector_equal(sp[i].point, sp[i].to_reach))
		{
			sp[i].is_moving = false;
			sp[i].in_pos = true;
		}
	}
}

int init_game(t_game* game)
{
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return 1;
	if (game->state != GAME_RESTART)
		game->window = SDL_CreateWindow("MINESTORM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	else 
		game->state = GAME_START;
	if (game->window == NULL)
		return 1;
	game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
	if (game->renderer == NULL)
		return 1;
	return 0;
}

int game_loop(t_game* game)
{
	if (!game)
		return 0;
	while (game->state != GAME_EXIT)
	{
		game->current_time = SDL_GetTicks();
   		game->delta_time = (game->time > 0) ? ((game->current_time - game->time) / 1000.f) : 1.0f / 60.0f;
       	game->time = game->current_time;
		
		game_input(game);
		if (game->state == GAME_EXIT)
			break;
		
		if (game_state(game))
			continue;

		
		mother_ship_update(game->mother_ship, game->delta_time);

		if (!game->next_level_setup)
		{
			spawnpoint_setup(game->sp, game->mother_ship, game->total_mine, &game->next_level_setup);
			spawnpoint_moving(game->sp, game->total_mine, game->delta_time);
			if (game->next_level_setup)
				next_level(game);
		}
		else
		{
			player_update(game, game->player);
			if (game->two_player)
				player_update(game, game->player2);
			mines_update(game, game->fmines, game->nb_fmine, FLOATING);		//TODO all mines update
			mines_update(game, game->mmines, game->nb_mmine, MAGNETIC);
			mines_update(game, game->firemines, game->nb_firemine, FIREBALL);
			mines_update(game, game->mfiremines, game->nb_mfiremine, MAGNETFIRE);

			for (int i = 0; i < game->max_mine_bullet; ++i)				//MINES BULLETS UPDATE
				bullet_update(&game->mine_bullets[i], game->delta_time);
		}
		game_render(game, game->renderer);
	
	}
	return 0;
}

void check_for_game_over(t_game* game)
{
	if (game->player->life <= 0)
	{
		game->player->is_alive = false;
		if (!game->two_player)
			game->state = GAME_OVER;
	}
	if (game->player2->life <= 0)
	{
		game->player2->is_alive = false;
		if (game->player->life <= 0)
			game->state = GAME_OVER;
	}
}

bool game_state(t_game* game)
{
	check_for_game_over(game);
	if (!game)
		return 0;
	if (game->state == GAME_PAUSE || game->state == GAME_START)
	{
		game_render(game, game->renderer);
		return 1;
	}
	
	if (game->state == GAME_OVER || game->state == GAME_OVER2)
	{
		game_render(game, game->renderer);
		return 1;
	}
	return 0;
}

int game_input(t_game* game)
{
	if (!game)
		return 0;
    SDL_Event event;
        while (SDL_PollEvent(&event))
        {
        	switch (event.type)
       	    {
           		case SDL_QUIT:
               		game->state = GAME_EXIT;
               		break;
               	case SDL_KEYDOWN:
               		if (event.key.keysym.sym == SDLK_ESCAPE)
                   	{
                		game->state = GAME_EXIT;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_UP)
                   	{
                		game->player2->is_moving_forward = true;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_LEFT)
                   	{
                		game->player2->is_moving_left = true;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_RIGHT)
                   	{
                		game->player2->is_moving_right = true;
               			break;
					}
          			if (event.key.keysym.sym == SDLK_DOWN)
                   	{
						player_teleport(game->player2);
               			break;
					}
          			if (event.key.keysym.sym == SDLK_KP_0)
                   	{
						player_shoot(game->player2);
               			break;
					}
               		if (event.key.keysym.sym == SDLK_w)
                   	{
                		game->player->is_moving_forward = true;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_a)
                   	{
                		game->player->is_moving_left = true;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_d)
                   	{
                		game->player->is_moving_right = true;
               			break;
					}
          			if (event.key.keysym.sym == SDLK_s)
                   	{
						player_teleport(game->player);
               			break;
					}
          			if (event.key.keysym.sym == SDLK_SPACE)
                   	{
						player_shoot(game->player);
               			break;
					}
          			if (event.key.keysym.sym == SDLK_h)
                   	{
						game->aabb_display = !game->aabb_display;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_f || event.key.keysym.sym == SDLK_k)
                   	{
						if (game->state == GAME_START)
							game->state = GAME_RUNNING;
               			if (event.key.keysym.sym == SDLK_k)
							game->two_player = !game->two_player;
						break;
					}
               		if (event.key.keysym.sym == SDLK_p)
                   	{
						if (game->state == GAME_RUNNING)
						{
							game->state = GAME_PAUSE;
							break;
						}
						if (game->state == GAME_PAUSE)
						{
							game->state = GAME_RUNNING;
							break;
						}
					}
               		if (event.key.keysym.sym == SDLK_r)
                   	{
						game->state = GAME_RESTART;
						destroy_game(game);
						create_game(game);
						init_game(game);
                		break;
					}
				case SDL_KEYUP:
               		if (event.key.keysym.sym == SDLK_UP)
                   	{
                		game->player2->is_moving_forward = false;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_LEFT)
                   	{
                		game->player2->is_moving_left = false;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_RIGHT)
                   	{
                		game->player2->is_moving_right = false;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_w)
                   	{
                		game->player->is_moving_forward = false;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_a)
                   	{
                		game->player->is_moving_left = false;
               			break;
					}
               		if (event.key.keysym.sym == SDLK_d)
                   	{
                		game->player->is_moving_right = false;
               			break;
					}
			       	default:
                   		break;
            }
        }
	return 0;
}

int game_render(t_game* game, SDL_Renderer* renderer)
{
	if (!game || !renderer)
		return 0;
    SDL_RenderClear(renderer);
	SDL_Texture* back = SDL_CreateTextureFromSurface(renderer, game->background);
  	SDL_Texture* fore = SDL_CreateTextureFromSurface(renderer, game->foreground);
	SDL_RenderCopy(renderer, back, NULL, NULL);
	
	if (game->state != GAME_START)
		display_text(game);
	
	spawnpoint_render(renderer, game->sp, game->total_mine);

	
	if (game->next_level_setup)
	{
		player_render(game, game->renderer, game->player);
		if (game->two_player)
			player_render(game, game->renderer, game->player2);

		mines_render(game, game->renderer, game->fmines, game->nb_fmine);
		mines_render(game, game->renderer, game->mmines, game->nb_mmine);
		mines_render(game, game->renderer, game->firemines, game->nb_firemine);
		mines_render(game, game->renderer, game->mfiremines, game->nb_mfiremine);
	
		bullets_render(renderer, game->mine_bullets, game->max_mine_bullet);
	}

	mother_ship_render(renderer, game->mother_ship);

	
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
	SDL_Color color = {235, 235, 235};
	if (game->state == GAME_PAUSE)
	{
		display_key(game);
		add_text(game, "PAUSED", 80, 200, 480, 150, color);
	}
	if (game->state == GAME_START)
	{
		display_key(game);
		color.r = 225; color.g = 120; color.b = 120;
		add_text(game, "Press \"f\" 1 player", 80, 200, 480, 100, color);
		color.r = 120; color.g = 225; color.b = 120;
		add_text(game, "Press \"k\" 2 players", 80, 300, 480, 100, color);
	}

	game_over(game);
	
	SDL_RenderCopy(renderer, fore, NULL, NULL);
	
	SDL_RenderPresent(renderer);
    SDL_Delay(1);
	SDL_DestroyTexture(back);
	SDL_DestroyTexture(fore);

	return 1;
}

void game_over(t_game* game)
{
	if (!game)
		return;
	if (game->state == GAME_OVER || game->state == GAME_OVER2)
	{
       	SDL_SetRenderDrawColor(game->renderer, 40, 20, 100, 255);
       	SDL_Rect rect = { 100, 150, 440, 370 };
       	SDL_RenderFillRect(game->renderer, &rect);
		
		SDL_Color color = {255, 255, 255};
		add_text(game, "GAME OVER", 170, 170, 300, 100, color);
		add_text(game, "Press \"r\" to restart", 215, 380, 200, 50, color);
		add_text(game, "Press \"ESCAPE\" to quit", 215, 450, 200, 50, color);
			
		color.r = 255; color.g = 120; color.b = 120;
		char* text = int_to_alpha(game->player->score);
		add_text(game, text, 270, 275, 60, 50, color);
		add_text(game, "Score P1:", 130, 275, 140, 50, color);
		free(text);
		if (game->two_player)
		{		
			color.r = 120; color.g = 225; color.b = 120;
			text = int_to_alpha(game->player2->score);
			add_text(game, text, WIDTH - 170, 315, 60, 50, color);
			add_text(game, "Score P2:", WIDTH - 320, 315, 140, 50, color);
			free(text);
		}
    }
}

void display_text(t_game* game)
{
	if (!game->font || !game)
		return;
	
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

	SDL_Color color = {225, 120, 120};
	char* text = int_to_alpha(game->player->score);
	add_text(game, text, 205, 600, 65, 40, color);
	free(text);
	text = "Score P1:";
	add_text(game, text, 75, 600, 130, 40, color);
	text = "Life P1:";
	add_text(game, text, 75, 550, 130, 40, color);
	text = int_to_alpha(game->player->life);
	add_text(game, text, 207, 550, 25, 40, color);
	free(text);
	
	if (game->two_player)
	{
		color.r = 120; color.g = 225; color.b = 120;
		text = "Score P2:";
		add_text(game, text, WIDTH - 260, 600, 130, 40, color);
		text = int_to_alpha(game->player2->score);
		add_text(game, text, WIDTH - 130, 600, 65, 40, color);
		free(text);
		text = "Life P2:";
		add_text(game, text, WIDTH - 235, 550, 130, 40, color);
		text = int_to_alpha(game->player2->life);
		add_text(game, text, WIDTH - 100, 550, 25, 40, color);
		free(text);
	}

}

void display_key(t_game* game)
{
	if (!game->font || !game)
		return;
	SDL_Color color = {225, 120, 120};
	char* text = "Player 1 key :";
	add_text(game, text, 85, 80, 80, 20, color);
	text = "- Forward : W";
	add_text(game, text, 85, 100, 80, 20, color);
	text = "- Turn Right : D";
	add_text(game, text, 85, 120, 100, 20, color);
	text = "- Turn Left : A";
	add_text(game, text, 85, 140, 100, 20, color);
	text = "- Teleport : S";
	add_text(game, text, 85, 160, 90, 20, color);
	text = "- SHOOT : SPACE";
	add_text(game, text, 85, 180, 100, 20, color);
	
	color.r = 120; color.g = 225; color.b = 120;
	text = "Player 2 key :";
	add_text(game, text, 440, 80, 100, 20, color);
	text = "- Forward : UP";
	add_text(game, text, 440, 100, 100, 20, color);
	text = "- Turn Right : RIGHT";
	add_text(game, text, 440, 120, 130, 20, color);
	text = "- Turn Left : LEFT";
	add_text(game, text, 440, 140, 120, 20, color);
	text = "- Teleport : DOWN";
	add_text(game, text, 440, 160, 110, 20, color);
	text = "- SHOOT : KEYPAD 0";
	add_text(game, text, 440, 180, 120, 20, color);
	
	color.r = 255; color.g = 255; color.b = 255;
	
	text = "PAUSE : P";
	add_text(game, text, 270, 80, 80, 30, color);
	text = "EXIT : ESC";
	add_text(game, text, 270, 110, 80, 30, color);
	text = "AABB : H";
	add_text(game, text, 270, 140, 80, 30, color);
	text = "Restart : R";
	add_text(game, text, 270, 170, 80, 30, color);

}

void add_text(t_game* game, char* str, int x, int y, int w, int h, SDL_Color color)
{
	if (!game || !str)
		return;
	SDL_Rect Message_rect;
	Message_rect.x = x; 
	Message_rect.y = y; 
	Message_rect.w = w;
	Message_rect.h = h;
	SDL_Surface* surface = TTF_RenderText_Solid(game->font, str, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(game->renderer, texture, NULL, &Message_rect);
	SDL_DestroyTexture(texture);
} 

void destroy_game(t_game* game)
{
	if (!game)
		return;
	TTF_CloseFont(game->font);
	SDL_DestroyRenderer(game->renderer);
	SDL_FreeSurface(game->background);
	SDL_FreeSurface(game->foreground);
	if (game->state != GAME_RESTART)
		SDL_DestroyWindow(game->window);
	
	destroy_mines(game->fmines, game->nb_fmine);
	destroy_mines(game->mmines, game->nb_mmine);
	destroy_mines(game->firemines, game->nb_firemine);
	destroy_mines(game->mfiremines, game->nb_mfiremine);
	
	if ((game->next_level_setup || game->level > 1) && game->mine_bullets)
		free(game->mine_bullets);

	free(game->sp);
	
	destroy_mother_ship(game->mother_ship);
	destroy_player(game->player);
	destroy_player(game->player2);
}
