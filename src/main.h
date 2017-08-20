#ifndef MAIN_H
#define MAIN_H

#define WIDTH 1000
#define HEIGHT 1000
#define REFRACTIVE_INDEX_AIR 1.0
#define DEBUG 0
#define DEG2RAD 0.0174532925199

#define DISTRIBUTED_RAYTRACER_LIMIT 1

#define AREA_LIGHT_SAMPLING_RATE 25

// Number of photons to shoot into the scene per light
#define NUMBER_OF_PHOTON_SAMPLES 50000

// Distance of effect each photon has
#define PHOTON_SAMPLING_DISTANCE 0.05
#define PHOTON_SAMPLING_INCREMENT 0.1
#define PHOTON_SAMPLING_NUMBER	20
#define DRAW_PHOTONS 0

#define PHOTON_COLOUR_BLEED 1

#define PHOTON_MAPPING_RECURSIVE_LIMIT 5
#define FINAL_GATHERING_LIMIT 1024

#define IRRADIANCE_CACHE_FACTOR 0.5

// Renderer parameters
#define WHITTED_RAYTRACER_ONLY 1
#define PHOTON_MAPPING_ONLY 0
#define PHOTON_MAPPING_FINAL_GATHERING 0
#define IRRADIANCE_CACHING 0

#define RECURSIVELIMIT 5
#include "VectorLibrary.h"
#include "Camera.h"
#include "World.h"
#include "Output.h"
#include "RayData.h"
#include "World.h"
#include "Types.h"
#include "PhotonMapping.h"
#include "irradiance.h"
#include <time.h>

#endif