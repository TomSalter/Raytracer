#ifndef VECTORLIB_C
#define VECTORLIB_C

#define ASM 1

#include "VectorLibrary.h"

float sqr(float num)
{
	return num*num;
}

VECTOR3 CrossProductVector3(VECTOR3 Vector1, VECTOR3 Vector2)
{
	VECTOR3 Result;

	Result.i = (Vector1.j * Vector2.k) - (Vector2.j * Vector1.k);
	Result.j = -((Vector1.i * Vector2.k) - (Vector2.i * Vector1.k));
	Result.k = (Vector1.i * Vector2.j) - (Vector2.i * Vector1.j);
	Result.w = 1;

	return Result;
}

float FindLengthVector3(VECTOR3 Vector)
{
#if ASM
	float length;
	float* pointer = &Vector.i;
	float* length_ptr = &length;

	__asm mov esi, pointer
	__asm fld dword ptr [esi+0]
	__asm fmul dword ptr [esi+0]
	__asm fld dword ptr [esi+4]
	__asm fmul dword ptr [esi+4]
	__asm fld dword ptr [esi+8]
	__asm fmul dword ptr [esi+8]
	__asm fadd st(0), st(1)
	__asm fadd st(0), st(2)
	__asm fsqrt
	__asm mov esi, length_ptr
	__asm fstp dword ptr [esi+0]
	__asm emms

	return length;
#else
	float Magnitude = (float)sqrt(sqr(Vector.i) + sqr(Vector.j) + sqr(Vector.k));
	if(Magnitude < 0.0)
	{
		return -Magnitude;
	}
	return Magnitude;
#endif
}

void NormaliseVector3(VECTOR3 *Vector)
{
	float Magnitude = FindLengthVector3(*Vector);
	Vector->i = Vector->i / Magnitude;
	Vector->j = Vector->j / Magnitude;
	Vector->k = Vector->k / Magnitude;
	Vector->w = Vector->w / Magnitude;
}

float DotProductVector3(VECTOR3 Vector1, VECTOR3 Vector2)
{
	float DotProd;

	DotProd = (	(Vector1.i * Vector2.i) + 
				(Vector1.j * Vector2.j) +
				(Vector1.k * Vector2.k));

	return DotProd;
}

VECTOR3 newVector3(float i, float j, float k)
{
	VECTOR3 vect;

	vect.i = i;
	vect.j = j;
	vect.k = k;
	vect.w = 1.0;

	return vect;
}

COLOUR newColour(float r, float g, float b, float a)
{
	COLOUR col;

	col.r = r;
	col.g = g;
	col.b = b;
	col.alpha = a;

	return col;
}
	
float floatabs(float num)
{
	if(num < 0)
		return -num;
	
	return num;
}

VECTOR3 SubtractVector3(VECTOR3 v1, VECTOR3 v2)
{
	VECTOR3 retvect;

	retvect.i = v1.i - v2.i;
	retvect.j = v1.j - v2.j;
	retvect.k = v1.k - v2.k;
	retvect.w = 1.0;

	return retvect;
}

float CalculateTriangleArea(VECTOR3 v0, VECTOR3 v1, VECTOR3 v2)
{
	float a, b, c;
	float area;
	float s;
	float temp;

	a = FindLengthVector3(v0);
	b = FindLengthVector3(v1);
	c = FindLengthVector3(v2);

	s = (a + b + c) / 2;

	area = sqrt( s * (s - a) * (s - b) * (s - c) );

	temp = floatabs((a + b + c) * (b + c - a) * (c + a - b) * (a + b - c));

	area = 0.25 * (sqrt( temp ) );
	
	return area;

}

void saturate(float *v)
{
	if(*v < 0.0)
		*v = 0.0;
	else if(*v > 1.0)
		*v = 1.0;
}

void NegateVector3(VECTOR3* v1)
{
	v1->i = -v1->i;
	v1->j = -v1->j;
	v1->k = -v1->k;
	v1->w = -v1->w;
}

VECTOR3 SphericalInterpolateVector3(VECTOR3 v1, VECTOR3 v2, float t)
{
	float theta, t1, t2;
	VECTOR3 d;

	NormaliseVector3(&v1);
	NormaliseVector3(&v2);

	theta = acos(DotProductVector3(v1, v2));

	if(theta == 0)
	{
		return v1;
	}
	else
	{
		t1 = (sin(1 - t) * theta) / sin(theta);
		t2 = (sin(t) * theta) / sin(theta);
	}

	d.i = t1 * v1.i + t2 * v2.i;
	d.j = t1 * v1.j + t2 * v2.j;
	d.k = t1 * v1.k + t2 * v2.k;

	d.w = 1;

	return d;	
}


#endif