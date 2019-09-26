#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "my_functions.h"
#include "game.h"


int main(int argc, char* argv[])
{
	srand(time(NULL));
	t_game game;
	create_game(&game);
	init_game(&game);

	game_loop(&game);
 
	destroy_game(&game);
	SDL_Quit();
	return 0;
}
