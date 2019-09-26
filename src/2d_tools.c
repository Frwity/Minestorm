#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "2d_tools.h"

#define EPSI 30

t_vector2 null_vector()
{
    t_vector2 vector;
    vector.x = 0;
    vector.y = 0;
    return vector;
}

t_vector2 unit_vector(t_vector2 vector)
{
    float power = vector_length(vector);
	vector.x /= power;
	vector.y /= power;
	return vector;
}

bool is_vector_equal(t_vector2 vector1, t_vector2 vector2)
{
	if ((vector1.x - EPSI <= vector2.x && vector1.x + EPSI >= vector2.x)
	&&  (vector1.y - EPSI <= vector2.y && vector1.y + EPSI >= vector2.y))
		return true;
	return false;
}

float vector_length(t_vector2 vector)
{
    return sqrt(pow(vector.x, 2) + pow(vector.y, 2));
}

float vector_square_length(t_vector2 vector)
{
    return pow(vector.x, 2) + pow(vector.y, 2);
}

float distance_between_point(t_point p1, t_point p2)
{
    return abs(vector_length(p1) - vector_length(p2));
}

t_vector2 add_vector(t_vector2 vector1, t_vector2 vector2)
{
	t_vector2 v;
    v.x = vector1.x + vector2.x;
    v.y = vector1.y + vector2.y;
    return v;
}

t_vector2 sub_vector(t_vector2 vector1, t_vector2 vector2)
{
	t_vector2 v;
    v.x = vector1.x - vector2.x;
    v.y = vector1.y - vector2.y;
    return v;
}

t_vector2 mult_vector(t_vector2 vector1, t_vector2 vector2)
{
    vector1.x *= vector2.x;
    vector1.y *= vector2.y;
    return vector1;
}

t_vector2 negate_vector(t_vector2 vector)
{
    vector.x *= -1;
    vector.y *= -1;
    return vector;
}

t_vector2 rotate_vector(t_vector2 vector, float angle)
{
	if (angle == 0)
		return vector;
	t_vector2 v;
    v.x = vector.x * cos(angle) - vector.y * sin(angle);
    v.y = vector.x * sin(angle) + vector.y * cos(angle);
	return v;
}

t_point rotate_point(t_point point, t_point center, float angle)
{
	t_point p;
	p = rotate_vector(point, angle);
	return p;
}

t_vector2 vector_normal(t_vector2 vector)
{
    t_vector2 v;
	v.x = -vector.y;
	v.y = vector.x;
	return v;
}

float dot_product(t_vector2 vector1, t_vector2 vector2)
{
    return vector1.x * vector2.x + vector1.y * vector2.y;
}

float angle_between_vector(t_vector2 vector1, t_vector2 vector2)
{
	return acos(dot_product(vector1, vector2) / (vector_length(vector1) * vector_length(vector2)));
}

t_vector2 scale_vector(t_vector2 vector, float scale)
{
	t_vector2 v;
	v.x = vector.x * scale;
	v.y = vector.y * scale;
	return v;
}

void init_ref(t_ref* ref)
{
	ref->i.x = 1;
	ref->i.y = 0;
	ref->j.x = 0;
	ref->j.y = 1;
	ref->angle = 0;
}

void ref_update(t_ref* ref)
{
	t_ref r;
	init_ref(&r);
	ref->i = rotate_vector(r.i, ref->angle);
	ref->j = rotate_vector(r.j, ref->angle);
	(void)r;
}

t_vector2 global_to_local_vect(t_vector2 vector, t_ref ref)
{
	return rotate_vector(vector, -ref.angle);
}

t_point global_to_local_point(t_point point, t_ref ref)
{
	point = rotate_point(point, ref.center, -ref.angle);
	point = add_vector(point, ref.center);
	return point;
}

t_point local_to_global_vect(t_vector2 vector, t_ref ref)
{
	return rotate_vector(vector, ref.angle);
}

t_point local_to_global_point(t_point point, t_ref ref)
{
	point = rotate_point(point, ref.center, ref.angle);
	point = add_vector(point, ref.center);
	return point;
}

t_convexPoly* local_to_global_poly(t_convexPoly poly, t_ref ref)
{
	t_convexPoly* p = malloc(sizeof(t_convexPoly));

	p->point = malloc(poly.nb_point * sizeof(t_point));
	p->nb_point = poly.nb_point;

	for (int i = 0; i < poly.nb_point; ++i)
	{
		p->point[i] = scale_vector(poly.point[i], ref.scale);
		p->point[i] = local_to_global_point(p->point[i], ref);
	}
	return p;
}

void init_range(t_range* range)
{
    range->min = 0;
    range->max = 0;
}

void set_range(t_range* range, float nb)
{
    if (nb < range->min)
        range->min = nb;
    if (nb > range->max)
		range->max = nb;
}

bool range_intersect(t_range range1, t_range range2)
{
    if (((range1.max > range2.min && range1.max < range2.max) || (range1.min > range2.min && range1.min < range2.max))
    ||  ((range2.max > range1.min && range2.max < range1.max) || (range2.min > range1.min && range2.min < range1.max)))
        return true;
    return false;
}
