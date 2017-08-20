#ifndef REFLECTION_C
#define REFLECTION_C

#include "Reflection.h"

RAY CalculateReflectedRay(RAY ray, const INTERSECTION *isect)
{
	float NdotL;
	VECTOR3 Lvect;
	RAY reflectray;
	VECTOR3 normal;

	// If we are in an object, use the negative version of the normal given to us the correct refraction vector
	if(ray.InObject == 1)
	{
		normal.i = -isect->normal.i;
		normal.j = -isect->normal.j;
		normal.k = -isect->normal.k;
	}
	else
	{
		normal.i = isect->normal.i;
		normal.j = isect->normal.j;
		normal.k = isect->normal.k;
	}

	Lvect = SubtractVector3(ray.origin, isect->position);
	NormaliseVector3(&Lvect);

	NdotL = DotProductVector3(normal, Lvect);

	reflectray.vector.i =  2 * NdotL * normal.i - Lvect.i;
	reflectray.vector.j = 2 * NdotL * normal.j - Lvect.j;
	reflectray.vector.k = 2 * NdotL * normal.k - Lvect.k;

	reflectray.origin = isect->position;


	// Refractive Index is the same as the current ray, as it's a reflected ray
	reflectray.current_refractive_index = ray.current_refractive_index;

	// Reflected rays always stay in the same object as the incoming ray
	reflectray.InObject = ray.InObject;

	return reflectray;
}

VECTOR3 CalculateReflectionVector(VECTOR3 normal, VECTOR3 lvect)
{
	VECTOR3 reflectvect;
	float coefficient;

	coefficient = 2 * DotProductVector3(normal, lvect);

	reflectvect.i = (coefficient * normal.i) - lvect.i;
	reflectvect.j = (coefficient * normal.j) - lvect.j;
	reflectvect.k = (coefficient * normal.k) - lvect.k;

	reflectvect.w = 1.0;

	return reflectvect;
}



#endif