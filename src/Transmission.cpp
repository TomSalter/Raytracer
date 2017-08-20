#ifndef TRANSMISSION_C
#define TRANSMISSION_C

#include "Transmission.h"
#include "Bumpmapping.h"

RAY CalculateRefractedRay(RAY ray, const INTERSECTION *isect, float n2)
{
	float coefficient1, coefficient2, coefficient3;
	float NdotL;
	VECTOR3 Lvect;
	VECTOR3 normal;
	RAY transray;

	Lvect = SubtractVector3(ray.origin, isect->position);
	NormaliseVector3(&Lvect);

	normal = isect->normal;

	// If we are in an object, use the negative version of the normal given to us the correct refraction vector
	if(ray.InObject == 1)
	{
		normal.i = -normal.i;
		normal.j = -normal.j;
		normal.k = -normal.k;
	}

	NdotL = DotProductVector3(normal, Lvect);

	coefficient3 = (ray.current_refractive_index / n2);
	coefficient1 = coefficient3 * NdotL;
	coefficient2 = (float)(1 - (sqr(ray.current_refractive_index) / sqr(n2)) * (1 - sqr(NdotL)));

	// Take into account Total Internal Reflection
	if(coefficient2 <= 0.0)
	{
		transray = CalculateReflectedRay(ray, isect);
		if( ray.InObject == 0)
			transray.InObject = 1;
		else
			transray.InObject = 0;

		transray.current_refractive_index = n2;
		transray.t = 0;

		return transray;
	}

	coefficient2 = (float)sqrt(coefficient2);


	transray.origin = isect->position;

	transray.vector.i = (coefficient1 - coefficient2) * normal.i - coefficient3 * Lvect.i;
	transray.vector.j = (coefficient1 - coefficient2) * normal.j - coefficient3 * Lvect.j;
	transray.vector.k = (coefficient1 - coefficient2) * normal.k - coefficient3 * Lvect.k;

	transray.current_refractive_index = n2;

	transray.t = 0;

	// Transmitted rays *always* switch between InObject and OutObject at each intersection
	if( ray.InObject == 0)
		transray.InObject = 1;
	else
		transray.InObject = 0;

	return transray;
}

#endif