#ifndef __2D_TOOLS_H__
#define __2D_TOOLS_H__

#include <SDL2/SDL.h>

typedef struct s_vector2
{
    float x;
    float y;

} t_vector2;

typedef struct s_vector2 t_point;

typedef struct s_ref 
{
	t_point center;
	t_vector2 i;
	t_vector2 j;
	float angle;
	float scale;

} t_ref;

typedef struct s_line
{
    t_point a;
    t_vector2 dir;

} t_line;

typedef struct s_segment
{
    t_point a;
    t_point b;

} t_segment; 

typedef struct s_circle
{
    t_point center;
    float r;

} t_circle;

typedef struct s_rect
{
    t_point center;
    float hwidth;
    float hheigth;

} t_rect;

typedef struct s_orientedRect
{
    t_rect rect;
    float angle;

} t_orientedRect;

typedef struct s_aabb
{
	t_point a;
	t_point b;

} t_aabb;


typedef struct s_convexPoly
{
    t_point* point;
	t_aabb aabb;
    int nb_point;

} t_convexPoly;

typedef struct s_range
{
    float min;
    float max;

} t_range;

t_vector2 null_vector();

t_vector2 unit_vector(t_vector2 vector);

bool is_vector_equal(t_vector2 vector1, t_vector2 vector2);

float vector_length(t_vector2 vector);

float vector_square_length(t_vector2 vector);

float distance_between_point(t_point p1, t_point p2);

t_vector2 add_vector(t_vector2 vector1, t_vector2 vector2);

t_vector2 sub_vector(t_vector2 vector1, t_vector2 vector2);

t_vector2 mult_vector(t_vector2 vector1, t_vector2 vector2);

t_vector2 negate_vector(t_vector2 vector);

t_vector2 rotate_vector(t_vector2 vector, float angle);

t_point rotate_point(t_point point, t_point center, float angle);

t_vector2 vector_normal(t_vector2 vector);

float dot_product(t_vector2 vector1, t_vector2 vector2);

float angle_between_vector(t_vector2 vector1, t_vector2 vector2);

t_vector2 global_to_local_vect(t_vector2 vector, t_ref ref);

t_point global_to_local_point(t_point point, t_ref ref);

t_point local_to_global_vect(t_vector2 vector, t_ref ref);

t_point local_to_global_point(t_point point, t_ref ref);

t_vector2 scale_vector(t_vector2 vector, float scale);

void init_ref(t_ref* ref);

void ref_update(t_ref* ref);

t_convexPoly* local_to_global_poly(t_convexPoly poly, t_ref ref);

void init_range(t_range* range);

void set_range(t_range* range, float nb);

bool range_intersect(t_range range1, t_range range2);

void draw_aabb(SDL_Renderer* renderer, t_aabb* aabb);

#endif
