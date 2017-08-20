#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "Types.h"
#include "VectorLibrary.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

FILE* SearchFileForString(const char*, int, FILE*);
MESH LoadMeshFromFile(char*, char*);
int LoadHMP(char*, HEIGHTMAP*);
AXISALIGNEDBOUNDBOX CalculateAxisAlignedBoundingBox(const MESH*);

#endif