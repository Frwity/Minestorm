#ifndef __GAME_H__
#define __GAME_H__

#include <SDL2/SDL_ttf.h>
#include "2d_tools.h"

#define WIDTH 640
#define HEIGHT 780

typedef struct s_player t_player;

typedef struct s_mother_ship t_mother_ship;

t_player* g_closest_player;

typedef struct s_mine t_mine;
typedef struct s_mines t_mines;

typedef struct s_bullet
{
	t_circle circle;
	t_ref ref;
	t_aabb aabb;
	float speed;

	bool is_alive;

} t_bullet;

typedef struct s_spawnpoint
{
	t_point point;
	t_point to_reach;
	bool is_used;
	bool to_display;
	bool in_pos;
	bool is_moving;

} t_spawnpoint;
	
typedef enum
{
	FLOATING = 10,
	MAGNETIC = 40,
	FIREBALL = 70,
	MAGNETFIRE = 100

} E_MINE_TYPE;

typedef enum 
{
	GAME_OVER,
	GAME_OVER2,
	GAME_START,
	GAME_PAUSE,
	GAME_RUNNING,
	GAME_RESTART,
	GAME_EXIT
	
} E_GAME_STATE;

E_GAME_STATE* state;

typedef struct s_game
{
	

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* background;
	SDL_Surface* foreground;
	TTF_Font* font;

	t_player* player;
	t_player* player2;
////////
	t_mother_ship* mother_ship;
	
	t_spawnpoint* sp;
	t_mines* fmines;
	t_mines* mmines;
	t_mines* firemines;
	t_mines* mfiremines;
	
	t_bullet* mine_bullets;
	int nb_fmine;
	int nb_mmine;
	int nb_firemine;
	int nb_mfiremine;
	int max_mine_bullet;
	int total_mine;
	
	Uint32 level;
	Uint8 next_level;

	Uint8 add_fmine;
	Uint8 add_mmine;
	Uint8 add_firemine;
	Uint8 add_mfiremine;

	bool no_fmine;
	bool no_mmine;
	bool no_firemine;
	bool no_mfiremine;
////////
	E_GAME_STATE state;
	
	Uint32 time;
	float delta_time;
	int current_time;

	bool aabb_display;
	bool next_level_setup;
	bool two_player;
	
}t_game;

#define NEXT_FMINE 2
#define NEXT_MMINE 3
#define NEXT_FIREMINE 6
#define NEXT_MFIREMINE 8

void create_game(t_game* game);

t_spawnpoint* create_spawnpoint(int nb);
void spawnpoint_render(SDL_Renderer* renderer, t_spawnpoint* sp, int nb_sp);
void spawnpoint_setup(t_spawnpoint* sp, t_mother_ship* ms, int nb_sp, bool* setup_end);
void spawnpoint_moving(t_spawnpoint* sp, int nb_sp, float delta_time);

int init_game(t_game* game);

int game_loop(t_game* game);

bool game_state(t_game* game);
void check_for_game_over(t_game* game);

int game_input(t_game* game);

int game_render(t_game* game, SDL_Renderer* renderer);

void display_text(t_game* game);

void display_key(t_game* game);

void add_text(t_game* game, char* str, int x, int y, int w, int h, SDL_Color color);

void game_over(t_game* game);

void destroy_game(t_game* game);

#endif
