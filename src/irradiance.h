#ifndef IRRADIANCE_H
#define IRRADIANCE_H

#include "Types.h"
#include "main.h"
#include "PhotonMapping.h"

IRRADIANCE_MAP CompleteIrradianceMap(int, int);
COLOUR ReturnIrradianceMapColour(INTERSECTION*, IRRADIANCE_MAP*);

#endif