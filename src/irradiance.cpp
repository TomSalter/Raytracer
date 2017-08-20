#ifndef IRRADIANCE_C
#define IRRADIANCE_C

#include "irradiance.h"

extern WORLD world;

// Irradiance map is a direct value returned from the photon map.
// It's an estimate of the light leaving this position.
// We map this, because querying the photon map directly is SLOOOOOOOW! 
// So we pre-compute it and put these values into a map
IRRADIANCE_MAP CompleteIrradianceMap(int mesh_index, int polygon_index)
{
	unsigned int ir_map_size;
	unsigned int i, j;
	float triangle_area, gamma;
	IRRADIANCE_MAP ir_map;
	float u, v;

	RAY ray;
	INTERSECTION isect;

	VERTEX3 v1, v2, v3;

	v1 = world.meshes[mesh_index].vertices[world.meshes[mesh_index].polygons[polygon_index].index[0]];
	v2 = world.meshes[mesh_index].vertices[world.meshes[mesh_index].polygons[polygon_index].index[1]];
	v3 = world.meshes[mesh_index].vertices[world.meshes[mesh_index].polygons[polygon_index].index[2]];

	// First we calculate the size of the polygon.  Then we can create an irradiance map correctly.

	triangle_area = CalculateTriangleArea(	v1.position, v2.position, v3.position );
	printf("Triangle area is %f.\n", triangle_area);

	ir_map_size = (unsigned int)triangle_area * IRRADIANCE_CACHE_FACTOR;
	
//	if(DEBUG)
//	{
		printf("Triangle irradiance map is of size %d.\n", ir_map_size);
//	}

	// Now we have the size of our irradiance map we can begin to allocate irradiance elements to it.

	ir_map.size = ir_map_size;
	ir_map.ir_map = (IRRADIANCE_ELEMENT**)malloc(ir_map.size * sizeof(IRRADIANCE_ELEMENT*));

	// Check for running out of memory
	if(ir_map.ir_map == NULL)
	{
		ReportError(IRRADIANCE_MAP_OUT_OF_MEMORY, "ir_map_1");
	}

	// Allocate secondary array
	for( i = 0; i < ir_map_size; i++ )
	{
		ir_map.ir_map[i] = (IRRADIANCE_ELEMENT*)malloc(ir_map.size * sizeof(IRRADIANCE_ELEMENT));

		if(ir_map.ir_map[i] == NULL)
		{
			ReportError(IRRADIANCE_MAP_OUT_OF_MEMORY, "ir_map_2");
		}
	}

	// Now we have to fill the map in from photon details
	for(i = 0; i < ir_map_size; i++)
	{
		u = (float)i / (float)ir_map_size;
		assert((u >= 0.0) && (v <= 1.0));

		if(!((u >= 0.0) && (v <= 1.0)))
		{
			printf("u = %f v = %f\n", u, v);
			ReportError(IRRADIANCE_COORDS_OUT_OF_RANGE, "CompleteIrradianceMap()");
		}

		for(j = 0; j < ir_map_size; j++)
		{
			v = (float)j / (float)ir_map_size;
			assert((v >= 0.0) && (v <= 1.0));
			float temp = u + v;
			printf("%f.\n", temp);

			gamma = 1.0 - (u + v);
			if(gamma < 0.0)
			{
				printf("BLEURGH-.\n");
				gamma = 0.0;
			}
			if(gamma > 1.0)
			{
				gamma = 1.0;
				printf("BLEURGH+.\n");
			}

			isect.position.i =	gamma * v1.position.i +
								u * v2.position.i +
								v * v3.position.i;
			isect.position.j =	gamma * v1.position.j +
								u * v2.position.j +
								v * v3.position.j;
			isect.position.k =	gamma * v1.position.k +
								u * v2.position.k +
								v * v3.position.k;

			isect.normal.i =	gamma * v1.normal.i +
								u * v2.normal.i +
								v * v3.normal.i;
			isect.normal.j =	gamma * v1.normal.j +
								u * v2.normal.j +
								v * v3.normal.j;
			isect.normal.k =	gamma * v1.normal.k +
								u * v2.normal.k +
								v * v3.normal.k;

			isect.tex.u =	gamma * v1.tex.u +
							u * v2.tex.u +
							v * v3.tex.u;
			isect.tex.v =	gamma * v1.tex.v +
							u * v2.tex.v +
							v * v3.tex.v;

			isect.mesh_pointer = &world.meshes[mesh_index];

			// Form our ray
			ray.origin = isect.position;
			
			// now we store the total incedent irradiance for this position
			ir_map.ir_map[i][j].colour = newColour(0.0, 0.0, 0.0, 1.0);

			ray.vector = isect.normal;

			ir_map.ir_map[i][j].colour = ReturnPhotonMapColour(ray, &isect);

			if(DEBUG)
			{
				printf("Returned colour %f %f %f for Irradiance Map.\n",	ir_map.ir_map[i][j].colour.r, ir_map.ir_map[i][j].colour.g, ir_map.ir_map[i][j].colour.b);
			}		
		}
	}
	
	return ir_map;
}
		
COLOUR ReturnIrradianceMapColour(INTERSECTION* isect, IRRADIANCE_MAP* ir_map)
{
	// Function returns a colour from the irradiance map.
	COLOUR colour;

	unsigned int trueU, trueV;

	trueU = (unsigned int) isect->u * ir_map->size;
	trueV = (unsigned int) isect->v * ir_map->size;

	assert( trueU < ir_map->size );
	assert( trueV < ir_map->size );

	colour = ir_map->ir_map[trueU][trueV].colour;

	return colour;
}


#endif