// Entry point for the application

#include "main.h"
#include "Timer.h"

WORLD world;

int main()
{	
	int i, j, k;
	int number_of_subpixels = (int)sqrt(DISTRIBUTED_RAYTRACER_LIMIT);
	int num_iterations = (int)sqr(number_of_subpixels - 1);
	clock_t app_start, app_end;

	Timer timer;
	timer.Start();

	app_start = clock();
	
	COLOUR *colour;
	COLOUR total_colour;

	PHOTON current_photon;

	world = LoadWorldFromFile("settings.dat");

	/* Start Photon Mapping Process */
	if(!WHITTED_RAYTRACER_ONLY)
	{
		printf("Beginning the Photon Mapping process.\n");

		for(i = 0; i < world.numAreaLights; i++)
		{
			for(j = 0; j < NUMBER_OF_PHOTON_SAMPLES; j++)
			{
				current_photon = ShootPhotonFromAreaLight(world.area_lights[i]);
				FollowPhotonPath(current_photon);
				if(DEBUG)
					{
						printf("Photon %d traced.\n", j);
					}

				if(j % 1000 == 0)
					printf("Photon %d traced.\n", j);
			}
		}
		printf("Photon Mapping completed.\n");
	}


	if(IRRADIANCE_CACHING == 1)
	{

		printf("Beginning to calculate Irradiance Maps.\n");
		for(i = 0; i < world.numMeshes; i++)
		{
			for(j = 0; j < world.meshes[i].numPolys; j++)
			{
				world.meshes[i].polygons[j].ir_map = CompleteIrradianceMap(i, j);
	//			if(DEBUG)
	//			{
					printf("Completed Irradiance Map for mesh %d, polygon %d.\n", i, j);
	//			}
			}
		}
		printf("Finished calculating Irradiance Maps.\n");
	}

	world.ambient_light.r = 0.0;
	world.ambient_light.g = 0.0;
	world.ambient_light.b = 0.0;
	world.ambient_light.alpha = 1.0;

	// Set up our variables for Distributed Ray Tracing
	if(DISTRIBUTED_RAYTRACER_LIMIT > 1)
	{
		colour = (COLOUR*)malloc(num_iterations * sizeof(COLOUR));
	}
	else
	{
		colour = (COLOUR*)malloc(sizeof(COLOUR));
	}

	// Create our pointer to pass into the per-pixel loop
	INTERSECTION* current_intersect;

	printf("Creating image.\n");

	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			if(DISTRIBUTED_RAYTRACER_LIMIT == 1)
			{
				colour[0].r = world.ambient_light.r;
				colour[0].g = world.ambient_light.g;
				colour[0].b = world.ambient_light.b;
				// Calculate the initial ray from the camera	
				ReturnInitialRayFromCamera(&world.camera, j, i);
				// The camera isn't in an object
				world.camera.ray.InObject = 0;
				// Calculate the first intersection point in the scene
				current_intersect = ReturnIntersectPoint(world.camera.ray);
				// If this ray doesn't hit anything in the scene
				if(current_intersect == NULL)
				{
				//	colour[k] = newColour(1.0, 0.0, 0.0, 1.0);
				}
				// Else if it hits something in the scene
				else
				{
					// Set up initial recursive depth
					current_intersect->RecursiveDepth = RECURSIVELIMIT;
					// Create our intersection tree
					ProcessIntersect(current_intersect, RECURSIVELIMIT);
					// Traverse this tree to calculate our colour values
					ColourIntersectTree(world.camera.ray, current_intersect);
	
					colour[0] = CalculateRayTreeIntensity(current_intersect);	

					// Plot this color value
					ClearTree(current_intersect);
				}
				if(DEBUG)
				{
					printf("Completed ray tree for pixel %d by %d.\n", i, j);
					printf("Plotted colour %f %f %f.\n", colour[0].r, colour[0].g, colour[0].b);
				}

				saturate(&colour[0].r);
				saturate(&colour[0].g);
				saturate(&colour[0].b);

			//	printf("Completed ray tree for pixel %d by %d.\n", i, j);
			//	printf("Plotted colour %f %f %f.\n", colour[0].r, colour[0].g, colour[0].b);

				WritePixel(colour[0], j, i);
			}
			else
			{
				total_colour = newColour(0.0, 0.0, 0.0, 0.0);
				for(k = 0; k < num_iterations; k++)
				{
					colour[k].r = world.ambient_light.r;
					colour[k].g = world.ambient_light.g;
					colour[k].b = world.ambient_light.b;
					// Calculate the initial ray from the camera	
					ReturnJitteredRayFromCamera(&world.camera, j, i, k, number_of_subpixels);
					// The camera isn't in an object
					world.camera.ray.InObject = 0;
					// Calculate the first intersection point in the scene
					current_intersect = ReturnIntersectPoint(world.camera.ray);
					// If this ray doesn't hit anything in the scene
					if(current_intersect == NULL)
					{
					//	colour[k] = newColour(1.0, 0.0, 0.0, 1.0);
					}
					// Else if it hits something in the scene
					else
					{
						// Set up initial recursive depth
						current_intersect->RecursiveDepth = RECURSIVELIMIT;
						// Create our intersection tree
						ProcessIntersect(current_intersect, RECURSIVELIMIT);
						// Traverse this tree to calculate our colour values
						ColourIntersectTree(world.camera.ray, current_intersect);
		
						colour[k] = CalculateRayTreeIntensity(current_intersect);	

						// Plot this color value
						ClearTree(current_intersect);
					}
					if(DEBUG)
					{
						printf("Completed ray tree for pixel %d by %d.\n", i, j);
						printf("Plotted colour %f %f %f.\n", colour[k].r, colour[k].g, colour[k].b);
					}
				}


				for(k = 0; k < num_iterations; k++)
				{
					total_colour.r += colour[k].r;
					total_colour.g += colour[k].g;
					total_colour.b += colour[k].b;
				}

				total_colour.r = total_colour.r / num_iterations;
				total_colour.g = total_colour.g / num_iterations;
				total_colour.b = total_colour.b / num_iterations;

				WritePixel(total_colour, j, i);
			}
		}

		printf("Row %d of %d completed.\n", i, HEIGHT);
	}

	free(colour);

	app_end = clock();

	printf("Image completed.  Writing to file.\n");

	CreateImage("output.tga");

	printf("Output completed.\n");

	timer.Stop();

	printf("Frame completed in %.2f seconds.\n", timer.GetTime());
	printf("Your rendering score was: %d.\n", (int)((1.0 / (app_end - app_start)) * 100000000));

	//getc();

	return 1;
}
