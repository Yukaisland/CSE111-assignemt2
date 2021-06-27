/*
 * Copyright (C) 2018-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 */

/*
#######################################################################
#
# Do not modify this file. 
#
# If you modify it and your code relies on those modifications, your code
# will not compile in the automated test harness and will be unable to
# execute any tests.
#
#######################################################################
*/

#ifndef _BOUNDS_H_
#define _BOUNDS_H_

typedef struct point_t {
  double x;
  double y;
} 
Point;

enum shape {
  CIRCLE,
  POLYGON
};

#define TYPE enum shape type

typedef struct shape_t {
  TYPE;
}
Shape;

typedef struct circle_t {
  TYPE;
  Point center;
  double radius;
} 
Circle;

typedef struct polygon_t {
  TYPE;
  unsigned int num_vertices;
  Point *vertices; // clockwise
} 
Polygon;

#endif