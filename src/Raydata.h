#ifndef RAYDATA_H
#define RAYDATA_H

#include "Types.h"
#include "RayIntersect.h"
#include "Reflection.h"
#include "Transmission.h"
#include "main.h"
#include "Bumpmapping.h"
#include "ShadowFeeler.h"

INTERSECTION* ReturnIntersectPoint(RAY);
void ProcessIntersect(INTERSECTION*, int);
COLOUR CalculateIntensity(RAY, INTERSECTION*);
void ClearTree(INTERSECTION*);

void ColourIntersectTree(RAY, INTERSECTION*);
COLOUR PhongShadeIntersect(RAY, INTERSECTION);

COLOUR CalculateRayTreeIntensity(INTERSECTION*);


#endif