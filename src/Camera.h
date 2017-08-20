#ifndef CAMERA_H
#define CAMERA_H

#include "Types.h"
#include "main.h"
#include "VectorLibrary.h"
#include <math.h>

void ReturnInitialRayFromCamera(CAMERA*, int, int);
void ReturnRandomInitialRayFromCamera(CAMERA*, int, int);
void ReturnJitteredRayFromCamera(CAMERA*, int, int, int, int);

#endif