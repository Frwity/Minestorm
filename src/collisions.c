#include <math.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "2d_tools.h"
#include "collisions.h"

bool line_intersections(t_line line1, t_line line2)
{

	return false;
}

bool line_segment_intersection(t_line line, t_segment seg)
{

	return false;
}

bool circle_intersections(t_circle circle1, t_circle circle2)
{
	if (vector_length(sub_vector(circle1.center, circle2.center)) < circle1.r + circle2.r)
		return true;
	return false;
}

bool poly_point_intersection(t_convexPoly poly, t_point point)
{
    t_vector2 normal;
    int k = 0;
    t_range r1;
    t_range r2;
    for (int i = 0; i < poly.nb_point; i++)
    {
        if (i == poly.nb_point - 1)
            k = 0;
        else
            k = i + 1;

        normal = vector_normal(sub_vector(poly.point[i], poly.point[k]));

        r1.min = dot_product(point, normal);
        r1.max = dot_product(point, normal);

        r2.min = dot_product(poly.point[0], normal);
        r2.max = dot_product(poly.point[0], normal);

		for (int j = 1; j < poly.nb_point; j++)
            set_range(&r2, dot_product(poly.point[j], normal));

		if (!range_intersect(r1, r2))
            return false;
    }
	return true;
}

bool poly_intersections(t_convexPoly poly1, t_convexPoly poly2)
{
    for (int i = 0; i < poly2.nb_point; i++)
        if (poly_point_intersection(poly1, poly2.point[i]))
            return true;

    for (int i = 0; i < poly1.nb_point; i++)
	{
        if (poly_point_intersection(poly2, poly1.point[i]))
            return true;
	}
	return false;
}

bool poly_circle_intersection(t_convexPoly poly, t_circle circle)
{
    t_vector2 normal;
    int k = 0;
    t_range r1;
    t_range r2;
    t_point* closest;
	float dist_closest = 10000;

    for (int i = 0; i < poly.nb_point; i++)
    {
        if (i == poly.nb_point - 1)
            k = 0;
        else
            k = i + 1;

        normal = vector_normal(sub_vector(poly.point[i], poly.point[k]));

    	r1.min = dot_product(circle.center, normal);
    	r1.max = dot_product(circle.center, normal);
        r1.min -= circle.r;
        r1.max += circle.r;

        r2.min = dot_product(poly.point[0], normal);
        r2.max = dot_product(poly.point[0], normal);

        for (int j = 1; j < poly.nb_point; j++)
            set_range(&r2, dot_product(poly.point[j], normal));
        
        if (!range_intersect(r1, r2))
			break;
        
	    if (distance_between_point(poly.point[i], circle.center) < dist_closest)
     	{
   		  	dist_closest = distance_between_point(poly.point[i], circle.center);
   		  	closest = &poly.point[i];
		}
    }
///////////////////////////////////////////
    
    normal = vector_normal(sub_vector(*closest, circle.center));

   	r1.min = dot_product(circle.center, normal);
   	r1.max = dot_product(circle.center, normal);
    r1.min -= circle.r;
    r1.max += circle.r;

    r2.min = dot_product(poly.point[0], normal);
    r2.max = dot_product(poly.point[0], normal);
    for (int j = 0; j < poly.nb_point; j++)
	{
		set_range(&r2, dot_product(poly.point[j], normal));
	}
    
    return range_intersect(r1, r2);
}

void init_aabb(t_aabb* aabb, t_point point)
{
	aabb->a.x = point.x;
	aabb->a.y = point.y;
	aabb->b.x = point.x;
	aabb->b.y = point.y;
}

void update_aabb(t_aabb* aabb, t_point* point, int nb_point)
{
	for (int i = 0; i < nb_point; ++i)
	{
		if (point[i].x < aabb->a.x)
			aabb->a.x = point[i].x;
		if (point[i].y < aabb->a.y)
			aabb->a.y = point[i].y;

		if (point[i].x > aabb->b.x)
			aabb->b.x = point[i].x;
		if (point[i].y > aabb->b.y)
			aabb->b.y = point[i].y;
	}
}

bool hitbox_aabb(t_aabb aabb1, t_aabb aabb2)
{
	if ((aabb1.a.x > aabb2.a.x && aabb1.a.x < aabb2.b.x && aabb1.a.y > aabb2.a.y && aabb1.a.y < aabb2.b.y)
	||  (aabb1.b.x > aabb2.a.x && aabb1.b.x < aabb2.b.x && aabb1.b.y > aabb2.a.y && aabb1.b.y < aabb2.b.y)
	||  (aabb1.a.x > aabb2.a.x && aabb1.a.x < aabb2.b.x && aabb1.b.y > aabb2.a.y && aabb1.b.y < aabb2.b.y)
	||  (aabb1.b.x > aabb2.a.x && aabb1.b.x < aabb2.b.x && aabb1.a.y > aabb2.a.y && aabb1.a.y < aabb2.b.y)
	||  (aabb2.a.x > aabb1.a.x && aabb2.a.x < aabb1.b.x && aabb2.a.y > aabb1.a.y && aabb2.a.y < aabb1.b.y)
	||  (aabb2.b.x > aabb1.a.x && aabb2.b.x < aabb1.b.x && aabb2.b.y > aabb1.a.y && aabb2.b.y < aabb1.b.y)
	||  (aabb2.a.x > aabb1.a.x && aabb2.a.x < aabb1.b.x && aabb2.b.y > aabb1.a.y && aabb2.b.y < aabb1.b.y)
	||  (aabb2.b.x > aabb1.a.x && aabb2.b.x < aabb1.b.x && aabb2.a.y > aabb1.a.y && aabb2.a.y < aabb1.b.y))
	{
		return true;
	}
	return false;
}
