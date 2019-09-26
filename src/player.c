#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_functions.h"
#include "2d_tools.h"
#include "game.h"
#include "collisions.h"
#include "mine.h"
#include "player.h"
#include "game_collisions.h"
#include "draw.h"

t_player* create_player()
{
	t_player* player = malloc(sizeof(t_player));

	player->poly = malloc(2 * sizeof(t_convexPoly));
	
	player->poly[0].point = malloc(3 * sizeof(t_point));
	player->poly[1].point = malloc(3 * sizeof(t_point));

	player->poly[0].nb_point = 3;
	player->poly[0].point[0].x = 0;
	player->poly[0].point[0].y = -1.2;
	player->poly[0].point[1].x = -0.5;
	player->poly[0].point[1].y = 0.2;
	player->poly[0].point[2].x = 0;
	player->poly[0].point[2].y = 0;
	
	player->poly[1].nb_point = 3;
	player->poly[1].point[0].x = 0;
	player->poly[1].point[0].y = -1.2;
	player->poly[1].point[1].x = 0.5;
	player->poly[1].point[1].y = 0.2;
	player->poly[1].point[2].x = 0;
	player->poly[1].point[2].y = 0;

	player->ref.scale = 20;

	init_ref(&player->ref);
	player->ref.center.x = 260;
	player->ref.center.y = 260;

	player->score = 0;
	player->life = 3;
	player->is_alive = true;
	
	player->max_bullet = 10;
	player->bullets = malloc(player->max_bullet * sizeof(t_bullet));
	for (int i = 0; i < player->max_bullet; ++i)
	{
		init_ref(&player->bullets[i].ref);
		player->bullets[i].circle.center.x = -10;
		player->bullets[i].circle.center.y = -10;
		player->bullets[i].circle.r = player->ref.scale / 10 + 1;
		player->bullets[i].is_alive = false;
	}
	
	player->thrust = 800;
	player->mass = 60;
	player->decel_coef = -100;
	player->max_speed = 1.2;
	player->Velocity.x = 0;
	player->Velocity.y = 0;
	
	player->is_moving_left = false;
	player->is_moving_forward = false;
	player->is_moving_right = false;

	return player;
}

void player_render(t_game* game, SDL_Renderer* renderer ,t_player* player)
{
	if (!player->is_alive)
		return;
	if (player == game->player2)
		SDL_SetRenderDrawColor(renderer, 120, 255, 120, 255);
	else
		SDL_SetRenderDrawColor(renderer, 255, 120, 120, 255);
	
	t_convexPoly** poly = malloc(2 * sizeof(t_convexPoly*));
	for (int i = 0; i < 2; ++i)
	{
		poly[i] = local_to_global_poly(player->poly[i], player->ref);
		update_aabb(&player->aabb, poly[i]->point, poly[i]->nb_point);
		update_aabb(&player->poly[i].aabb, poly[i]->point, poly[i]->nb_point);
	}
	
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < poly[i]->nb_point - 1; ++j)
		{
			SDL_RenderDrawLine(renderer, poly[i]->point[j].x,
						   			     poly[i]->point[j].y, 
										 poly[i]->point[j + 1].x, 
										 poly[i]->point[j + 1].y);
			if (poly[i]->point[j].x < 60 || poly[i]->point[j + 1].x < 60)
				SDL_RenderDrawLine(renderer, poly[i]->point[j].x + WIDTH - 120,
							   			     poly[i]->point[j].y, 
											 poly[i]->point[j + 1].x + WIDTH - 120, 
											 poly[i]->point[j + 1].y);
			if (poly[i]->point[j].x > WIDTH - 60 || poly[i]->point[j + 1].x > WIDTH - 60)
				SDL_RenderDrawLine(renderer, poly[i]->point[j].x - WIDTH + 120,
							   			     poly[i]->point[j].y, 
											 poly[i]->point[j + 1].x - WIDTH + 120, 
											 poly[i]->point[j + 1].y);
			if (poly[i]->point[j].y < 60 || poly[i]->point[j + 1].y < 60)
				SDL_RenderDrawLine(renderer, poly[i]->point[j].x,
							   			     poly[i]->point[j].y + HEIGHT - 148, 
											 poly[i]->point[j + 1].x, 
											 poly[i]->point[j + 1].y + HEIGHT - 148);
			if (poly[i]->point[j].y > HEIGHT - 88 || poly[i]->point[j + 1].y > HEIGHT - 88)
				SDL_RenderDrawLine(renderer, poly[i]->point[j].x,
							   			     poly[i]->point[j].y - HEIGHT + 148, 
											 poly[i]->point[j + 1].x, 
											 poly[i]->point[j + 1].y - HEIGHT + 148);
		}

	for (int i = 0; i < 2; ++i)
	{
		free(poly[i]->point);
		free(poly[i]);
	}
	free(poly);
	bullets_render(renderer, player->bullets, player->max_bullet);

	if (game->aabb_display)
	{
		for (int i = 0; i < 2; ++i)
			draw_aabb(renderer, &player->poly[i].aabb);
		draw_aabb(renderer, &player->aabb);
		for (int i = 0; i < player->max_bullet; ++i)
			if (player->bullets[i].is_alive)
				draw_aabb(renderer, &player->bullets[i].aabb);
	}
}

void bullets_render(SDL_Renderer* renderer, t_bullet* bullets, int max_bullet)
{	
	if (!bullets || (long int)bullets == 1)
		return;
	for (int i = 0; i < max_bullet; ++i)
	{
		if (bullets[i].is_alive)
			draw_circle(renderer, bullets[i].circle);
	}
}

void player_update(t_game* game ,t_player* player)
{
	if (!player->is_alive)
		return;
	player_moving(player, game->delta_time);
	ref_update(&player->ref);

	player_is_hit_by_mine(player, game->fmines, game->nb_fmine);
	player_is_hit_by_mine(player, game->mmines, game->nb_mmine);
	player_is_hit_by_mine(player, game->firemines, game->nb_firemine);
	player_is_hit_by_mine(player, game->mfiremines, game->nb_mfiremine);

	for (int i = 0; i < game->max_mine_bullet; i++)
		if (hitbox_bullet_player(&game->mine_bullets[i], player))
		{
			player_teleport(player);
			player->life--;
			game->mine_bullets[i].is_alive = false;
		}
	
	for (int i = 0; i < 2; ++i)
		init_aabb(&player->poly[i].aabb, player->ref.center);
	init_aabb(&player->aabb, player->ref.center);

	for (int i = 0; i < player->max_bullet; ++i)
	{
		if (!player->bullets[i].is_alive)
			continue;
		bullet_update(&player->bullets[i], game->delta_time);
	}

	bullet_mine_events(player->bullets, game->fmines, game->nb_fmine, FLOATING, &player->score, game);
	bullet_mine_events(player->bullets, game->mmines, game->nb_mmine, MAGNETIC, &player->score, game);
	bullet_mine_events(player->bullets, game->firemines, game->nb_firemine, FIREBALL, &player->score, game);
	bullet_mine_events(player->bullets, game->mfiremines, game->nb_mfiremine, MAGNETFIRE, &player->score, game);
}

void player_is_hit_by_mine(t_player* player, t_mines* mines, int nb_mine)
{
	if (!mines)
		return;
	for (int i = 0; i < nb_mine; ++i)
	{
		if (!mines[i].is_used)
			continue;
		if (hitbox_player_mine(player, mines[i].mine))
		{
			player_teleport(player);
			player->life--;
		}
	}
}

void bullet_mine_events(t_bullet* bullets, t_mines* mines, int nb_mine, E_MINE_TYPE type, int* score, t_game* game)
{
	if (!mines)
		return;
	int size;
	for (int i = 0; i < 10; ++i)
	{
		if (!bullets[i].is_alive)
			continue;

		for (int j = 0; j < game->max_mine_bullet; ++j)
		{
			if (hitbox_bullets(&bullets[i], &game->mine_bullets[j]))
			{
				bullets[i].is_alive = false;
				game->mine_bullets[j].is_alive = false;
			}
		}

		for (int j = 0; j < nb_mine; ++j)
		{
			if (!mines[j].is_used)
					continue;

			if (hitbox_bullet_mine(&bullets[i], mines[j].mine))
			{
				
				bullets[i].is_alive = false;
				mines[j].mine->is_alive = false;
				

				if (type == FIREBALL || type == MAGNETFIRE)
					mine_shoot(mines[j].mine, game->max_mine_bullet, game->mine_bullets);

				size = destroy_mine(mines[j].mine);
				mines[j].is_used = false;
				
				*score += type + 50 - size;

				mine_split(mines, nb_mine, size, type, game->sp, game->total_mine);
			}
		}
	}	
}

void bullet_update(t_bullet* bullet, float delta_time)
{
	bullet_moving(bullet, delta_time);
	bullet_boarder(bullet);
}

void bullet_moving(t_bullet* bullet, float delta_time)
{
	t_ref r;
	init_ref(&r);
	bullet->circle.center.x -= bullet->ref.j.x * delta_time * bullet->speed;
	bullet->circle.center.y -= bullet->ref.j.y * delta_time * bullet->speed;

}

void bullet_boarder(t_bullet* bullet)
{
	if ((bullet->circle.center.x < 60)
	|| (bullet->circle.center.x > 760)
	|| (bullet->circle.center.y < 60)
	|| (bullet->circle.center.y > 760))
		bullet->is_alive = false;

}

void player_moving(t_player* player, float delta_time)
{
	t_vector2 thrust;
	thrust.x = player->ref.j.x * player->thrust;	
	thrust.y = player->ref.j.y * player->thrust;	
	
	t_vector2 friction;
	friction.x = player->Velocity.x * player->decel_coef;	
	friction.y = player->Velocity.y * player->decel_coef;	

	if (vector_length(player->Velocity) < player->max_speed)
	{
		if (player->is_moving_forward)
		{
			player->Velocity.x += thrust.x / player->mass * delta_time;
			player->Velocity.y += thrust.y / player->mass * delta_time;
		}
	}

	if (vector_length(player->Velocity) > 0.2)
	{
		player->Velocity.x += friction.x / player->mass * delta_time;
		player->Velocity.y += friction.y / player->mass * delta_time;
	}
	else
	{
		player->Velocity.x = 0;
		player->Velocity.y = 0;
	}

	player->ref.center.x -= player->Velocity.x * 350 * delta_time;
	player->ref.center.y -= player->Velocity.y * 350 * delta_time;

	if (player->is_moving_left)
		player->ref.angle -= 5 * delta_time;
	if (player->is_moving_right)
		player->ref.angle += 5 * delta_time;

	player_boarder_teleport(player);

}

void player_boarder_teleport(t_player* player)
{
	if (player->ref.center.x < 60)
		player->ref.center.x = WIDTH - 60;
	if (player->ref.center.x > WIDTH - 60)
		player->ref.center.x = 60;
	if (player->ref.center.y < 60)
		player->ref.center.y = HEIGHT - 88;
	if (player->ref.center.y > HEIGHT - 88)
		player->ref.center.y = 60;
}

void player_teleport(t_player* player)
{
	player->ref.center.x = rand() % WIDTH;
	player->ref.center.y = rand() % HEIGHT;
}

void player_shoot(t_player* player)
{
	for (int i = 0; i < player->max_bullet; ++i)
	{
		if (!player->bullets[i].is_alive)
		{
			t_ref r;
			init_ref(&r);
			player->bullets[i].circle.center.x = player->ref.center.x + dot_product(player->ref.i, r.j) * player->ref.scale;
			player->bullets[i].circle.center.y = player->ref.center.y - dot_product(player->ref.i, r.i) * player->ref.scale;
			player->bullets[i].ref.angle = player->ref.angle; 
			ref_update(&player->bullets[i].ref);
			player->bullets[i].is_alive = true;
			player->bullets[i].speed = 800;
			return;
		}
	}
}

void destroy_player(t_player* player)
{
	for (int i = 0; i < 2; ++i)
		free(player->poly[i].point);
	free(player->bullets);
	free(player->poly);
	free(player);
}
