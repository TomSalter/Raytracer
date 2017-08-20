#ifndef VECTORLIB_H
#define VECTORLIB_H

#include "Types.h"
#include "main.h"
#include <math.h>

VECTOR3 CrossProductVector3(VECTOR3 Vector1, VECTOR3 Vector2);
float sqr(float num);
float FindLengthVector3(VECTOR3 Vector);
void NormaliseVector3(VECTOR3 *Vector);
float DotProductVector3(VECTOR3 Vector1, VECTOR3 Vector2);
VECTOR3 newVector3(float, float, float);
COLOUR newColour(float, float, float, float);
float floatabs(float);
VECTOR3 SubtractVector3(VECTOR3, VECTOR3);
float CalculateTriangleArea(VECTOR3, VECTOR3, VECTOR3);
void saturate(float*);
void NegateVector3(VECTOR3*);
VECTOR3 SphericalInterpolateVector3(VECTOR3, VECTOR3, float);

#endif