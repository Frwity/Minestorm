#ifndef __GAME_COLLISIONS_H__
#define __GAME_COLLISIONS_H__

#include <stdbool.h>
#include "2d_tools.h"

bool hitbox_player_mine(t_player* player, t_mine* mine);

bool hitbox_bullet_mine(t_bullet* bullet, t_mine* mine);

bool hitbox_bullet_player(t_bullet* bullet, t_player* player);

bool hitbox_bullets(t_bullet* bullet1, t_bullet* bullet2);

#endif
