#ifndef BUMPMAPPING_H
#define BUMPMAPPING_H

#include "Types.h"
#include "VectorLibrary.h"

// Makes the bumpmaps more or less defined
#define BUMPMAP_FACTOR 2

VECTOR3 FindHeightMapNormal(TEXCOORDS, VECTOR3, HEIGHTMAP*);
float FindHeightValue(TEXCOORDS, HEIGHTMAP);

COLOUR ReturnColourFromTextureMap(TEXCOORDS, TEXTURE*);


#endif