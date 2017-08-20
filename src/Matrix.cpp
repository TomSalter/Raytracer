#ifndef MATRIX_C
#define MATRIX_C

#include "Matrix.h"

void newMatrix4x4(MATRIX4x4* temp)
{
	temp->mat[0][0] = temp->mat[1][1] = temp->mat[2][2] = temp->mat[3][3] = 1.0;

	temp->mat[0][1] = temp->mat[0][2] = temp->mat[0][3] = temp->mat[1][0] =
	temp->mat[1][2] = temp->mat[1][3] = temp->mat[2][0] = temp->mat[2][1] =
	temp->mat[2][3] = temp->mat[3][0] = temp->mat[3][1] = temp->mat[3][2] = 0.0;
}

VECTOR3 MultiplyVectorWithMatrix4(VECTOR3 vect, MATRIX4x4 mat)
{
	VECTOR3 rVect;

	rVect.i =	(mat.mat[0][0] * vect.i) + (mat.mat[1][0] * vect.j) +
				(mat.mat[2][0] * vect.k) + (mat.mat[3][0] * vect.w);

	rVect.j =	(mat.mat[0][1] * vect.i) + (mat.mat[1][1] * vect.j) +
				(mat.mat[2][1] * vect.k) + (mat.mat[3][1] * vect.w);

	rVect.k =	(mat.mat[0][2] * vect.i) + (mat.mat[1][2] * vect.j) +
				(mat.mat[2][2] * vect.k) + (mat.mat[3][2] * vect.w);

	rVect.w =	(mat.mat[0][3] * vect.i) + (mat.mat[1][3] * vect.j) +
				(mat.mat[2][3] * vect.k) + (mat.mat[3][3] * vect.w);

	return rVect;
}

MATRIX4x4 MatrixMultiply4(MATRIX4x4 A, MATRIX4x4 B) 
{
	MATRIX4x4 out;
	int i,j,k;
	for(i = 0; i < 4; i++)
		for(k = 0; k < 4; k++)
			out.mat[i][k] = 0;

	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			for (k=0;k<4;k++)
				out.mat[i][j] += A.mat[i][k] * B.mat[k][j];

	return out;
}

#endif
