#ifndef PHOTONMAPPING_H
#define PHOTONMAPPING_H

#include "Types.h"
#include "main.h"

PHOTON ShootPhotonFromAreaLight(AREA_LIGHT);
void FollowPhotonPath(PHOTON);
COLOUR ReturnPhotonMapColour(RAY, INTERSECTION*);
COLOUR ReturnPhotonMapColourFinalGathering(RAY, INTERSECTION *isect);
COLOUR ReturnPhotonMapColourDrawPhotons(RAY, INTERSECTION*);
VECTOR3 ReturnDiffuseVector(VECTOR3);

void FollowPhotonPath2(PHOTON);
#endif

