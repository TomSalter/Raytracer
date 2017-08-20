// File to load up a .x mesh into a MESH structure

#ifndef MESHLOADER_C
#define MESHLOADER_C

#include "MeshLoader.h"

// Function calculates an axis aligned bounding box around a mesh
AXISALIGNEDBOUNDBOX CalculateAxisAlignedBoundingBox(const MESH* mesh)
{
	AXISALIGNEDBOUNDBOX aabb;

	float smallest_x = 10000, smallest_y = 10000, smallest_z = 10000;
	float biggest_x = -99999, biggest_y = -99999, biggest_z = -99999;

	int i;

	for(i = 0; i < mesh->numVertices; i++)
	{
		if(mesh->vertices[i].position.i < smallest_x)
			smallest_x = mesh->vertices[i].position.i;
		if(mesh->vertices[i].position.j < smallest_y)
			smallest_y = mesh->vertices[i].position.j;
		if(mesh->vertices[i].position.k < smallest_z)
			smallest_z = mesh->vertices[i].position.k;
		if(mesh->vertices[i].position.i > biggest_x)
			biggest_x = mesh->vertices[i].position.i;
		if(mesh->vertices[i].position.j > biggest_y)
			biggest_y = mesh->vertices[i].position.j;
		if(mesh->vertices[i].position.k > biggest_z)
			biggest_z = mesh->vertices[i].position.k;
	}

	aabb.smallest = newVector3(smallest_x, smallest_y, smallest_z);
	aabb.biggest = newVector3(biggest_x, biggest_y, biggest_z);

	if(DEBUG)
	{
		printf("Axis Aligned Bounding Box Complete.\n");
		printf("Smallest = %lf %lf %lf.\n", smallest_x, smallest_y, smallest_z);
		printf("Biggest = %lf %lf %lf.\n", biggest_x, biggest_y, biggest_z);
	}

	return aabb;
}

// Written:  2002/2003
// Function takes in a file pointer, string, and size of the string and returns the file pointer to the next instance of that string
// Tested:  OK
// Improvements Available:  No
FILE* SearchFileForString(const char* string, int size, FILE* fptr)
{
	/*	We need a file pointer to refer back to, if the string is not correct	*/
	char* buffer;
	char ch;
	int i;
	buffer = (char*)malloc((size + 1) * sizeof(char));


	ch = fgetc(fptr);
	while (ch != EOF)
	{
		/*	Search for the first letter of the string	*/
		while((ch != EOF) && (ch != string[0]))
		{
			ch = fgetc(fptr);
		}		

		/*	Now we compare the next characters with the string	*/
		for(i = 0; i < size; i++)
		{	
			if(ch == EOF)
			{
				return NULL;
			}
			buffer[i] = ch;
			ch = fgetc(fptr);
		}
		buffer[size] = '\0';
		if(strcmp(buffer, string) == 0)
		{
			fseek(fptr, -1, SEEK_CUR);
			return fptr;	
		}
		else
		{
			/*	Go back to the point after the first letter of the string	*/
			fseek(fptr, -(size) + 1, SEEK_CUR);
		}
	}

	free(buffer);
	return NULL;
}


// Written: 2002/2003
// Function takes a filename and a heightmap pointer, mallocs and creates it
// Tested:  A-ok
// Improvements: none
int LoadHMP(char* file, HEIGHTMAP *hmp)
{
	FILE *pfile;
	unsigned char uselessChar;
	long int width;
	long int height;
	long int i;
	float temp;

	pfile = fopen(file, "rb");
	if(!pfile)
		return 0;

	printf("Opened %s...\n", file);

	/*	First we read in the width and height of the heightmap	*/
	fread(&width, sizeof(long),1 , pfile);
	printf("Width...%d\n", width);
	fread(&height, sizeof(long), 1, pfile);
	printf("Height...%d\n", height);

	hmp->width = width;
	hmp->height = height;

	/*	Now we read in the 92 bytes of nothingness	*/
	for(i = 0; i < 92; i++)
	{
		fread(&uselessChar, sizeof(char), 1, pfile);
	}

	/*	Now we are at the beginning of the data heightmap data	
	 *	We fill our structure with the casted heights.
	 */

	printf("Allocating heightmap memory...\n");
	hmp->heightdata = (float*)malloc(((width * height) + width) * sizeof(float));

	printf("Loading height data...\n");

	for(i = 0; i < ((width * height)); i++)
	{
		fread(&uselessChar, sizeof(char), 1, pfile);
		/*	Since each map item is a byte, we firstly convert to an int.
		 *	Then we divide by 255, and cast to a float
		 *	So we get a value between zero and one	*/
		temp = (float)((int)uselessChar) / 255;
		hmp->heightdata[i] = temp;
	}

	printf("Load complete..\n");
	fclose(pfile);
	return 1;
}

// Written:  2002/2003
// Function loads a MESH data structure with a .x file, also adds the heightmap.
// Tested:  OK apart from negative numbers
// Improvements available: Need to add negative number capabilities :: Attempted 02/11/2004
MESH LoadMeshFromFile(char* filename, char* heightmap)
{
	/*
	 *	Algorithm Overview
	 *	0.5)  open the file.
	 *	1) Create a new mesh object.  Find the number of vertices.  
	 *	2) Allocate the vertex array. Fill it.
	 *	3) Allocate the polygon array.  Fill it.
	 *	4) Allocate the normals.
	 *	5) Allocate the texture coordinates.
	 *	6) Allocate the materials
	 *	7) Return the mesh object, so it can be added to a list.
	 */
	FILE* fptr;
	MESH mesh;
	char numBuffer[60];
	char ch;
	int Nameflag = 0;
	int i, currentIndex, coordIndex;

	if((fptr = fopen(filename, "rb")))
	{
		/*	Stage One - find the number of vertices	*/

		/*	Search for the token "Mesh "	*/
		fptr = SearchFileForString("Mesh ", strlen("Mesh "), fptr);
		/*	We have now found the point with the mesh definition	*/
		SearchFileForString("{", strlen("{"), fptr);
		ch = fgetc(fptr);
		while(! ( (ch>=48 && ch <=57) || (ch == '-') ) )
		{
			ch = fgetc(fptr);
		}
		/*	Now we should be at the start of the number		*/
		i = 0;
		while((ch>=48 && ch <=57) || (ch == '-'))
		{
			numBuffer[i] = ch;
			ch = fgetc(fptr);
			i++;
		}
		numBuffer[i] = '\0';

		/*	Now we must convert the string into a char	*/
		mesh.numVertices = atoi(numBuffer);
		/*	Now the number of vertices is set, we can read in the vertices. */
		/*	Firstly we allocate the new memory for the vertices.	*/
		mesh.vertices = (VERTEX3*)malloc(mesh.numVertices * sizeof(VERTEX3));

		/*	Now advance to the start of the list	*/
		ch = fgetc(fptr);
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}
		/*	Pointer is now looking at the first character	*/
		for(currentIndex = 0; currentIndex < mesh.numVertices; currentIndex++)
		{
			for(coordIndex = 0; coordIndex < 3; coordIndex++)
			{
				i = 0;
				while(ch!=';')
				{
					numBuffer[i] = ch;
					ch = fgetc(fptr);
					i++;
				}	
				numBuffer[i] = '\0';
				ch = fgetc(fptr);
				if(coordIndex == 0)
					mesh.vertices[currentIndex].position.i = (float)atof(numBuffer);
				if(coordIndex == 1)
					mesh.vertices[currentIndex].position.j = (float)atof(numBuffer);
				if(coordIndex == 2)
					mesh.vertices[currentIndex].position.k = (float)atof(numBuffer);
			}
			mesh.vertices[currentIndex].position.w = 1;
			ch = fgetc(fptr);
		}

		/*	Now we have to scan to the number of polygon's in the mesh	*/
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}
		i=0;
		while((ch>=48 && ch <=57) || (ch == '-'))
		{
			numBuffer[i] = ch;
			ch = fgetc(fptr);
			i++;
		}

		/*	Now we can read in the number of polygons.	*/
		numBuffer[i] = '\0';
		mesh.numPolys = atoi(numBuffer);
		printf("Number of polygons.....\t %d\n", mesh.numPolys);
		/*	We now have the number of polygons, so we read the indexes in.	*/
		mesh.polygons = (POLYGON*)malloc(mesh.numPolys * sizeof(POLYGON));
		/*	Now we have to scan to the start of the list	*/
		for(currentIndex = 0; currentIndex < mesh.numPolys; currentIndex++)
		{
			/*	First characters are 3;	*/
				while (!(ch>=48 && ch <=57))
					ch = fgetc(fptr);

				while(ch>=48 && ch <=57)
				{
					ch = fgetc(fptr);
				}
				ch = fgetc(fptr);
			for(coordIndex = 0; coordIndex < 3; coordIndex++)
			{
				i = 0;
				while(ch != ',' && ch != ';')
				{
					numBuffer[i] = ch;
					ch = fgetc(fptr);
					i++;
				}	
				numBuffer[i] = '\0';  
				ch = fgetc(fptr);
				mesh.polygons[currentIndex].index[coordIndex] = atoi(numBuffer);
			}	
			// Allocate memory for 100 photons in the photon map per polygon
			mesh.polygons[currentIndex].alloc_space = 100;
			mesh.polygons[currentIndex].num_photons = 0;
			mesh.polygons[currentIndex].photon_list = (PHOTON*)malloc(mesh.polygons[currentIndex].alloc_space* sizeof(PHOTON));

			if(DEBUG)
				printf("Allocated %d photons for polygon %d.\n", mesh.polygons[currentIndex].alloc_space, currentIndex);
				
			("Vertex: %d loaded. \n", currentIndex);
		}
		/*	Now we have read in all of the Mesh Normals	*/
		SearchFileForString("MeshNormals ", strlen("MeshNormals "), fptr);
		SearchFileForString("{", 1, fptr);
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}
		i = 0;
		while(ch != ';')
		{
			numBuffer[i] = ch;
			ch = fgetc(fptr);
			i++;
		}
		numBuffer[i] = '\0';
		mesh.numNormals = atoi(numBuffer);
		/*	So now we have the number of normals, we read them all in.	*/
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}

		for(currentIndex = 0; currentIndex < mesh.numNormals; currentIndex++)
		{
			for(coordIndex = 0; coordIndex < 3; coordIndex++)
			{
				i = 0;
				while(ch!=';')
				{
					numBuffer[i] = ch;
					ch = fgetc(fptr);
					i++;
				}	
				numBuffer[i] = '\0';
				ch = fgetc(fptr);
				if(coordIndex == 0)
					mesh.vertices[currentIndex].normal.i = (float)atof(numBuffer);
				if(coordIndex == 1)
					mesh.vertices[currentIndex].normal.j = (float)atof(numBuffer);
				if(coordIndex == 2)
					mesh.vertices[currentIndex].normal.k = (float)atof(numBuffer);
			}
			ch = fgetc(fptr);
		}
		/*	Now we get to the textures coords	*/
		SearchFileForString("MeshTextureCoords ", strlen("MeshTextureCoords "), fptr);
		SearchFileForString("{", strlen("{"),fptr);
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}
		i = 0;
		while(ch != ';')
		{
			numBuffer[i] = ch;
			ch = fgetc(fptr);
			i++;
		}
		numBuffer[i] = '\0';
		mesh.numTexCoords = atoi(numBuffer);
		while(!((ch>=48 && ch <=57) || (ch=='-') || (ch=='.')))
		{
			ch = fgetc(fptr);
		}
		/*	We are now at the start of the tex coord data.	*/
		for(currentIndex = 0; currentIndex < mesh.numTexCoords; currentIndex++)
		{
			for(coordIndex = 0; coordIndex < 2; coordIndex++)
			{
				i = 0;
				while(ch!=';')
				{
					numBuffer[i] = ch;
					ch = fgetc(fptr);
					i++;
				}	

				numBuffer[i] = '\0';
				ch = fgetc(fptr);
				if(coordIndex == 0)
					mesh.vertices[currentIndex].tex.u = (float)atof(numBuffer);
				if(coordIndex == 1)
					mesh.vertices[currentIndex].tex.v = (float)atof(numBuffer);


			}

			ch = fgetc(fptr);

		}

		LoadHMP(heightmap, &mesh.heightmap);
		fclose(fptr);

		return mesh;
	}

	printf("Couldn't open file %s\nQuitting Application", filename);
	exit(EXIT_SUCCESS);
}



#endif