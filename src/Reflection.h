#ifndef REFLECTION_H
#define REFLECTION_H

#include "Types.h"
#include "VectorLibrary.h"

RAY CalculateReflectedRay(RAY, const INTERSECTION*);
VECTOR3 CalculateReflectionVector(VECTOR3, VECTOR3);

#endif