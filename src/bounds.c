
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bounds.h"

#define EPS 0.0000001

// make array of polygon's edges (just for convenience)
// edges[0] is array of first points and edges[1] is array of second points.
void make_edges(Polygon *poly, Point *edges[2]) {
  edges[0] = malloc(poly->num_vertices * sizeof(Point));
  assert(edges[0]);
  edges[1] = malloc(poly->num_vertices * sizeof(Point));
  assert(edges[1]);
  unsigned i;
  for (i = 0; i < poly->num_vertices - 1; ++i) {
    edges[0][i] = poly->vertices[i];
    edges[1][i] = poly->vertices[i + 1];
  }
  edges[0][i] = poly->vertices[i];
  edges[1][i] = poly->vertices[0];
}

// free array of edges
void free_edges(Point *edges[2]) {
  free(edges[0]);
  free(edges[1]);
}

// distance from p1 to p2
double distance(Point *p1, Point *p2) {
  return sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));
}

// Is point on the line segment?
// Adopted from:
// https://www.cdn.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool point_on_line(Point *p, Point *p1, Point *p2) {
  if (p->x <= fmax(p1->x, p2->x) && p->x >= fmin(p1->x, p2->x) &&
      p->y <= fmax(p1->y, p2->y) && p->y >= fmin(p1->y, p2->y))
    return true;
  return false;
}

// Direction of q relateive to pr
// Adopted from:
// https://www.cdn.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
int direction(Point *p, Point *q, Point *r) {
  double t = (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
  if (t <= EPS)
    return 0; // colinear

  if (t < 0)
    return -1; // anti-clockwise direction

  // if (t > 0)
  return 1; // clockwise direction
}

// Is two line segments intersect?
// Adopted from:
// https://www.cdn.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool lines_intersect(Point *p1, Point *q1, Point *p2, Point *q2) {
  // four direction for two lines and points of other line
  int dir1 = direction(p1, q1, p2);
  int dir2 = direction(p1, q1, q2);
  int dir3 = direction(p2, q2, p1);
  int dir4 = direction(p2, q2, q1);

  if (dir1 != dir2 && dir3 != dir4)
    return true; // they are intersecting

  if (dir1 == 0 && point_on_line(p2, p1, q1))
    return true;

  if (dir2 == 0 && point_on_line(q2, p1, q1))
    return true;

  if (dir3 == 0 && point_on_line(p1, p2, q2))
    return true;

  if (dir4 == 0 && point_on_line(q1, p2, q2))
    return true;

  return false;
}

// Returns centroid of the polygon
// Adopted from:
// https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
Point centroid(Polygon *poly) {
  Point c = {0, 0};
  double sa = 0.0; // Signed area
  double x0 = 0.0; // Current vertex x
  double y0 = 0.0; // Current vertex y
  double x1 = 0.0; // Next vertex x
  double y1 = 0.0; // Next vertex y
  double a = 0.0;  // Partial signed area

  int lastidx = poly->num_vertices - 1;
  const Point *prev = &(poly->vertices[lastidx]);
  const Point *next;

  // For all vertices in a loop
  for (unsigned int i = 0; i < poly->num_vertices; ++i) {
    next = &(poly->vertices[i]);
    x0 = prev->x;
    y0 = prev->y;
    x1 = next->x;
    y1 = next->y;
    a = x0 * y1 - x1 * y0;
    sa += a;
    c.x += (x0 + x1) * a;
    c.y += (y0 + y1) * a;
    prev = next;
  }

  sa *= 0.5;
  c.x /= (6.0 * sa);
  c.y /= (6.0 * sa);

  return c;
}

// Is line segment p1-p2 intersect circle.
// Returns number of intersection and intersection points int1 and int 2
// Adopted from:
// http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/
int line_circle_intersect(Point *p1, Point *p2, Circle *circle, Point *int1,
                          Point *int2) {
  double dx, dy, A, B, C, det, t;

  dx = p2->x - p1->x;
  dy = p2->y - p1->y;

  A = dx * dx + dy * dy;
  B = 2 * (dx * (p1->x - circle->center.x) + dy * (p1->y - circle->center.y));
  C = pow(p1->x - circle->center.x, 2) + pow(p1->y - circle->center.y, 2) -
      pow(circle->radius, 2);

  det = B * B - 4 * A * C;
  if ((A <= EPS) || (det < 0)) {
    // No solutions.
    return 0;
  } else if (det == 0) {
    // One solution.
    t = -B / (2 * A);
    int1->x = p1->x + t * dx;
    int1->y = p1->y + t * dy;
    return 1;
  } else {
    // Two solutions.
    t = ((-B + sqrt(det)) / (2.0 * A));
    int1->x = p1->x + t * dx;
    int1->y = p1->y + t * dy;

    t = ((-B - sqrt(det)) / (2.0 * A));
    int2->x = p1->x + t * dx;
    int2->y = p1->y + t * dy;
    return 2;
  }
}

// Is point inside a polygon?
// Adopted from:
// https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
bool point_contained_by_poly(Point *p, Polygon *poly) {
  // Create a point for line segment from p to infinite
  Point extreme = {INFINITY, p->y};
  Point *edges[2];
  make_edges(poly, edges);
  int count = 0;
  for (unsigned i = 0; i < poly->num_vertices; ++i) {
    if (lines_intersect(&edges[0][i], &edges[1][i], p, &extreme)) {
      if (direction(p, &edges[0][i], &edges[1][i]) == 0) {
        bool result = point_on_line(p, &edges[0][i], &edges[1][i]);
        free_edges(edges);
        return result;
      }
      count++;
    }
  }
  free_edges(edges);
  // Return true if count is odd, false otherwise
  return count % 2 == 1;
}

// Is point inside a circle?
bool point_contained_by_circle(Point *p, Circle *circle) {
  return distance(p, &circle->center) <= circle->radius;
}

bool circle_contained_by_circle(Circle *circle1, Circle *circle2) {
  double d = distance(&circle1->center, &circle2->center);
  return d <= circle2->radius - circle1->radius;
}

bool poly_contained_by_circle(Polygon *poly, Circle *circle) {
  // true if  all vertices of the poly are inside the circle.
  // i.e. at a distance from the center of the circle
  // less than or equal to the radius.
  for (unsigned i = 0; i < poly->num_vertices; ++i) {
    if (!point_contained_by_circle(&poly->vertices[i], circle)) {
      return false;
    }
  }
  return true;
}

bool circle_contained_by_poly(Circle *circle, Polygon *poly) {

  Point int1, int2;
  Point *edges[2];
  make_edges(poly, edges);
  Point c = centroid(poly);

  // case 1: there are two intersections on any edge.
  for (unsigned i = 0; i < poly->num_vertices; ++i) {
    if (line_circle_intersect(&edges[0][i], &edges[1][i], circle, &int1,
                              &int2) == 2) {
      free_edges(edges);
      return false;
    }
  }

  // case 2:  two edges each have one intersection.
  int n = 0;
  for (unsigned i = 0; i < poly->num_vertices && n < 2; ++i) {
    if (line_circle_intersect(&edges[0][i], &edges[1][i], circle, &int1,
                              &int2) == 1) {
      ++n;
    }
  }
  if (n >= 2) {
    free_edges(edges);
    return false;
  }

  // case 3: one edge has a single intersection and the center of the circle
  // is further from the centroid of the triangle than the intersection is.
  if (n == 1) {
    if (distance(&c, &circle->center) > distance(&c, &int1)) {
      free_edges(edges);
      return false;
    }
  }

  // case 4: one edge has a single intersection and
  // the center of the circle is closer to the centroid
  // of the triangle than the intersection is.
  if (n == 1) {
    Point c = centroid(poly);
    if (distance(&c, &circle->center) < distance(&c, &int1)) {
      free_edges(edges);
      return true;
    }
  }

  // case 6: no intersections and center of circle is further from
  // centroid of triangle than the edge closest to the center of the circle.
  if (n == 0) {
    for (unsigned i = 0; i < poly->num_vertices; ++i) {
      if (lines_intersect(&edges[0][i], &edges[1][i], &circle->center, &c)) {
        free_edges(edges);
        return false;
      }
    }
  }

  // case 7: all vertices of the triangle are inside the circle.
  // i.e.at a distance from the center of the circle less than or equal to the
  // radius.
  if (poly_contained_by_circle(poly, circle)) {
    free_edges(edges);
    return false;
  }

  // case 5: Prove by showing all the negative cases do not hold.
  free_edges(edges);
  return true;
}

bool poly_contained_by_poly(Polygon *poly1, Polygon *poly2) {
  // true if all vertices of poly1 are inside poly2
  for (unsigned i = 0; i < poly1->num_vertices; ++i) {
    if (!point_contained_by_poly(&poly1->vertices[i], poly2)) {
      return false;
    }
  }
  return true;
}

/*
 * Determine if one shape is contained by another where "contained"
 * means entirely within the bounds of the containing shape.
 *
 * Return TRUE if INNER is contained within OUTER, FALSE otherwise.
 */
bool contained_by(Shape *inner, Shape *outer) {
  // assert(inner && outer);
  if (inner->type == CIRCLE) {
    if (outer->type == CIRCLE) {
      return circle_contained_by_circle((Circle *)inner, (Circle *)outer);
    } else if (outer->type == POLYGON) {
      return circle_contained_by_poly((Circle *)inner, (Polygon *)outer);
    } else {
      return false;
    }
  } else if (inner->type == POLYGON) {
    if (outer->type == CIRCLE) {
      return poly_contained_by_circle((Polygon *)inner, (Circle *)outer);
    } else if (outer->type == POLYGON) {
      return poly_contained_by_poly((Polygon *)inner, (Polygon *)outer);
    } else {
      return false;
    }
  } else {
    return false;
  }
  return false;
}
