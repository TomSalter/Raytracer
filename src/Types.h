// Definition of all types across the project
#ifndef TYPES_H
#define TYPES_H

typedef struct
{
	float	i;
	float	j;
	float	k;
	float	w;
}VECTOR3;

typedef struct
{
	VECTOR3 position;
	VECTOR3 normal;
	float d;
}PLANE;

typedef struct
{
	VECTOR3	origin;
	VECTOR3 vector;
	float t;
	float current_refractive_index; // Refractive index of current medium
	int InObject; // Are we in an object?

}RAY;

typedef struct
{
	float	r;
	float	g;
	float	b;
	float	alpha;
}COLOUR;

typedef struct
{
	float u;
	float v;
}TEXCOORDS;

typedef struct
{
	VECTOR3	position;
	VECTOR3	normal;
	COLOUR	colour;
	TEXCOORDS tex;
}VERTEX3;

typedef struct
{
	long int width;
	long int height;
	float* heightdata;
}HEIGHTMAP;

typedef struct
{
	RAY photon_ray;		// Ray of the photon
	COLOUR	colour;		// Colour of the photon
	int	index;			// Index for the photon.  Increases after each bounce.  
	int used_flag;		// Flag to check if the photon has been used already.
}PHOTON;

typedef struct
{
	COLOUR colour;		// Colour to be stored in the irradiance map
}IRRADIANCE_ELEMENT;

typedef struct
{
	IRRADIANCE_ELEMENT**	ir_map;
	unsigned int			size;
}IRRADIANCE_MAP;
	
typedef struct
{
	int index[3];
	PHOTON* photon_list;	// List of photons that hit the surface
	int	num_photons;		// Number of photons that have hit the surface
	int alloc_space;		// Number of photon locations stored per surface
	IRRADIANCE_MAP	ir_map;	// Irradiance map for this surface
}POLYGON;	

typedef struct
{
	VECTOR3 smallest;
	VECTOR3 biggest;
}AXISALIGNEDBOUNDBOX;

typedef struct
{
	unsigned char* pixels;
	int		width;
	int		height;
}TEXTURE;

typedef struct
{
	VERTEX3*	vertices;			// Vertex buffer for mesh
	int			numVertices;		// Number of vertices in the buffer
	POLYGON*	polygons;			// Polygon buffer
	int			numPolys;			// Number of polygons in the mesh
	int			numNormals;			// Number of normals (these are placed in the VERTEX3 data structure)
	int			numTexCoords;		// Number of texture coordinates per mesh
	COLOUR		diffuse;			// Diffuse component for lighting
	COLOUR		specular;			// Specular component for lighting
	HEIGHTMAP	heightmap;			// Heightmap data can be loaded into here
	int			index;
	float		refractive_index;	// Refractive index of object.  
	float		transparency;		// The transparency of the object.
	float		reflectivity;		// Reflectivity of the object
	VECTOR3		translation;		// Translation vector
	VECTOR3		rotation;			// Rotation vector (degrees around i, j and k
	VECTOR3		scalar;				// Scalar vector (i, j and k scalar)
	AXISALIGNEDBOUNDBOX	aabb;		// Axis Aligned Bounding Box for this object
	TEXTURE*	texture;			// Pointer to a texture
}MESH;

typedef unsigned char byte;
typedef unsigned short word;

struct TGA
{
	byte idLength;
	byte colourMapType;
	byte imageType;
	word colourMapFirstEntryIndex;
	word colourMapLength;
	byte colourMapEntrySize;
	word xOrigin;
	word yOrigin;
	word width;
	word height;
	byte pixelDepth;
	byte imageDescriptor;
};



typedef struct
{
	VECTOR3			position;	// Light position
	COLOUR			colour;		// Light colour
	int				index;
}LIGHT;

typedef struct 
{
	VECTOR3			corners[4]; // 4 corners of the rectangular/square light
	float			increment;  // Increment for the monte-carlo sampling
	COLOUR			colour;
	int				index;
}AREA_LIGHT;

typedef struct
{
	VECTOR3			eyepoint;	// Eyepoint
	VECTOR3			Q;			// Top left corner of view plane
	VECTOR3			B;			// Botton left corner of view plane
	VECTOR3			S;			// Bottom right corner of view plane
	VECTOR3			C;			// Point we are looking at, to be worked out
	RAY				ray;		// For each pixel, we create a ray...
}CAMERA;

typedef struct
{
	MESH*			meshes;			// List of models
	int				numMeshes;		// Number of models
	LIGHT*			point_lights;	// Point Lights
	int				numPointLights; // Number of point lights
	AREA_LIGHT*		area_lights;	// Area Lights
	int				numAreaLights;	// Number of area lights
	CAMERA			camera;			// Camera
	COLOUR			ambient_light;	// Ambient Light
}WORLD;

struct INTERSECTION
{
	VECTOR3			position;		// Intersection position
	VECTOR3			normal;			// Normal at intersection
	COLOUR			colour;			// Colour at the intersection
	TEXCOORDS		tex;			// Texture Coordinates
	int				RecursiveDepth;	// Recursive Depth
	RAY				ReflectRay;		// Reflected Ray
	RAY				RefractRay;		// Refracted Ray
	MESH*			mesh_pointer;	// Give the intersect a pointer to a mesh object
	int				polygon_index;	// Index of the polygon hit in the mesh
	float			u;				// u barycentric coordinate
	float			v;				// v barycentric coordinate
	INTERSECTION*	Reflection;		// Reflected ray intersection
	INTERSECTION*	Refraction;		// Refracted ray intersection
};

typedef struct INTERSECTION INTERSECTION;

typedef struct 
{
	float mat[4][4];
}MATRIX4x4;





#endif