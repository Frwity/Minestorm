#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_functions.h"
#include "2d_tools.h"
#include "game.h"
#include "mine.h"
#include "player.h"
#include "collisions.h"
#include "game_collisions.h"

bool hitbox_player_mine(t_player* player, t_mine* mine)
{
	t_convexPoly* polym;
	t_convexPoly* polyp;
	if (!hitbox_aabb(player->aabb, mine->aabb))
		return false;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < mine->nb_branch; ++j)
		{
			if (hitbox_aabb(player->poly[i].aabb, mine->poly[j].aabb))
			{
				polym = local_to_global_poly(mine->poly[j], mine->ref);
				polyp = local_to_global_poly(player->poly[i], player->ref);
				if (poly_intersections(*polym, *polyp))
				{
					free(polym->point);
					free(polym);
					free(polyp->point);
					free(polyp);
					return true;
				}
				free(polym->point);
				free(polym);
				free(polyp->point);
				free(polyp);
			}
		}
	}
	return false;
}

bool hitbox_bullet_mine(t_bullet* bullet, t_mine* mine)
{
	init_aabb(&bullet->aabb, bullet->circle.center);
	t_point point[2];
	point[0].x = bullet->circle.center.x - bullet->circle.r;
	point[0].y = bullet->circle.center.y - bullet->circle.r;
	point[1].x = bullet->circle.center.x + bullet->circle.r;
	point[1].y = bullet->circle.center.y + bullet->circle.r;
	update_aabb(&bullet->aabb, point, 2);

	t_convexPoly* polym;

	if (!hitbox_aabb(bullet->aabb, mine->aabb))
		return false;
	
	for (int j = 0; j < mine->nb_branch; ++j)
	{
		if (hitbox_aabb(bullet->aabb, mine->poly[j].aabb))
		{
			polym = local_to_global_poly(mine->poly[j], mine->ref);

			if (poly_circle_intersection(*polym, bullet->circle))
			{
				free(polym->point);
				free(polym);
				return true;
			}

			free(polym->point);
			free(polym);
		}	
	}
	return false;
}

bool hitbox_bullet_player(t_bullet* bullet, t_player* player)
{
	init_aabb(&bullet->aabb, bullet->circle.center);
	t_point point[2];
	point[0].x = bullet->circle.center.x - bullet->circle.r;
	point[0].y = bullet->circle.center.y - bullet->circle.r;
	point[1].x = bullet->circle.center.x + bullet->circle.r;
	point[1].y = bullet->circle.center.y + bullet->circle.r;
	update_aabb(&bullet->aabb, point, 2);

	t_convexPoly* polyp;

	if (!hitbox_aabb(bullet->aabb, player->aabb))
		return false;
	
	for (int j = 0; j < 2; ++j)
	{
		if (hitbox_aabb(bullet->aabb, player->poly[j].aabb))
		{
			polyp = local_to_global_poly(player->poly[j], player->ref);

			if (poly_circle_intersection(*polyp, bullet->circle))
			{
				free(polyp->point);
				free(polyp);
				return true;
			}

			free(polyp->point);
			free(polyp);
		}	
	}
	return false;
}

bool hitbox_bullets(t_bullet* bullet1, t_bullet* bullet2)
{
	if (circle_intersections(bullet1->circle, bullet2->circle))
		return true;
	return false;
}
