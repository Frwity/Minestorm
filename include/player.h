#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "collisions.h"

typedef struct s_convexPoly t_convexPoly;

typedef struct s_ref t_ref;

typedef struct s_aabb t_aabb;

typedef struct s_mine t_mine;

typedef struct s_bullet t_bullet;

typedef struct s_player
{
	t_convexPoly* poly;
	
	t_ref ref;
	t_aabb aabb;

	int life;
	int score;

	t_bullet* bullets;
	int max_bullet;

	t_vector2 Velocity;
	float thrust;
	float mass;
	
	float decel_coef; // to DEFINE
	float max_speed;

	bool is_moving_left;
	bool is_moving_right;
	bool is_moving_forward;
	bool is_alive;

} t_player;

t_player* create_player();

void player_render(t_game* game, SDL_Renderer* renderer ,t_player* player);
	
void bullets_render(SDL_Renderer* renderer, t_bullet* bullet, int max_bullet);

void player_update(t_game* game ,t_player* player);

void player_is_hit_by_mine(t_player* player, t_mines* mines, int nb_mine);

void bullet_update(t_bullet* bullet, float);

void bullet_mine_events(t_bullet* bullet, t_mines* mines, int nb_mine, E_MINE_TYPE type, int* score, t_game* game);

void bullet_moving(t_bullet* bullet, float);

void bullet_boarder(t_bullet* bullet);

void player_moving(t_player* player, float delta_time);

void player_boarder_teleport(t_player* player);

void player_teleport(t_player* player);

void player_shoot(t_player* player);

void destroy_player(t_player* player);

#endif
