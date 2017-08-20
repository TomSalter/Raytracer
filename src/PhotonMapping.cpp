#ifndef PHOTONMAPPING_C
#define PHOTONMAPPING_C

#include "PhotonMapping.h"

#define PI 3.141592654


float PHOTON_COLOUR_SCALE = (1.0 / NUMBER_OF_PHOTON_SAMPLES) * 200.0;

#define PHOTON_MAPPING_FILTER_VALUE	1
#define RR_ABSORB_BOUNDARY 0.4

extern WORLD world;

unsigned long int next = 1;
int PhotonBounces = 0;


int rand_num(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next/65536) % 32768;
}

PHOTON ShootPhotonFromAreaLight(AREA_LIGHT light)
{
	// Choose a random position on the area light
	VECTOR3 rand_pos;
	VECTOR3 rand_dir;

	PHOTON photon;

	VECTOR3 left_side, right_side;

	// Let's work out the position between 0 and 1 vertices
	float t;

	t = (float)rand() / RAND_MAX;

	left_side.i = (t * (light.corners[1].i - light.corners[0].i));
	left_side.j = (t * (light.corners[1].j - light.corners[0].j));
	left_side.k = (t * (light.corners[1].k - light.corners[0].k));

	// Work out the position between 0 and 3 vertices
	t = (float)rand() / RAND_MAX;

	right_side.i = (t * (light.corners[3].i - light.corners[0].i));
	right_side.j = (t * (light.corners[3].j - light.corners[0].j));
	right_side.k = (t * (light.corners[3].k - light.corners[0].k));

	// Work out the final position of the photon origin
	rand_pos.i = light.corners[0].i + left_side.i + right_side.i;
	rand_pos.j = light.corners[0].j + left_side.j + right_side.j;
	rand_pos.k = light.corners[0].k + left_side.k + right_side.k;

	// Now we calculate a completely random vector for the photon distribution
	rand_dir.i = ((float)rand() / RAND_MAX) - 0.5;
	rand_dir.j = ((float)rand() / RAND_MAX) - 0.5;
	rand_dir.k = ((float)rand() / RAND_MAX) - 0.5;
	NormaliseVector3(&rand_dir);
	rand_dir.w = 1.0;

	photon.photon_ray.origin = rand_pos;
	photon.photon_ray.vector = rand_dir;

	photon.photon_ray.InObject = 0;
	photon.photon_ray.current_refractive_index = REFRACTIVE_INDEX_AIR;

	photon.colour.r = light.colour.r * PHOTON_COLOUR_SCALE;
	photon.colour.g = light.colour.g * PHOTON_COLOUR_SCALE;
	photon.colour.b = light.colour.b * PHOTON_COLOUR_SCALE;

	photon.index = 0;

	photon.used_flag = 0;

	return photon;
}
	

void FollowPhotonPath(PHOTON photon)
{
	float lowest_t = 100000;
	int mesh_intersect_index = -1;
	int polygon_intersect_index = -1;
	float intersection_test;
	INTERSECTION isect;
	float rr_test;
	int i, j;
	COLOUR tex;

	//Loop through all objects
	for(i = 0; i < world.numMeshes; i++)
	{
		// Perform a bounding box check on this mesh
		if( CheckAxisAlignedBoundingBoxIntersection( world.meshes[i].aabb, photon.photon_ray) )
		{
			// Now we've hit the mesh...
			// Loop through all polygons in this object
			for(j = 0; j < world.meshes[i].numPolys; j++)
			{
				// Work out the value of t for the intersection test
				intersection_test = CheckIfIntersectionQuick(photon.photon_ray, world.meshes[i].vertices[world.meshes[i].polygons[j].index[0]],
														   world.meshes[i].vertices[world.meshes[i].polygons[j].index[1]],
														   world.meshes[i].vertices[world.meshes[i].polygons[j].index[2]]);

				if((intersection_test < lowest_t) && (intersection_test > 0.001))
				{
					// Store the mesh and polygon of the closest intersected object
					lowest_t = intersection_test;
					mesh_intersect_index = i;
					polygon_intersect_index = j;
				}
			}
		}
	}

	// If there was no intersection...
	if(mesh_intersect_index == -1)
	{
		if(DEBUG)
		{
			printf("Photon did not intersect an object.\n");
		}
		return;
	}

	// Calculate the position of this intersection event

	isect = CalculateRayTriangleIntersectionQuick(	photon.photon_ray,
													world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[0]],
													world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[1]],
													world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[2]]);

	isect.mesh_pointer = &world.meshes[mesh_intersect_index];

	// Now we have an INTERSECTION event where we can store the photon location in.

	// If the photon is in an object, always do refraction
	if(photon.photon_ray.InObject == 1)
	{
		// We're in an object.
		photon.photon_ray = CalculateRefractedRay(photon.photon_ray, &isect, REFRACTIVE_INDEX_AIR);
		FollowPhotonPath(photon);
		return;
	}

	if(isect.mesh_pointer->transparency != 0.0)
	{
		// Refract the photon
		photon.photon_ray = CalculateRefractedRay(photon.photon_ray, &isect, isect.mesh_pointer->refractive_index);
		if(isect.mesh_pointer->texture == NULL)
		{
			if(PHOTON_COLOUR_BLEED)
			{
				photon.colour.r *= isect.mesh_pointer->diffuse.r;
				photon.colour.g *= isect.mesh_pointer->diffuse.g;
				photon.colour.b *= isect.mesh_pointer->diffuse.b;
			}
		}
		else
		{
			if(PHOTON_COLOUR_BLEED)
			{
				tex = ReturnColourFromTextureMap(isect.tex, isect.mesh_pointer->texture);

				photon.colour.r *= tex.r;
				photon.colour.g *= tex.g;
				photon.colour.b *= tex.b;
			}
		}
		FollowPhotonPath(photon);
		return;
	}

	// Now perform Russian Roulette test.

	rr_test = (float)rand() / RAND_MAX;

	// Absorb the photon
	if( ((rr_test >= 0.0) && (rr_test <= RR_ABSORB_BOUNDARY)) || (PhotonBounces == PHOTON_MAPPING_RECURSIVE_LIMIT) )
	{
		// Store polygon in the list...
		// There was an intersection, so we record this value in the polygon photon map

	//	printf("BEFORE NV = %f %f %f.\n", isect.normal.i, isect.normal.j, isect.normal.k);

		// Check to see if we need to allocate more space:
		if(world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].num_photons >= (world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].alloc_space - 10) )
		{
			// We need to allocate more space for the photons here
			// Here we allocate another 100 locations
			world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].alloc_space += 100;
			world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].photon_list = (PHOTON*)realloc(world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].photon_list, world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].alloc_space * sizeof(PHOTON));
			
			if(DEBUG)
			{
				printf("Allocated more space for photon list.\n");
			}
		}

		if(DEBUG)
		{
			// Printout where we store the photons location
			printf("Photon hit mesh %d at polygon %d.\n", mesh_intersect_index, polygon_intersect_index);
			printf("Stored at location %d.\n", world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].num_photons);
		}

		// Store the photon in the list...
		photon.photon_ray.origin = isect.position;
		world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].photon_list[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].num_photons] = photon;

		// Increment the number of photons
		world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].num_photons++;

		PhotonBounces = 0;
	}
	// Else the photon is reflected isotropic.
	// For some reason the code dies here, WHY?!
	else
	{
	//	printf("BEFORE NV = %f %f %f.\n", isect.normal.i, isect.normal.j, isect.normal.k);
		// Check if reflective or refractive
		if(isect.mesh_pointer->transparency == 0.0)
		{
			photon.photon_ray = CalculateReflectedRay(photon.photon_ray, &isect);

			// Scale the photon's colour with the surfaces' colour
			// Take into account texturing
			if(isect.mesh_pointer->texture == NULL)
			{
				if(PHOTON_COLOUR_BLEED)
				{
				//	photon.colour.r = photon.colour.r + photon.colour.r * isect.mesh_pointer->diffuse.r;
				//	photon.colour.g = photon.colour.g + photon.colour.g * isect.mesh_pointer->diffuse.g;
				//	photon.colour.b = photon.colour.b + photon.colour.b * isect.mesh_pointer->diffuse.b;

					photon.colour.r *= isect.mesh_pointer->diffuse.r;
					photon.colour.g *= isect.mesh_pointer->diffuse.g;
					photon.colour.b *= isect.mesh_pointer->diffuse.b;
				}
			}
			else
			{
				if(PHOTON_COLOUR_BLEED)
				{
					tex = ReturnColourFromTextureMap(isect.tex, isect.mesh_pointer->texture);
				//	photon.colour.r = photon.colour.r + photon.colour.r * tex.r;
				//	photon.colour.g = photon.colour.g + photon.colour.g * tex.g;
				//	photon.colour.b = photon.colour.b + photon.colour.b * tex.b;

					photon.colour.r *= tex.r;
					photon.colour.g *= tex.g;
					photon.colour.b *= tex.b;
				}
			}

			FollowPhotonPath(photon);
			return;
		}
		else
		{
			if(photon.photon_ray.InObject == 0)
			{
				photon.photon_ray = CalculateRefractedRay(photon.photon_ray, &isect, REFRACTIVE_INDEX_AIR);
			}
			else
			{
				photon.photon_ray = CalculateRefractedRay(photon.photon_ray, &isect, isect.mesh_pointer->refractive_index);
			}

			// Scale the photon's colour with the surfaces' colour
			// Take into account texturing
			if(isect.mesh_pointer->texture == NULL)
			{
				if(PHOTON_COLOUR_BLEED)
				{
				//	photon.colour.r = photon.colour.r + photon.colour.r * isect.mesh_pointer->diffuse.r;
				//	photon.colour.g = photon.colour.g + photon.colour.g * isect.mesh_pointer->diffuse.g;
				//	photon.colour.b = photon.colour.b + photon.colour.b * isect.mesh_pointer->diffuse.b;
					photon.colour.r *= isect.mesh_pointer->diffuse.r;
					photon.colour.g *= isect.mesh_pointer->diffuse.g;
					photon.colour.b *= isect.mesh_pointer->diffuse.b;

				}
			}
			else
			{
				if(PHOTON_COLOUR_BLEED)
				{
					tex = ReturnColourFromTextureMap(isect.tex, isect.mesh_pointer->texture);
					photon.colour.r = photon.colour.r + photon.colour.r * tex.r;
					photon.colour.g = photon.colour.g + photon.colour.g * tex.g;
					photon.colour.b = photon.colour.b + photon.colour.b * tex.b;

					photon.colour.r *= tex.r;
					photon.colour.g *= tex.g;
					photon.colour.b *= tex.b;
				}
			}

			if(DEBUG)
			{
				printf("Reflecting Photon...\n");
			}
			// Now we follow the path of this reflected photon
			// Attenuate light here...?
			FollowPhotonPath(photon);
			PhotonBounces++;
			return;
		}
	}

	return;
}


COLOUR ReturnPhotonMapColour(RAY ray, INTERSECTION*isect)
{
	VECTOR3 Light;
	VECTOR3 Normal;
	VECTOR3 length_vect;
	
	float length;
	float filter_value;
	float LdotN;
	int i, j, k;

	COLOUR colour = newColour(0.0, 0.0, 0.0, 0.0);
	COLOUR final_colour = newColour(0.0, 0.0, 0.0, 0.0);
	COLOUR diffuse;

	// Photon list to operate on
	PHOTON** Photons;
	PHOTON temp_photon;

	// Need to let the array changeable, in case there are less than PHOTONSAMPLINGNUMBER photons hitting a surface
	int number_of_samples;
	int lowest_index_polygon;
	int lowest_index_photon;
	float lowest_length = 1000000;
	float sampling_length;

	// Fill the array with the N closest photons
	Normal = isect->normal;

	// Need to find out how many photons are on the surface
	number_of_samples = 0;

	for(i = 0; i < isect->mesh_pointer->numPolys;i++)
	{
		number_of_samples += isect->mesh_pointer->polygons[i].num_photons;
	}

	if( number_of_samples >= PHOTON_SAMPLING_NUMBER)
	{
		number_of_samples = PHOTON_SAMPLING_NUMBER;
	}

	// Create the array
	Photons = (PHOTON**)malloc(number_of_samples * sizeof(PHOTON*));

	// For each element in the array...
	for(i = 0; i < number_of_samples; i++)
	{
		// For all the polygons in the mesh...
		for(j = 0; j < isect->mesh_pointer->numPolys; j++)
		{
			// For all the photons on this polygon...
			for(k = 0; k < isect->mesh_pointer->polygons[j].num_photons; k++)
			{

				length_vect = SubtractVector3(	isect->mesh_pointer->polygons[j].photon_list[k].photon_ray.origin,
												isect->position );

				length = FindLengthVector3(length_vect);

				// If this is the lowest encountered length, and the photon isn't in the list already
				if( (length < lowest_length) && (isect->mesh_pointer->polygons[j].photon_list[k].used_flag == 0) )
				{
					// If this is the lowest encountered distance
					// We set the lowest length to this one
					// And then put the photon in the array
					lowest_length = length;
					Photons[i] = &isect->mesh_pointer->polygons[j].photon_list[k];
					lowest_index_photon = k;
					lowest_index_polygon = j;
				}
			}
		}

		lowest_length = 1000000;
		isect->mesh_pointer->polygons[lowest_index_polygon].photon_list[lowest_index_photon].used_flag = 1;

		if(Photons[i] == NULL)
		{
			printf("ERROR!!\n");
		}
	}

	filter_value = 1.0 - (2 / (3 * PHOTON_MAPPING_FILTER_VALUE));

	length_vect = SubtractVector3( Photons[number_of_samples - 1]->photon_ray.origin, isect->position);
	sampling_length = FindLengthVector3(length_vect);

	// Now we have the Photon[], we can then use them to get the colour out
	for(i = 0; i < number_of_samples; i++)
	{
		assert( Photons[i] != NULL);

		temp_photon = *Photons[i];

		Light.i = -temp_photon.photon_ray.vector.i;
		Light.j = -temp_photon.photon_ray.vector.j;
		Light.k = -temp_photon.photon_ray.vector.k;

		NormaliseVector3(&Light);

		LdotN = DotProductVector3(Light, Normal);
		saturate(&LdotN);

		// Do it for texturing...
		if(isect->mesh_pointer->texture == NULL)
		{
			colour.r = temp_photon.colour.r * (isect->mesh_pointer->diffuse.r * LdotN);
			colour.g = temp_photon.colour.g * (isect->mesh_pointer->diffuse.g * LdotN);
			colour.b = temp_photon.colour.b * (isect->mesh_pointer->diffuse.b * LdotN);
		//	colour.r = temp_photon.colour.r * (isect->mesh_pointer->diffuse.r);
		//	colour.g = temp_photon.colour.g * (isect->mesh_pointer->diffuse.g);
		//	colour.b = temp_photon.colour.b * (isect->mesh_pointer->diffuse.b);

		}
		else
		{
			diffuse = ReturnColourFromTextureMap(isect->tex, isect->mesh_pointer->texture);
			colour.r = temp_photon.colour.r * (diffuse.r * LdotN);
			colour.g = temp_photon.colour.g * (diffuse.g * LdotN);
			colour.b = temp_photon.colour.b * (diffuse.b * LdotN);

		//	colour.r = temp_photon.colour.r * (diffuse.r);
		//	colour.g = temp_photon.colour.g * (diffuse.g);
		//	colour.b = temp_photon.colour.b * (diffuse.b);
		}
		if(DEBUG)
			printf("Found a photon.\n");

		// Modify this colour according to the distance from the photon
		colour.r = colour.r / (PI * sqr(sampling_length));
		colour.g = colour.g / (PI * sqr(sampling_length));
		colour.b = colour.b / (PI * sqr(sampling_length));

		final_colour.r += colour.r;
		final_colour.g += colour.g;
		final_colour.b += colour.b;
	}

	// Now we need to clear the photon list to unused again, ready for the next pass
	for(i = 0; i < isect->mesh_pointer->numPolys; i++)
	{
		for(j = 0; j < isect->mesh_pointer->polygons[i].num_photons; j++)
		{
			isect->mesh_pointer->polygons[i].photon_list[j].used_flag = 0;
		}
	}

	// Free the photon array
	free(Photons);
	return final_colour;
}
					
COLOUR ReturnPhotonMapColourDrawPhotons(RAY ray, INTERSECTION* isect)
{
	VECTOR3 Light;
	VECTOR3 Normal;
	VECTOR3 length_vect;
	PHOTON photon;
	COLOUR colour;
	COLOUR diffuse;
	int i, j, k;

	float length;

	float LdotN;

	Normal = isect->normal;
	NormaliseVector3(&Normal);

	colour = newColour(0.0, 0.0, 0.0, 0.0);

	k = 0;
	for(i = 0; i < isect->mesh_pointer->numPolys; i++)
	{
		for(j = 0; j < isect->mesh_pointer->polygons[i].num_photons; j++)
		{
			// Distance check
			length_vect = SubtractVector3( isect->mesh_pointer->polygons[i].photon_list[j].photon_ray.origin, isect->position);
			length = FindLengthVector3(length_vect);

			if( length < PHOTON_SAMPLING_DISTANCE )
			{
				// FIXME :: Perform a check on the normals of the polygons
				// We have found a photon.  Get the colour of said photon.

				photon = isect->mesh_pointer->polygons[i].photon_list[j];

				Light.i = -photon.photon_ray.vector.i;
				Light.j = -photon.photon_ray.vector.j;
				Light.k = -photon.photon_ray.vector.k;

				NormaliseVector3(&Light);

				LdotN = DotProductVector3(Light, Normal);
				saturate(&LdotN);

				// Do it for texturing...
				if(isect->mesh_pointer->texture == NULL)
				{
					colour.r += photon.colour.r * (isect->mesh_pointer->diffuse.r * LdotN);
					colour.g += photon.colour.g * (isect->mesh_pointer->diffuse.g * LdotN);
					colour.b += photon.colour.b * (isect->mesh_pointer->diffuse.b * LdotN);
				}
				else
				{
					diffuse = ReturnColourFromTextureMap(isect->tex, isect->mesh_pointer->texture);
					colour.r += photon.colour.r * (diffuse.r * LdotN);
					colour.g += photon.colour.g * (diffuse.g * LdotN);
					colour.b += photon.colour.b * (diffuse.b * LdotN);
				}
				if(DEBUG)
				{
					printf("Found a photon.  \n");
				}
			}
		}
	}

	// Divide by unit area
	colour.r /= PI * sqr(PHOTON_SAMPLING_DISTANCE);
	colour.g /= PI * sqr(PHOTON_SAMPLING_DISTANCE);
	colour.b /= PI * sqr(PHOTON_SAMPLING_DISTANCE);

	return colour;
}

COLOUR ReturnPhotonMapColourFinalGathering(RAY ray, INTERSECTION* isect)
{
	// For an intersection, it shoots out 1024 random rays
	// Each ray is randomly generated over the hemisphere
	// Sample the photon map at this place, and consider it a light source

	VECTOR3 Light;
	VECTOR3 Normal;
	COLOUR colour;
	COLOUR diffuse;
	int i;
	float LdotN;
	RAY light_ray;
	INTERSECTION intersect;
	float intersection_test;
	float lowest_t = 100000;
	int mesh_intersect_index, polygon_intersect_index;
	int x, y;

	LIGHT light_array;

	Normal = isect->normal;

	colour = newColour(0.0, 0.0, 0.0, 1.0);

	for(i = 0; i < FINAL_GATHERING_LIMIT; i++)
	{
		// Form the random ray
		light_ray.origin = isect->position;
		light_ray.vector = ReturnDiffuseVector(isect->normal);

		// Check to see what this ray hits

		// Find if this ray intersects with anything
		mesh_intersect_index = -1;
		polygon_intersect_index = -1;

		lowest_t = 100000;

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
					if((intersection_test < lowest_t) && (intersection_test > 0.001))
					{
						// Store the mesh and polygon of the closest intersected object
						mesh_intersect_index = x;
						polygon_intersect_index = y;
						lowest_t = intersection_test;
					}
				}
			}
		}

		if( mesh_intersect_index == -1)
		{
			if(DEBUG)
			{
				printf("Photon Map Final Gathering Ray #%d didn't hit anything.\n", i);
			}
		}
		else
		{
			intersect = CalculateRayTriangleIntersectionQuick(light_ray, 
														world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[0]],
														world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[1]],
														world.meshes[mesh_intersect_index].vertices[world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].index[2]]);

			intersect.mesh_pointer = &world.meshes[mesh_intersect_index];

			// Extrapolate this colour from the light vector
//			light_array.colour = ReturnPhotonMapColour(light_ray, &intersect);

			light_array.colour = ReturnIrradianceMapColour(&intersect, &world.meshes[mesh_intersect_index].polygons[polygon_intersect_index].ir_map);

			light_array.position = intersect.position;

			// No need to do intersection test, already done.

			Light = SubtractVector3(light_array.position, isect->position);

			LdotN = DotProductVector3(Light, Normal);

			// Do it for texturing...
			if(isect->mesh_pointer->texture == NULL)
			{
				colour.r += light_array.colour.r * (isect->mesh_pointer->diffuse.r * LdotN);
				colour.g += light_array.colour.g * (isect->mesh_pointer->diffuse.g * LdotN);
				colour.b += light_array.colour.b * (isect->mesh_pointer->diffuse.b * LdotN);
			}
			else
			{
				diffuse = ReturnColourFromTextureMap(isect->tex, isect->mesh_pointer->texture);
				colour.r += light_array.colour.r * (diffuse.r * LdotN);
				colour.g += light_array.colour.g * (diffuse.g * LdotN);
				colour.b += light_array.colour.b * (diffuse.b * LdotN);
			}
		}
	}

	// Divide by number of rays
//	colour.r /= FINAL_GATHERING_LIMIT;
//	colour.g /= FINAL_GATHERING_LIMIT;
//	colour.b /= FINAL_GATHERING_LIMIT;

	return colour;
}

// Function returns a diffuse vector, according to a previously given normal vector
// Shoots out over the whole hemisphere 
VECTOR3 ReturnDiffuseVector(VECTOR3 Normal)
{
	VECTOR3 diffuse;

	float val;
	for(;;)
	{
	//	diffuse.i = ( (float)rand() / RAND_MAX) - 0.5;
	//	diffuse.j = ( (float)rand() / RAND_MAX) - 0.5;
	//	diffuse.k = ( (float)rand() / RAND_MAX) - 0.5;

		diffuse.i = ( (float)rand_num() / 32768) - 0.5;
		diffuse.j = ( (float)rand_num() / 32768) - 0.5;
		diffuse.k = ( (float)rand_num() / 32768) - 0.5;

	//	printf("RV %f %f %f\n", diffuse.i, diffuse.j, diffuse.k);
	//	printf("NV %f %f %f\n", Normal.i, Normal.j, Normal.k);

		NormaliseVector3(&diffuse);

		val = DotProductVector3(Normal, diffuse);

	//	printf("
		
		if( DotProductVector3(Normal, diffuse) >= 0.0 )
			return diffuse;

	//	printf("Not successful...\n");
	}
}

#endif

