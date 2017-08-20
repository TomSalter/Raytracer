#ifndef BUMPMAPPING_C
#define BUMPMAPPING_C

#include "Bumpmapping.h"

float FindHeightValue(TEXCOORDS tex, HEIGHTMAP* hmp)
{
	float height;
	int newU, newV;

	newU = (int)tex.u * hmp->width;
	newV = (int)tex.v * hmp->height;

	height = hmp->heightdata[(hmp->width * newU)];

	return height;
}

VECTOR3 FindHeightMapNormal(TEXCOORDS tex, VECTOR3 normal, HEIGHTMAP *hmp)
{
	VECTOR3 u_vector;
	VECTOR3 v_vector;
	VECTOR3 normal_vector;
	MATRIX4x4 tangent_to_world;
	float heights[4];

	/*	Since the heightmap needs to be looked up using ints,
	 *	We convert the old tex coords to ints.
	 */
	int newU, newV;

	if(tex.u < 0.0)
		tex.u = 0.0;
	if(tex.u > 1.0)
		tex.u = 1.0;
	if(tex.v < 0.0)
		tex.v = 0.0;
	if(tex.v > 1.0)
		tex.v = 1.0;

	newU = (int)(tex.u * (float)hmp->width);
	newV = (int)(tex.v * (float)hmp->height);
	if(!((newU == 0) || (newU == hmp->width) || (newV == 0) || (newV == hmp->height)))
	{
		/*	We have to work out the heights at the adjacent pixels	*/
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	/*	If the current point is at the left of the bump-map	
	 *	Sample the normal from the current position, and the surrounding ones
	 */
	if((newU == 0) && !((newV == 0) || (newV == hmp->height)))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU];	
	}

	/*	If it at the right of the bump-map, then sample using correct heights */
	if((newU == hmp->width) && ((newV > 0) && (newV < hmp->height)))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	/*	If it is at the bottom of the bumpmap, sample using correct heights	*/
	if(!((newU != 0) || (newU != hmp->width)) && (newV == 0))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	/*	If it is at the top, sample using correct heights	*/

	if(((newU != 0) || (newU != hmp->width)) && (newV == hmp->height))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	/*	Bottom left corner	*/

	if((newU == 0) && (newV == 0))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU];	
	}

	/*	Bottom right corner	*/

	if((newU == hmp->width) && (newV == 0))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU + hmp->width];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	/*	Top left corner	*/

	if((newU == 0.0f) && (newV == hmp->height))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU + 1];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU];	
	}

	/*	Top right corner	*/

	if((newU == hmp->width) && (newV == hmp->height))
	{
		heights[0] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[1] = hmp->heightdata[(hmp->width * newV) + newU - hmp->width];
		heights[2] = hmp->heightdata[(hmp->width * newV) + newU];
		heights[3] = hmp->heightdata[(hmp->width * newV) + newU - 1];	
	}

	u_vector.i = 0;
	u_vector.k = heights[0] - heights[1];
	u_vector.k *= BUMPMAP_FACTOR;
	u_vector.j = 2;	
	v_vector.i = 2;
	v_vector.k = heights[2] - heights[3];
	v_vector.k *= BUMPMAP_FACTOR;
	v_vector.j = 0;
	NormaliseVector3(&u_vector);
	NormaliseVector3(&v_vector);
	normal_vector = CrossProductVector3(v_vector, u_vector);
	NormaliseVector3(&normal_vector);

	/*	We now have a normal in tangent space. Now transform from tangent to world.	*/

	newMatrix4x4(&tangent_to_world);

	tangent_to_world.mat[0][0] = u_vector.i;
	tangent_to_world.mat[0][1] = u_vector.j;
	tangent_to_world.mat[0][2] = u_vector.k;

	tangent_to_world.mat[1][0] = v_vector.i;
	tangent_to_world.mat[1][1] = v_vector.j;
	tangent_to_world.mat[1][2] = v_vector.k;

	tangent_to_world.mat[2][0] = normal.i;
	tangent_to_world.mat[2][1] = normal.j;
	tangent_to_world.mat[2][2] = normal.k;

	/*	Transform the normal into from tangent space to world	*/
	normal_vector = MultiplyVectorWithMatrix4(normal_vector, tangent_to_world);

	return normal_vector;
}		

COLOUR ReturnColourFromTextureMap(TEXCOORDS tex, TEXTURE* texture)
{
	int exact_u, exact_v;
	COLOUR colour;

	// Work out the texel which we will sample from in the texture.
	if(tex.u < 0.0)
		tex.u = 0.0;
	if(tex.u > 1.0)
		tex.u = 1.0;
	if(tex.v < 0.0)
		tex.v = 0.0;
	if(tex.v > 1.0)
		tex.v = 1.0;

	exact_u = int( tex.u * texture->width );
	
	exact_v = int( (1 - tex.v) * texture->height );

	// Ensure that they are within bounds...

	if( exact_u > texture->width )
		exact_u = texture->width;
	
	if( exact_v > texture->height )
		exact_v = texture->height;

	// Now we extrapolate the colour data from the texture map

	colour.r = (float)texture->pixels[( ( (exact_v * texture->width) + exact_u) * 3)] / 255;
	colour.g = (float)texture->pixels[( ( (exact_v * texture->width) + exact_u) * 3) + 1] / 255;
	colour.b = (float)texture->pixels[( ( (exact_v * texture->width) + exact_u) * 3) + 2] / 255;

	return colour;
}

#endif
