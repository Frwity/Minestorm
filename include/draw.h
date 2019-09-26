#ifndef __DRAW_H__
#define __DRAW_H__

void set_pixel(SDL_Renderer* renderer, int x, int y);

void draw_circle(SDL_Renderer *ren, t_circle circle);

t_convexPoly* create_star_shape(int nb_branch, float r1, float r2);

void draw_aabb(SDL_Renderer* renderer, t_aabb* aabb);

#endif
