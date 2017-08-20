#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "Types.h"
#include <math.h>
#include "VectorLibrary.h"

RAY CalculateRefractedRay(RAY, const INTERSECTION*, float);

#endif