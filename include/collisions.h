#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <stdbool.h>
#include "2d_tools.h"

bool line_intersections(t_line line1, t_line line2);

bool line_segment_intersection(t_line line, t_segment seg);

bool circle_intersections(t_circle circle1, t_circle circle2);

bool poly_point_intersection(t_convexPoly poly, t_point point);

bool poly_intersections(t_convexPoly poly1, t_convexPoly poly2);

bool poly_circle_intersection(t_convexPoly poly, t_circle circle);

void init_aabb(t_aabb* aabb, t_point point);

void update_aabb(t_aabb* aabb, t_point* point, int nb_point);

bool hitbox_aabb(t_aabb aabb1, t_aabb aabb2);

#endif
