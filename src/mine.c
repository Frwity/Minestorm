#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_functions.h"
#include "2d_tools.h"
#include "game.h"
#include "mine.h"
#include "collisions.h"
#include "draw.h"
#include "player.h"
#include "mother_ship.h"


t_mine* create_mine(int size, E_MINE_TYPE type, t_spawnpoint* sp, int max)
{
	if (max == 0)
		return NULL;
	t_mine* mine = malloc(sizeof(t_mine));
	float r1;
	float r2;
	if (type == FLOATING)
	{
		mine->nb_branch = 3;
		r1 = 1;
		r2 = 0.25;
	}
	else if (type == MAGNETIC)
	{
		mine->nb_branch = 4;
		r1 = 1;
		r2 = 0.5; 
	}
	else if (type == FIREBALL)
	{
		mine->nb_branch = 4;
		r1 = 1;
		r2 = 0.25; 
	}
	else if (type == MAGNETFIRE)
	{
		mine->nb_branch = 6;
		r1 = 1;
		r2 = 0.5; 
	}

	mine->poly = create_star_shape(mine->nb_branch, r1, r2);
	
	mine->ref.scale = size;
	init_ref(&mine->ref);
	mine->ref.angle = rand() % 360;
	ref_update(&mine->ref);

	for (int i = 0; i < max; ++i)
	{
		if (!sp[i].is_used)
		{
			mine->ref.center.x = sp[i].point.x;
			mine->ref.center.y = sp[i].point.y;
			mine->sp = &sp[i];
		   	sp[i].is_used = true;	
			break;
		}
	}

	mine->is_alive = true;
	return mine;
}

t_mines* create_mines(int nb_mine, E_MINE_TYPE type, t_spawnpoint* sp, int total_mine)
{
	t_mines* mines = malloc(nb_mine * sizeof(t_mines));
	for (int i = 0; i < nb_mine / 7; ++i)
	{
		mines[i].is_used = true;
		mines[i].mine = create_mine(50, type, sp, total_mine);
	}
	for (int i = nb_mine / 7; i < nb_mine; ++i)
		mines[i].is_used = false;

	return mines;
}

int get_unused_mine_index(t_mines* mines, int max_mine)
{
	if (!mines)
		return -1;
	for (int i = 0; i < max_mine; ++i)
	{
		if (!mines[i].is_used)
		{
			mines[i].is_used = true;
			return i;
		}
	}
	return -1;
}


void mines_render(t_game* game, SDL_Renderer* renderer ,t_mines* mines, int nb_mine)
{
	if (!mines)
		return;
	for (int i = 0; i < nb_mine; ++i)
		if (mines[i].is_used)
			mine_render(renderer , mines[i].mine, game->aabb_display);
}

void mine_render(SDL_Renderer* renderer ,t_mine* mine, bool aabb_display)
{
	
	t_convexPoly** poly = malloc(mine->nb_branch * sizeof(t_convexPoly*));
	for (int i = 0; i < mine->nb_branch; ++i)
	{
		poly[i] = local_to_global_poly(mine->poly[i], mine->ref);
		update_aabb(&mine->aabb, poly[i]->point, poly[i]->nb_point);
		update_aabb(&mine->poly[i].aabb, poly[i]->point, poly[i]->nb_point);
	}

	for (int i = 0; i < mine->nb_branch; ++i)
		for (int j = 1; j < poly[i]->nb_point - 1; ++j)
		{
			SDL_SetRenderDrawColor(renderer, 70, 102, 255, 255);
			SDL_RenderDrawLine(renderer, poly[i]->point[j].x,
						   			     poly[i]->point[j].y, 
										 poly[i]->point[j + 1].x, 
										 poly[i]->point[j + 1].y);
		}
	
	for (int i = 0; i < mine->nb_branch; ++i)
	{
		free(poly[i]->point);
		free(poly[i]);
	}
	free(poly);
	
	if (aabb_display)
	{
		for (int i = 0; i < mine->nb_branch; ++i)
			draw_aabb(renderer, &mine->poly[i].aabb);
		draw_aabb(renderer, &mine->aabb);
	}
}

void mines_update(t_game* game ,t_mines* mines, int nb_mine, E_MINE_TYPE type) //TODO Decompose
{
	if (!game->next_level_setup || nb_mine < 0)
		return;
	for (int i = 0; i < nb_mine; ++i)
	{
		if (mines[i].is_used)
			mine_update(game->delta_time ,mines[i].mine, type);
	}
	//////////////////////// Test if all mine are destroyed
	for (int i = 0; i < nb_mine; ++i)
	{
		if (mines[i].is_used && mines[i].mine->is_alive)
			return;
		else 
			continue;
	}
	///////////////////
	if (game->next_level != 4) // TEST for which type of mine are destroyed
	{	
		if (type == FLOATING && !game->no_fmine)
		{
			game->no_fmine = true;
			game->next_level++;
		}
		if (type == MAGNETIC && !game->no_mmine)
		{
			game->no_mmine = true;
			game->next_level++;
		}
		if (type == FIREBALL && !game->no_firemine)
		{
			game->no_firemine = true;
			game->next_level++;
		}
		if (type == MAGNETFIRE && !game->no_mfiremine)
		{
			game->no_mfiremine = true;
			game->next_level++;
		}
		if (game->next_level != 4)
			return;
	}
	game->next_level = 0;
	game->level++;
	
	if (++game->add_fmine == NEXT_FMINE)	// Increment nb of different mine 
	{
		game->nb_fmine += 7;
		game->add_fmine = 0;
	}
	if (++game->add_mmine == NEXT_MMINE)
	{
		game->nb_mmine += 7;
		game->add_mmine = 0;
	}
	if (++game->add_firemine == NEXT_FIREMINE)
	{
		game->nb_firemine += 7;
		game->add_firemine = 0;
	}
	if (++game->add_mfiremine == NEXT_MFIREMINE)
	{
		game->nb_mfiremine += 7;
		game->add_mfiremine = 0;
	}
	game->no_fmine = false;
	game->no_mmine = false;
	game->no_firemine = false;
	game->no_mfiremine = false;
	
	game->total_mine = game->nb_fmine + game->nb_mmine + game->nb_firemine + game->nb_mfiremine;
	free(game->sp);
	game->sp = create_spawnpoint(game->total_mine);

	game->mother_ship->is_moving = true;
	game->next_level_setup = false;

}

void next_level(t_game* game)
{
	if (game->level > 1)
	{
		free(game->fmines);
		free(game->mmines);
		free(game->firemines);
		free(game->mfiremines);
		free(game->mine_bullets);
	}

	game->fmines = create_mines(game->nb_fmine, FLOATING, game->sp, game->total_mine);
	game->mmines = create_mines(game->nb_mmine, MAGNETIC, game->sp, game->total_mine);
	game->firemines = create_mines(game->nb_firemine, FIREBALL, game->sp, game->total_mine);
	game->mfiremines = create_mines(game->nb_mfiremine, MAGNETFIRE, game->sp, game->total_mine);
	
	game->max_mine_bullet = game->nb_fmine + game->nb_mfiremine;

	if (game->max_mine_bullet == 0)
		return;
	game->mine_bullets = malloc(game->max_mine_bullet * sizeof(t_bullet)); //TODO CREATE_BULLETS()
	for (int i = 0; i < game->max_mine_bullet; ++i)
	{
		init_ref(&game->mine_bullets[i].ref);
		game->mine_bullets[i].circle.center.x = -10;
		game->mine_bullets[i].circle.center.y = -10;
		game->mine_bullets[i].circle.r = 3;
		game->mine_bullets[i].is_alive = false;
	}
}

void mine_update(float delta_time, t_mine* mine, E_MINE_TYPE type)
{
	mine->sp->to_display = !mine->is_alive;
	mine_moving(mine, delta_time, type);
	ref_update(&mine->ref);
	
	for (int i = 0; i < mine->nb_branch; ++i)
		init_aabb(&mine->poly[i].aabb, mine->ref.center);
	init_aabb(&mine->aabb, mine->ref.center);
}

void mine_moving(t_mine* mine, float delta_time, E_MINE_TYPE type)
{
	t_ref r;
	init_ref(&r);
	if (type == FLOATING || type == FIREBALL)
	{
		mine->ref.center.x += mine->ref.j.x * delta_time * (165 - (mine->ref.scale * 3));
		mine->ref.center.y -= mine->ref.j.y * delta_time * (165 - (mine->ref.scale * 3));
	}
	else //TODO the correct follow pattern with border
	{
		mine->ref.center.x -= unit_vector(sub_vector(mine->ref.center, g_closest_player->ref.center)).x * delta_time * (165 - (mine->ref.scale * 3));
		mine->ref.center.y -= unit_vector(sub_vector(mine->ref.center, g_closest_player->ref.center)).y * delta_time * (165 - (mine->ref.scale * 3));
		
	}

	mine_boarder_teleport(mine, delta_time);
}

void mine_boarder_teleport(t_mine* mine, float delta_time)
{
	if (mine->ref.center.x < 90)
		mine->ref.center.x = WIDTH - 90;
	if (mine->ref.center.x > WIDTH - 90)
		mine->ref.center.x = 90;
	if (mine->ref.center.y < 90)
		mine->ref.center.y = HEIGHT - 90;
	if (mine->ref.center.y > HEIGHT - 90)
		mine->ref.center.y = 90;
}

void mine_shoot(t_mine* mine, int max_bullet, t_bullet* bullets)
{
	for (int i = 0; i < max_bullet; ++i)
	{
		if (!bullets[i].is_alive)
		{
			bullets[i].ref.j.x = unit_vector(sub_vector(mine->ref.center, g_closest_player->ref.center)).x;
			bullets[i].ref.j.y = unit_vector(sub_vector(mine->ref.center, g_closest_player->ref.center)).y;
			bullets[i].circle.center.x = mine->ref.center.x;
			bullets[i].circle.center.y = mine->ref.center.y;
			bullets[i].is_alive = true;
			bullets[i].speed = 75;
			return;
		}
	}
}

void mine_split(t_mines* mines, int nb_mine, int size, E_MINE_TYPE type, t_spawnpoint* sp, int max_sp)
{
	if (size != 20)
	{
		int index;
		index = get_unused_mine_index(mines, nb_mine);
		if (index == -1)
			return;
		mines[index].mine = create_mine(size - 15, type, sp, max_sp);
		
		index = get_unused_mine_index(mines, nb_mine);
		if (index == -1)
			return;
		mines[index].mine = create_mine(size - 15, type, sp, max_sp);
	}	
}

int destroy_mine(t_mine* mine)
{
	if (!mine)
		return 0;
	int size = mine->ref.scale;
	for (int i = 0; i < mine->nb_branch; ++i)
	{
		free(mine->poly[i].point);
		mine->poly[i].point = NULL;
	}
	free(mine->poly);
	mine->poly = NULL;
	free(mine);
	mine = NULL;
	return size;
}

void destroy_mines(t_mines* mines, int nb_mine)
{
	if (!mines)
		return;
	for (int i = 0; i < nb_mine; ++i)
	{
		if (mines[i].is_used)
			destroy_mine(mines[i].mine);
	}
	free(mines);
	mines = NULL;
}
