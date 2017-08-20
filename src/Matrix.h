#ifndef MATRIX_H
#define MATRIX_H

#include "Types.h"

MATRIX4x4 MatrixMultiply4(MATRIX4x4, MATRIX4x4);
VECTOR3 MultiplyVectorWithMatrix4(VECTOR3, MATRIX4x4);
void newMatrix4x4(MATRIX4x4*);

#endif