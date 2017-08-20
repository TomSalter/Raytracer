#ifndef ERROR_H
#define ERROR_H

#include "main.h"
#include "stdio.h"
#include "stdlib.h"

// Define error codes here

#define WORLD_OPEN_FAILED 70001
#define MESH_MALLOC_FAILED 70002
#define RAY_INTERSECT_ERROR 70003
#define IRRADIANCE_COORDS_OUT_OF_RANGE 70004
#define IRRADIANCE_MAP_OUT_OF_MEMORY 70005

void ReportError(int, char*);

#endif