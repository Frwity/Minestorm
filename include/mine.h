#ifndef __MINE_H__
#define __MINE_H__

#include "2d_tools.h"

typedef struct s_convexPoly t_convexPoly;

typedef struct s_ref t_ref;

typedef struct s_bullet t_bullet;

typedef struct s_spawnpoint t_spawnpoint;

typedef struct s_mother_ship t_mother_ship;

typedef struct s_mine
{
	t_convexPoly* poly;
	t_spawnpoint* sp;
	t_ref ref;
	t_aabb aabb;
	int nb_branch;
	
	bool is_alive;
	
} t_mine;

typedef struct s_mines
{
	t_mine* mine;
	bool is_used;

} t_mines;


t_mine* create_mine(int size, E_MINE_TYPE type, t_spawnpoint* sp, int max);

t_mines* create_mines(int nb_mine, E_MINE_TYPE type, t_spawnpoint* sp, int total_mine);

void next_level(t_game* game);

int get_unused_mine_index(t_mines* mines, int max_mine);

void mines_render(t_game* game, SDL_Renderer* renderer ,t_mines* mines, int nb_mine);

void mine_render(SDL_Renderer* renderer ,t_mine* mine, bool aabb_display);

void mines_update(t_game* game ,t_mines* mines, int nb_mine, E_MINE_TYPE type);

void mine_update(float delta_time ,t_mine* mine, E_MINE_TYPE type);

void mine_moving(t_mine* mine, float delta_time, E_MINE_TYPE type);

void mine_boarder_teleport(t_mine* mine, float delta_time);

void mine_shoot(t_mine* mine, int max_bullet, t_bullet* bullets);

void mine_split(t_mines* mines, int nb_mine, int size, E_MINE_TYPE type, t_spawnpoint* sp, int max_sp);

int destroy_mine(t_mine* mine);

void destroy_mines(t_mines* mines, int nb_mine);

#endif
