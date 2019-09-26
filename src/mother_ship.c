#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "2d_tools.h"
#include "game.h"
#include "draw.h"
#include "mother_ship.h"

t_mother_ship* create_mother_ship()
{
	t_mother_ship* ms = malloc(sizeof(t_mother_ship));

	ms->poly = malloc(4 * sizeof(t_convexPoly));
	
	ms->poly[0].point = malloc(3 * sizeof(t_point));
	ms->poly[1].point = malloc(4 * sizeof(t_point));
	ms->poly[2].point = malloc(3 * sizeof(t_point));
	ms->poly[3].point = malloc(3 * sizeof(t_point));

	ms->poly[0].nb_point = 3;
	ms->poly[0].point[0].x = 0;
	ms->poly[0].point[0].y = 0.5;
	ms->poly[0].point[1].x = -0.25;
	ms->poly[0].point[1].y = 0.25;
	ms->poly[0].point[2].x = 0.25;
	ms->poly[0].point[2].y = 0.25;

	ms->poly[1].nb_point = 4;
	ms->poly[1].point[0].x = -0.9;
	ms->poly[1].point[0].y = 0.25;
	ms->poly[1].point[1].x = 0.9;
	ms->poly[1].point[1].y = 0.25;
	ms->poly[1].point[2].x = 0.9;
	ms->poly[1].point[2].y = 0;
	ms->poly[1].point[3].x = -0.9;
	ms->poly[1].point[3].y = 0;

	ms->poly[2].nb_point = 3;
	ms->poly[2].point[0].x = 0.9;
	ms->poly[2].point[0].y = 0.25;
	ms->poly[2].point[1].x = 1.1;
	ms->poly[2].point[1].y = -0.166666;
	ms->poly[2].point[2].x = 0.9;
	ms->poly[2].point[2].y = 0;

	ms->poly[3].nb_point = 3;
	ms->poly[3].point[0].x = -0.9;
	ms->poly[3].point[0].y = 0.25;
	ms->poly[3].point[1].x = -1.1;
	ms->poly[3].point[1].y = -0.166666;
	ms->poly[3].point[2].x = -0.9;
	ms->poly[3].point[2].y = 0.0;

	ms->ref.scale = 70;
	
	ms->is_moving = true;

	init_ref(&ms->ref);
	ms->ref.angle = M_PI;
	ms->ref.center.x = -50;
	ms->ref.center.y = 150;
	ref_update(&ms->ref);

	return ms;
}

void mother_ship_render(SDL_Renderer* renderer ,t_mother_ship* ms)
{
	t_convexPoly** poly = malloc(4 * sizeof(t_convexPoly*));
	for (int i = 0; i < 4; ++i)
		poly[i] = local_to_global_poly(ms->poly[i], ms->ref);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < poly[i]->nb_point - 1; ++j)
		{
			SDL_SetRenderDrawColor(renderer, 70, 102, 255, 255);
			SDL_RenderDrawLine(renderer, poly[i]->point[j].x,
						   			     poly[i]->point[j].y, 
										 poly[i]->point[j + 1].x, 
										 poly[i]->point[j + 1].y);
		}
	SDL_RenderDrawLine(renderer, poly[0]->point[0].x,
				   			     poly[0]->point[0].y, 
								 poly[0]->point[2].x, 
								 poly[0]->point[2].y);
	
	for (int i = 0; i < 4; ++i)
	{
		free(poly[i]->point);
		free(poly[i]);
	}
	free(poly);
}

void mother_ship_update(t_mother_ship* ms, float delta_time)
{
	if (ms->is_moving)
		mother_ship_move(ms, delta_time);
	if (ms->ref.center.x > WIDTH + 50)
	{
		ms->is_moving = false;
		ms->ref.center.x = -50;
	}
}

void mother_ship_move(t_mother_ship* ms, float delta_time)
{
	ms->ref.center.x += 150 * delta_time;
}

void destroy_mother_ship(t_mother_ship* mother_ship)
{
	if (!mother_ship)
		return;
	for (int i = 0; i < 4; ++i)
		free(mother_ship->poly[i].point);
	free(mother_ship->poly);
	free(mother_ship);
}
