// Header file for RayIntersect.cpp
// Contains all the function prototypes

#ifndef RAYINTERSECT_H
#define RAYINTERSECT_H

#include "VectorLibrary.h"
#include "Types.h"
#include <stdlib.h>
#include "Error.h"

VECTOR3 CalculateRayPlaneIntersectPoint(PLANE, RAY*);
int TriangleIntersectTestAlgebraic(const VECTOR3*, VECTOR3 point, RAY);
float CheckIfIntersection(RAY, VERTEX3, VERTEX3, VERTEX3);
INTERSECTION CalculateRayTriangleIntersection(RAY, VERTEX3, VERTEX3, VERTEX3);
int	CheckAxisAlignedBoundingBoxIntersection(AXISALIGNEDBOUNDBOX, RAY);

void SwapPoints(VECTOR3*, VECTOR3*);
void SortPointsVertAsc(VECTOR3*);
VECTOR3 InterpolateVertical(VECTOR3, VECTOR3, VECTOR3);
int TriangleIntersectTestProjected(const VECTOR3*, VECTOR3, RAY);
float CheckIfIntersectionProjected(RAY, VERTEX3, VERTEX3, VERTEX3);

INTERSECTION CalculateRayTriangleIntersectionProjected(RAY, VERTEX3, VERTEX3, VERTEX3);
INTERSECTION ReturnIntersectionProjected(const VERTEX3*, VECTOR3, RAY);

// Quick functions
float CheckIfIntersectionQuick(RAY, VERTEX3, VERTEX3, VERTEX3);
INTERSECTION CalculateRayTriangleIntersectionQuick(RAY, VERTEX3, VERTEX3, VERTEX3);

#endif