#ifndef __MOTHER_SHIP_H__
#define __MOTHER_SHIP_H__

#include "2d_tools.h"

typedef struct s_convexPoly t_convexPoly;

typedef struct s_ref t_ref;

typedef struct s_mother_ship
{
	t_convexPoly* poly;
	t_ref ref;

	bool is_moving;
	
	bool is_alive;
	
}t_mother_ship;

t_mother_ship* create_mother_ship();

void mother_ship_render(SDL_Renderer* renderer ,t_mother_ship* ms);

void mother_ship_update(t_mother_ship* ms, float delta_time);

void mother_ship_move(t_mother_ship* ms, float delta_time);

void destroy_mother_ship(t_mother_ship* mother_ship);

#endif
