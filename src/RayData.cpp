#ifndef RAYDATA_C
#define RAYDATA_C

#include "RayData.h"

// Our global root node for all pixels
INTERSECTION root_node;

extern WORLD world;


// Written 16/01/2004
// Returns the INTERSECTION from a ray.  Closest intersection to the ray origin. 
// Tested: No
// Improvements Available: Not yet
INTERSECTION* ReturnIntersectPoint(RAY ray)
{
	float lowest_t = 99999;
	int i, j;
	float intersection_test;
	int mesh_intersect_index = -1, polygon_intersect_index = -1;

	INTERSECTION isect;
	INTERSECTION* isectptr = NULL;

	//Loop through all objects
	for(i = 0; i < world.numMeshes; i++)
	{
		// Perform a bounding box check on this mesh
		if( CheckAxisAlignedBoundingBoxIntersection( world.meshes[i].aabb, ray) )
		{
			// Now we've hit the mesh...
			// Loop through all polygons in this object
			for(j = 0; j < world.meshes[i].numPolys; j++)
			{

//				printf("default.\n");
				// Work out the value of t for the intersection test
				intersection_test = CheckIfIntersectionQuick(ray, world.meshes[i].vertices[world.meshes[i].polygons[j].index[0]],
														   world.meshes[i].vertices[world.meshes[i].polygons[j].index[1]],
														   world.meshes[i].vertices[world.meshes[i].polygons[j].index[2]]);

				if((intersection_test < lowest_t) && (intersection_test > 0.0001))
				{
					// Store the mesh and polygon of the closest intersected object
					lowest_t = intersection_test;
					mesh_intersect_index = i;
					polygon_intersect_index = j;
				}
			}
		}
	}

	// If the ray hasn't hit anything, we return a NULL pointer.
	if(mesh_intersect_index == -1)
	{
		return NULL;
	}

	// Now we can work out the complete details of the intersection
	// of the ray with the closest object to the origin of the ray

	isect = CalculateRayTriangleIntersectionQuick(ray, 
											 world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[0]],
											 world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[1]],
											 world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[2]]);
	// This intersect is now the closest intersect to the camera

	// Assign the INTERSECTION mesh pointer to the object
	isect.mesh_pointer = &world.meshes[mesh_intersect_index];
	isect.polygon_index = polygon_intersect_index;

	isect.normal = FindHeightMapNormal(isect.tex, isect.normal, &isect.mesh_pointer->heightmap);

	//Need to create the reflected and refracted rays
	isect.ReflectRay = CalculateReflectedRay(ray, &isect);
	
	// Now for the refracted ray...
	if(ray.InObject == 0)
	{
		isect.RefractRay = CalculateRefractedRay(ray, &isect, world.meshes[mesh_intersect_index].refractive_index);
	}
	else
	{
		isect.RefractRay = CalculateRefractedRay(ray, &isect, REFRACTIVE_INDEX_AIR);
	}

	// Set both the reflected and refracted intersection events to NULL
	isect.Refraction = NULL;
	isect.Reflection = NULL;

	isectptr = (INTERSECTION*)malloc(sizeof(INTERSECTION));
	*isectptr = isect;

	return isectptr;
}

void ProcessIntersect(INTERSECTION* isect, int RecursiveDepth)
{
	if(RecursiveDepth >= 0)
	{
		// If we're in an object, don't create a reflective ray
		// Also check if the object is actually reflective
		if( (isect->ReflectRay.InObject == 0) && (isect->mesh_pointer->reflectivity != 0.0))
		{
			isect->Reflection = ReturnIntersectPoint(isect->ReflectRay);
			if(isect->Reflection != NULL)
			{
				ProcessIntersect(isect->Reflection, RecursiveDepth - 1);
			}
		}
		else
		{
			isect->Reflection = NULL;
		}

		// Don't bother refracting if the object isn't refractive
		if( isect->mesh_pointer->transparency != 0.0 )
		{
			isect->Refraction = ReturnIntersectPoint(isect->RefractRay);
			if(isect->Refraction != NULL)
			{
				ProcessIntersect(isect->Refraction, RecursiveDepth - 1);

			}
		}
		else
		{
			isect->Refraction = NULL;
		}
	}
	else
	{
		isect->Reflection = NULL;
		isect->Refraction = NULL;
	}

	return;
}

COLOUR PhongShadeIntersect(RAY ray, INTERSECTION isect)
{
	VECTOR3 Light;
	VECTOR3 View;
	VECTOR3 Normal;
	VECTOR3 Reflect;
	COLOUR colour;
	COLOUR texcolour;
	COLOUR photonmap_colour;
	RAY light_ray;

	int i, x, y;

	int mesh_intersect_index;
	int polygon_intersect_index;

	float LdotN;
	float RdotV;
	float light_t;
	float intersection_test;


	colour = newColour(0.0, 0.0, 0.0, 1.0);

	// Precompute some values

	View = SubtractVector3(ray.origin, isect.position);
	NormaliseVector3(&View);

	Normal = isect.normal;
	NormaliseVector3(&Normal);

	// If we're in an object, we need to turn the normal around
	if(ray.InObject == 1)
	{
//		NegateVector3(&Normal);
	}

	if(!PHOTON_MAPPING_ONLY)
	{
		for(i = 0; i < world.numPointLights; i++)
		{
			// Calculate the light vector

			Light = SubtractVector3(world.point_lights[i].position, isect.position);
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

					colour.r += world.point_lights[i].colour.r * ( (isect.mesh_pointer->diffuse.r * LdotN) + isect.mesh_pointer->specular.r * pow( (RdotV), 1) );
					colour.g += world.point_lights[i].colour.g * ( (isect.mesh_pointer->diffuse.g * LdotN) + isect.mesh_pointer->specular.g * pow( (RdotV), 1) );
					colour.b += world.point_lights[i].colour.b * ( (isect.mesh_pointer->diffuse.b * LdotN) + isect.mesh_pointer->specular.b * pow( (RdotV), 1) );
				}
				else
				{
					// Get Texture Colour
					texcolour = ReturnColourFromTextureMap(isect.tex, isect.mesh_pointer->texture);

					colour.r += world.point_lights[i].colour.r * ( ( texcolour.r * LdotN ) + isect.mesh_pointer->specular.r * pow( (RdotV), 1) );
					colour.g += world.point_lights[i].colour.g * ( ( texcolour.g * LdotN ) + isect.mesh_pointer->specular.g * pow( (RdotV), 1) );
					colour.b += world.point_lights[i].colour.b * ( ( texcolour.b * LdotN ) + isect.mesh_pointer->specular.b * pow( (RdotV), 1) );
				}
			}
		}
	}

	if(WHITTED_RAYTRACER_ONLY)
		return colour;

	// Now we have worked out direct illumination, we can proceed to do Photon Mapping for this surface.

	photonmap_colour = newColour(0.0, 0.0, 0.0, 1.0);

	if(PHOTON_MAPPING_FINAL_GATHERING)
	{
	//	photonmap_colour = ReturnPhotonMapColourFinalGathering(ray, &isect);
	//	photonmap_colour = ReturnIrradianceMapColour(&isect, &world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].ir_map);
		photonmap_colour = ReturnIrradianceMapColour(&isect, &isect.mesh_pointer->polygons[isect.polygon_index].ir_map);
	}
	else
	{
		// Only gather photons if the surface is not transparent
		if(isect.mesh_pointer->transparency == 0.0)
		{
			if(DRAW_PHOTONS)
			{
				photonmap_colour = ReturnPhotonMapColourDrawPhotons(ray, &isect);
			}
			else
			{
				photonmap_colour = ReturnPhotonMapColour(ray, &isect);
			}
		}
	}

	colour.r += photonmap_colour.r;
	colour.g += photonmap_colour.g;
	colour.b += photonmap_colour.b;

	if(photonmap_colour.r < 0.0)
		printf("ERROR WITH PHOTONMAP COLOUR.\n");

	saturate(&colour.r);
	saturate(&colour.g);
	saturate(&colour.b);

	if(PHOTON_MAPPING_ONLY)
		return photonmap_colour;

	return colour;
}

void ColourIntersectTree(RAY ray, INTERSECTION* isect)
{
	COLOUR colour;
	int i;
	// Take into account the first ray
	if(isect == NULL)
	{
		return;
	}

	if(isect->Reflection != NULL)
	{
		ColourIntersectTree(isect->ReflectRay, isect->Reflection);
	}

	if(isect->Refraction != NULL)
	{
		ColourIntersectTree(isect->RefractRay, isect->Refraction);
	}

	isect->colour = PhongShadeIntersect(ray, *isect);

	// Take into account the area lights
	if(world.numAreaLights != 0)
	{
		for(i = 0; i < world.numAreaLights; i++)
		{
			colour = ReturnAreaLightIntensity(*isect, world.area_lights[i], ray);
			isect->colour.r += colour.r;
			isect->colour.g += colour.g;
			isect->colour.b += colour.b;
		}
	}
	
	return;
}

COLOUR CalculateRayTreeIntensity(INTERSECTION* isect)
{
	COLOUR colour;
	COLOUR temp_colour1;
	COLOUR temp_colour2;

	colour = isect->colour;

	if(isect->mesh_pointer->transparency > 0.0)
	{
		colour.r = (1.0 - isect->mesh_pointer->transparency) * colour.r;
		colour.g = (1.0 - isect->mesh_pointer->transparency) * colour.g;
		colour.b = (1.0 - isect->mesh_pointer->transparency) * colour.b;
	}
	else
	{
		colour.r = (1.0 - isect->mesh_pointer->reflectivity) * colour.r;
		colour.g = (1.0 - isect->mesh_pointer->reflectivity) * colour.g;
		colour.b = (1.0 - isect->mesh_pointer->reflectivity) * colour.b;
	}

	if(isect->Reflection != NULL)
	{
		temp_colour1 = CalculateRayTreeIntensity(isect->Reflection);
		colour.r += (isect->mesh_pointer->reflectivity * temp_colour1.r);
		colour.g += (isect->mesh_pointer->reflectivity * temp_colour1.g);
		colour.b += (isect->mesh_pointer->reflectivity * temp_colour1.b);
	}

	if(isect->Refraction != NULL)
	{
		temp_colour2 = CalculateRayTreeIntensity(isect->Refraction);
		colour.r += (isect->mesh_pointer->transparency * temp_colour2.r);
		colour.g += (isect->mesh_pointer->transparency * temp_colour2.g);
		colour.b += (isect->mesh_pointer->transparency * temp_colour2.b);
	}

	return colour;
}

void ClearTree(INTERSECTION* isect)
{
	if(isect->Reflection != NULL)
		ClearTree(isect->Reflection);
	if(isect->Refraction != NULL)
		ClearTree(isect->Refraction);

	free(isect);

	isect = NULL;

	return;
}


			



			














	


#endif