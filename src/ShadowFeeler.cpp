#ifndef SHADOWFEELER_C
#define SHADOWFEELER_C

#include "ShadowFeeler.h"


extern WORLD world;

// Function takes in an area light and intersection and incoming ray, returns the colour for that intersection, taking into account shadows
COLOUR ReturnAreaLightIntensity(INTERSECTION isect, AREA_LIGHT light, RAY ray)
{
	VECTOR3 left_corner, right_corner, left_cornerdash, right_cornerdash;
	float t;
	int number_of_subunits = (int)sqrt(AREA_LIGHT_SAMPLING_RATE);
	int i, x, y;
	COLOUR colour_array[AREA_LIGHT_SAMPLING_RATE];
	COLOUR colour;
	COLOUR texcolour;

	LIGHT point_light;

	RAY light_ray;
	int mesh_intersect_index;
	int polygon_intersect_index;

	float LdotN;
	float RdotV;
	float light_t;
	float intersection_test;
	VECTOR3 Light;
	VECTOR3 View;
	VECTOR3 Normal;
	VECTOR3 Reflect;


	// Precompute things for the lighting equations...
	if(PHOTON_MAPPING_ONLY)
	{
		colour = newColour(0.0, 0.0, 0.0, 1.0);
		return colour;
	}

	View = SubtractVector3(ray.origin, isect.position);
	NormaliseVector3(&View);

//	Normal = FindHeightMapNormal(isect.tex, isect.normal, &isect.mesh_pointer->heightmap);
	Normal = isect.normal;
	NormaliseVector3(&Normal);

	int num_iterations;

	num_iterations = (int)sqr( (number_of_subunits - 1) );


	for(i = 0; i < num_iterations; i++)
	{
		t = (float)(i % (number_of_subunits - 1)) / (float)(number_of_subunits - 1);
		
		// Linear interpolate between light[0] and [3]

		left_corner.i = light.corners[0].i + (t * (light.corners[3].i - light.corners[0].i));
		left_corner.j = light.corners[0].j + (t * (light.corners[3].j - light.corners[0].j));
		left_corner.k = light.corners[0].k + (t * (light.corners[3].k - light.corners[0].k));

		t = (float)((i + 1) % number_of_subunits) / (float)(number_of_subunits - 1);

		left_cornerdash.i = light.corners[0].i + (t * (light.corners[3].i - light.corners[0].i));
		left_cornerdash.j = light.corners[0].j + (t * (light.corners[3].j - light.corners[0].j));
		left_cornerdash.k = light.corners[0].k + (t * (light.corners[3].k - light.corners[0].k));


		t = (float)(i / (number_of_subunits - 1)) / (float)(number_of_subunits - 1);

		right_corner.i = light.corners[0].i + (t * (light.corners[1].i - light.corners[0].i));
		right_corner.j = light.corners[0].j + (t * (light.corners[1].j - light.corners[0].j));
		right_corner.k = light.corners[0].k + (t * (light.corners[1].k - light.corners[0].k));

		t = (float)( (i + 1) / number_of_subunits) / (float)(number_of_subunits - 1);

		right_cornerdash.i = light.corners[0].i + (t * (light.corners[1].i - light.corners[0].i));
		right_cornerdash.j = light.corners[0].j + (t * (light.corners[1].j - light.corners[0].j));
		right_cornerdash.k = light.corners[0].k + (t * (light.corners[1].k - light.corners[0].k));

		// Now we interpolate between left_corner and right_corner to find final position, with a random value of t

		t = (float)rand()/RAND_MAX;

		left_corner.i = left_corner.i + (t * (left_cornerdash.i - left_corner.i));
		left_corner.j = left_corner.j + (t * (left_cornerdash.j - left_corner.j));
		left_corner.k = left_corner.k + (t * (left_cornerdash.k - left_corner.k));

		t = (float)rand()/RAND_MAX;

		right_corner.i = right_corner.i + (t * (right_cornerdash.i - right_corner.i));
		right_corner.j = right_corner.j + (t * (right_cornerdash.j - right_corner.j));
		right_corner.k = right_corner.k + (t * (right_cornerdash.k - right_corner.k));


		// Now we create our light with position...

		point_light.position.i = light.corners[0].i + (left_corner.i - light.corners[0].i) + (right_corner.i - light.corners[0].i);
		point_light.position.j = light.corners[0].j + (left_corner.j - light.corners[0].j) + (right_corner.j - light.corners[0].j);
		point_light.position.k = light.corners[0].k + (left_corner.k - light.corners[0].k) + (right_corner.k - light.corners[0].k);

		point_light.colour = light.colour;

		// Now we check if the intersection can see the light

		Light = SubtractVector3(point_light.position, isect.position);
		light_t = FindLengthVector3(Light);
		NormaliseVector3(&Light);

		Reflect = CalculateReflectionVector(Normal, Light);
		NormaliseVector3(&Reflect);

		RdotV = DotProductVector3(Reflect, View);

		// Check if the light can see the actual intersection

		// Form our light ray:
		light_ray.origin = isect.position;
		light_ray.vector = Light;

		LdotN = DotProductVector3(Light, Normal);

		// Clamp our vector values
		saturate(&LdotN);
		saturate(&RdotV);

		// Find if this ray intersects with anything
		mesh_intersect_index = -1;
		polygon_intersect_index = -1;

		colour_array[i] = newColour(0.0, 0.0, 0.0, 1.0);

		// Loop through all meshes
		for(x = 0; x < world.numMeshes; x++)
		{
			// Check against AABB first
			if( CheckAxisAlignedBoundingBoxIntersection( world.meshes[x].aabb, light_ray) )
			{
				// Loop through all polygons in this object
				for(y = 0; y < world.meshes[x].numPolys; y++)
				{
					// Work out the value of t for the intersection test
					intersection_test = CheckIfIntersectionQuick(light_ray, 
															world.meshes[x].vertices[world.meshes[x].polygons[y].index[0]],
															world.meshes[x].vertices[world.meshes[x].polygons[y].index[1]],
															world.meshes[x].vertices[world.meshes[x].polygons[y].index[2]]);
					if((intersection_test < light_t) && (intersection_test > 0.001))
					{
						// Store the mesh and polygon of the closest intersected object
						mesh_intersect_index = x;
						polygon_intersect_index = y;
					}
				}
			}
		}

		// Check if we hit anything
		if( (mesh_intersect_index == -1) && (polygon_intersect_index == -1) )
		{
			// Two cases.  One for a mesh with texture, one without.

			// If there is no texture...
			if(isect.mesh_pointer->texture == NULL)
			{

				colour_array[i].r += point_light.colour.r * ( (isect.mesh_pointer->diffuse.r * LdotN) + isect.mesh_pointer->specular.r * pow( (RdotV), 1) );
				colour_array[i].g += point_light.colour.g * ( (isect.mesh_pointer->diffuse.g * LdotN) + isect.mesh_pointer->specular.g * pow( (RdotV), 1) );
				colour_array[i].b += point_light.colour.b * ( (isect.mesh_pointer->diffuse.b * LdotN) + isect.mesh_pointer->specular.b * pow( (RdotV), 1) );
			}
			else
			{
				// Get Texture Colour
				texcolour = ReturnColourFromTextureMap(isect.tex, isect.mesh_pointer->texture);

				colour_array[i].r += point_light.colour.r * ( ( texcolour.r * LdotN ) + isect.mesh_pointer->specular.r * pow( (RdotV), 1) );
				colour_array[i].g += point_light.colour.g * ( ( texcolour.g * LdotN ) + isect.mesh_pointer->specular.g * pow( (RdotV), 1) );
				colour_array[i].b += point_light.colour.b * ( ( texcolour.b * LdotN ) + isect.mesh_pointer->specular.b * pow( (RdotV), 1) );
			}

			saturate(&colour_array[i].r);
			saturate(&colour_array[i].g);
			saturate(&colour_array[i].b);
		}

	}

	// Now we cycle through all the colours and find an average

	colour = newColour(0.0, 0.0, 0.0, 1.0);
	for(i = 0; i < num_iterations; i++)
	{
		colour.r += colour_array[i].r;
		colour.g += colour_array[i].g;
		colour.b += colour_array[i].b;
	}

	colour.r = colour.r / (float)num_iterations;
	colour.g = colour.g / (float)num_iterations;
	colour.b = colour.b / (float)num_iterations;

	return colour;
}
		




	





#endif