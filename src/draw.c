#include <math.h>
#include "stdbool.h"
#include "2d_tools.h"

void set_pixel(SDL_Renderer* renderer, int x, int y)
{
	SDL_RenderDrawPoint(renderer, x, y);
}

void draw_circle(SDL_Renderer *ren, t_circle circle)
{
	double error = (double)-circle.r;
	double x = (double)circle.r - 0.5;
	double y = (double)0.5;
	double cx = circle.center.x - 0.5;
	double cy = circle.center.y - 0.5;

	while (x >= y)
	{
		set_pixel(ren, (int)(cx + x), (int)(cy + y));
		set_pixel(ren, (int)(cx + y), (int)(cy + x));

		if (x != 0)
		{
			set_pixel(ren, (int)(cx - x), (int)(cy + y));
			set_pixel(ren, (int)(cx + y), (int)(cy - x));
		}

		if (y != 0)
		{
			set_pixel(ren, (int)(cx + x), (int)(cy - y));
			set_pixel(ren, (int)(cx - y), (int)(cy + x));
		}

		if (x != 0 && y != 0)
		{
			set_pixel(ren, (int)(cx - x), (int)(cy - y));
			set_pixel(ren, (int)(cx - y), (int)(cy - x));
		}

		error += y;
		++y;
		error += y;

		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}

t_convexPoly* create_star_shape(int nb_branch, float r1, float r2)
{
	t_convexPoly* poly = malloc(nb_branch * sizeof(t_convexPoly));

	
	t_point p1;
	p1.x = 0;
	p1.y = r1;
	p1 = rotate_vector(p1, M_PI / nb_branch);

	t_point p2;
	p2.x = 0;
	p2.y = r2;

	for (int i = 0; i < nb_branch; ++i)
	{
		poly[i].nb_point = 4;
		poly[i].point = malloc(poly[i].nb_point * sizeof(t_point));

		poly[i].point[0] = null_vector();

		poly[i].point[1] = rotate_vector(p2, i * 2 * M_PI / nb_branch);
		poly[i].point[2] = rotate_vector(p1, i * 2 * M_PI / nb_branch);
		poly[i].point[3] = rotate_vector(p2, i * 2 * M_PI / nb_branch + (2 * M_PI / nb_branch));

	}
	return poly;
}

void draw_aabb(SDL_Renderer* renderer, t_aabb* aabb)
{
	SDL_RenderDrawLine(renderer, aabb->a.x, aabb->a.y, aabb->b.x, aabb->a.y);
	SDL_RenderDrawLine(renderer, aabb->a.x, aabb->a.y, aabb->a.x, aabb->b.y);
	SDL_RenderDrawLine(renderer, aabb->b.x, aabb->b.y, aabb->a.x, aabb->b.y);
	SDL_RenderDrawLine(renderer, aabb->b.x, aabb->b.y, aabb->b.x, aabb->a.y);
}
