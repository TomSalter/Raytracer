#ifndef WORLD_H
#define WORLD_H

#include "Types.h"
#include "MeshLoader.h"
#include "Matrix.h"
#include "Error.h"
#include <stdlib.h>
#include <string.h>

void ProcessMesh(MESH*);
WORLD LoadWorldFromFile(char*);
TEXTURE* LoadTexture(const char*);

#endif