#ifndef WORLD_C
#define WORLD_C

#include "World.h"


void ProcessMesh(MESH* mesh)
{
	// Function creates a matrix for each mesh
	MATRIX4x4 translation;
	MATRIX4x4 rotationx, rotationy, rotationz;
	MATRIX4x4 scalar;
	MATRIX4x4 A, B, C, D;

	int i;

	newMatrix4x4(&translation);
	newMatrix4x4(&rotationx);
	newMatrix4x4(&rotationy);
	newMatrix4x4(&rotationz);
	newMatrix4x4(&scalar);

	// Form our scalar matrix
	scalar.mat[0][0] = mesh->scalar.i;
	scalar.mat[1][1] = mesh->scalar.j;
	scalar.mat[2][2] = mesh->scalar.k;

	// Form our rotationx matrix
	rotationx.mat[1][1] = cos(DEG2RAD * mesh->rotation.i);
	rotationx.mat[1][2] = sin(DEG2RAD * mesh->rotation.i);
	rotationx.mat[2][1] = -sin(DEG2RAD * mesh->rotation.i);
	rotationx.mat[2][2] = cos(DEG2RAD * mesh->rotation.i);

	// Form our rotationz matrix
	rotationz.mat[0][0] = cos(DEG2RAD * mesh->rotation.j);
	rotationz.mat[0][1] = sin(DEG2RAD * mesh->rotation.j);
	rotationz.mat[1][0] = -sin(DEG2RAD * mesh->rotation.j);
	rotationz.mat[1][1] = cos(DEG2RAD * mesh->rotation.j);

	// Form our rotationy matrix
	rotationy.mat[0][0] = cos(DEG2RAD * mesh->rotation.k);
	rotationy.mat[0][2] = -sin(DEG2RAD * mesh->rotation.k);
	rotationy.mat[2][0] = sin(DEG2RAD * mesh->rotation.k);
	rotationy.mat[2][2] = cos(DEG2RAD * mesh->rotation.k);

	// Form our translation matrix

	translation.mat[3][0] = mesh->translation.i;
	translation.mat[3][1] = mesh->translation.j;
	translation.mat[3][2] = mesh->translation.k;

	// Form our composite matrix
	A = MatrixMultiply4(scalar, rotationx);
	B = MatrixMultiply4(rotationy, rotationz);
	C = MatrixMultiply4(A, B);
	D = MatrixMultiply4(C, translation);
	C = MatrixMultiply4(rotationx, B);

	// Apply this composite matrix to all the 

	for(i = 0; i < mesh->numVertices; i++)
	{
		mesh->vertices[i].position = MultiplyVectorWithMatrix4(mesh->vertices[i].position, D);
		mesh->vertices[i].normal = MultiplyVectorWithMatrix4(mesh->vertices[i].normal, C);
	}

	printf("Applied matrix transformations to mesh %d.\n", mesh->index);
	return;
}










// Written 09/01/2004
// Returns a WORLD object loaded from a file
// Tested: YUS!
// Improvements Available: No
WORLD LoadWorldFromFile(char* filename)
{
	FILE* fptr;
	WORLD world;
	char buffer1[80], buffer2[80];
	int i;

	fptr = fopen(filename, "r");

	if(fptr == NULL)
	{
		ReportError(WORLD_OPEN_FAILED, filename);
	}
	
	// File opened OK.  Continue to load world details.
	// File is opened in the following order:
	// 1)  Camera.  Eyepoint, Q, B, S
	// 2)  Number of meshes
	// 3)  mesh index, .x file, .hmp file, diffuse, specular, refractive index, transparency
	// 4)  number of lights
	// 5)  light index, light position, light color
	// 6)  number of area lights
	// 7)  vertex1, vertex2, vertex3, vertex4, color

	// Read in camera
	fscanf(fptr, "%f\t%f\t%f", &world.camera.eyepoint.i, &world.camera.eyepoint.j, &world.camera.eyepoint.k);
	fscanf(fptr, "%f\t%f\t%f", &world.camera.Q.i, &world.camera.Q.j, &world.camera.Q.k);
	fscanf(fptr, "%f\t%f\t%f", &world.camera.B.i, &world.camera.B.j, &world.camera.B.k);
	fscanf(fptr, "%f\t%f\t%f", &world.camera.S.i, &world.camera.S.j, &world.camera.S.k);

	// Camera loaded.

	// Now for the meshes
	fscanf(fptr, "%d", &world.numMeshes);

	// Allocate memory for the meshes

	world.meshes = (MESH*)malloc(world.numMeshes * sizeof(MESH));

	// Check for lack of memory
	if(world.meshes == NULL)
	{
		ReportError(MESH_MALLOC_FAILED, "nullptr");
	}

	// Loop through the file loading the meshes as we go
	for(i = 0; i < world.numMeshes; i++)
	{
		// Load in the index
		fscanf(fptr, "%d", &world.meshes[i].index);
		// Load in the filename
		fscanf(fptr, "%s", &buffer1);
		// Load in the hmp filename
		fscanf(fptr, "%s", &buffer2);
		// Call the .x loading function to load up our mesh...
		world.meshes[i] = LoadMeshFromFile(buffer1, buffer2);
		world.meshes[i].index = i;

		// Check to see if there's a texture there
		fscanf(fptr, "%s", &buffer1);

		if( strcmp(buffer1, "NULL") == 0 )
		{
			world.meshes[i].texture = NULL;
		}
		else
		{
			world.meshes[i].texture = LoadTexture(buffer1);
		}
		// Load up the diffuse levels
		fscanf(fptr, "%f\t%f\t%f\t%f", &world.meshes[i].diffuse.r, &world.meshes[i].diffuse.g, &world.meshes[i].diffuse.b, &world.meshes[i].diffuse.alpha);
		// Load up the specular levels
		fscanf(fptr, "%f\t%f\t%f\t%f", &world.meshes[i].specular.r, &world.meshes[i].specular.g, &world.meshes[i].specular.b, &world.meshes[i].specular.alpha);
		// Load in the translation vector
		fscanf(fptr, "%f\t%f\t%f", &world.meshes[i].translation.i, &world.meshes[i].translation.j, &world.meshes[i].translation.k);
		// Load in the rotation vector
		fscanf(fptr, "%f\t%f\t%f", &world.meshes[i].rotation.i, &world.meshes[i].rotation.j, &world.meshes[i].rotation.k);
		// Load in the scalar vector
		fscanf(fptr, "%f\t%f\t%f", &world.meshes[i].scalar.i, &world.meshes[i].scalar.j, &world.meshes[i].scalar.k);
		// Load up the refractive index of the object
		fscanf(fptr, "%f", &world.meshes[i].refractive_index);
		// Load up the reflectivity level
		fscanf(fptr, "%f", &world.meshes[i].reflectivity);
		// Load up the transparancy level
		fscanf(fptr, "%f", &world.meshes[i].transparency);

		ProcessMesh(&world.meshes[i]);

		world.meshes[i].aabb = CalculateAxisAlignedBoundingBox(&world.meshes[i]);
	}

	// Now we can begin to load in the point lights

	// Load in the number of point lights
	fscanf(fptr, "%d", &world.numPointLights);

	// Allocate the number of point lights
	world.point_lights = (LIGHT*)malloc(world.numPointLights * sizeof(LIGHT));

	for(i = 0; i < world.numPointLights; i++)
	{
		// Load in the index
		fscanf(fptr, "%d", &world.point_lights[i].index);
		// Load in the position
		fscanf(fptr, "%f\t%f\t%f", &world.point_lights[i].position.i, &world.point_lights[i].position.j, &world.point_lights[i].position.k);
		// Load in the color
		fscanf(fptr, "%f\t%f\t%f", &world.point_lights[i].colour.r, &world.point_lights[i].colour.g, &world.point_lights[i].colour.b);
	}

	// Lights loaded in
	
	// Load in the area lights
	fscanf(fptr, "%d", &world.numAreaLights);

	// Allocate memory for the area lights
	world.area_lights = (AREA_LIGHT*)malloc(world.numAreaLights * sizeof(AREA_LIGHT));

	// Loop through the area lights and load them in
	for(i = 0; i < world.numAreaLights; i++)
	{
		// Load in the index
		fscanf(fptr, "%d", &world.area_lights[i].index);
		// Load in the first vertex
		fscanf(fptr, "%f\t%f\t%f", &world.area_lights[i].corners[0].i, &world.area_lights[i].corners[0].j, &world.area_lights[i].corners[0].k);
		// Load in the second vertex
		fscanf(fptr, "%f\t%f\t%f", &world.area_lights[i].corners[1].i, &world.area_lights[i].corners[1].j, &world.area_lights[i].corners[1].k);
		// Load in the third vertex
		fscanf(fptr, "%f\t%f\t%f", &world.area_lights[i].corners[2].i, &world.area_lights[i].corners[2].j, &world.area_lights[i].corners[2].k);
		// Load in the final vertex
		fscanf(fptr, "%f\t%f\t%f", &world.area_lights[i].corners[3].i, &world.area_lights[i].corners[3].j, &world.area_lights[i].corners[3].k);
		// Load in the color
		fscanf(fptr, "%f\t%f\t%f", &world.area_lights[i].colour.r, &world.area_lights[i].colour.g, &world.area_lights[i].colour.b);
	}
	return world;
}


TEXTURE* LoadTexture(const char* file)
{
	FILE *pfile;
	TGA* tgaFile;
	unsigned char tempColor;   // This will change the images from BGR to RGB.
	unsigned char uselessChar; // This will be used to hold char data we dont want.
	short int	  uselessInt;	// This will be used to hold int data we dont want.
	int colorMode;             // If this is 3 then its in RGB, 4 is RGBA.
	long tgaSize;		         // Image size.
	long index;                // Used in the for loop.

	TEXTURE* texture;

	texture = (TEXTURE*)malloc(sizeof(TEXTURE));

	tgaFile = (TGA*)malloc(sizeof(TGA));

	// Open the image and read in binary mode.
	pfile = fopen(file, "rb");
   // check if the file opened.
	if (!pfile)
		return 0;

	// Read in the two useless values.
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);

	// Read the image type, 2 is color, 4 is is greyscale.
	fread(&tgaFile->imageType, sizeof(unsigned char), 1, pfile);

	// We only want to be able to read in color or greyscale .tga's.
	if ((tgaFile->imageType != 2) && (tgaFile->imageType != 3))
	   {
		   fclose(pfile);
		   return 0;
	   }

	// Get rid of 13 bytes of useless data.
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);

	// Get the image width and height.
	fread(&tgaFile->width, sizeof(short int), 1, pfile);
	fread(&tgaFile->height, sizeof(short int), 1, pfile);

	texture->width = tgaFile->width;
	texture->height = tgaFile->height;

	printf("Opening texture %s.\n", file);
	printf("Width:\t%d\tHeight:\t%d\n", texture->width, texture->height);

	// Get the bit count.
	fread(&tgaFile->pixelDepth, sizeof(unsigned char), 1, pfile);

	// Get rid of 1 byte of useless data.
   fread(&uselessChar, sizeof(unsigned char), 1, pfile);

	// If the image is RGB then colorMode should be 3 and RGBA would
   // make colorMode equal to 4.  This will help in our loop when
   // we must swap the BGR(A) to RGB(A).
	colorMode = tgaFile->pixelDepth / 8;

   // Determine the size of the tga image.
	tgaSize = tgaFile->width * tgaFile->height * colorMode;
	// Allocate memory for the tga image.

	texture->pixels = (unsigned char*)malloc(sizeof(unsigned char) * tgaSize);



	// Read the image into imageData.
	fread(texture->pixels, sizeof(unsigned char), tgaSize, pfile);
	// This loop will swap the BGR(A) to RGB(A).
	for (index = 0; index < tgaSize; index += colorMode)
	{
		tempColor = texture->pixels[index];
		texture->pixels[index] = texture->pixels[index + 2];
		texture->pixels[index + 2] = tempColor;

	}

	// Close the file where your done.
	fclose(pfile);
   // return 1 to satisfy our if statement (load successful).
	return texture;
}




#endif