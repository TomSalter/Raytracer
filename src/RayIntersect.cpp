// Functions for calculation of intersection points between primitives and rays

#ifndef RAYINTERSECT_C
#define RAYINTERSECT_C

#include "RayIntersect.h"

/* Few #defines for Quick Intersect */

#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

// Written:  02/11/2004
// Returns the precise intersect point of a PLANE and a RAY
// Tested: OK
// Improvements available: change dot product to inline, but it is clearer this way
VECTOR3 CalculateRayPlaneIntersectPoint(PLANE plane, RAY *ray)
{
	VECTOR3 PointOnPlane;

	float VdotN;

	// Check if the plane is parallel to the vector
	VdotN = DotProductVector3(ray->vector, plane.normal);

	if(VdotN == 0.0)
	{
		// Plane is parallel to the vector, will never intersect
		PointOnPlane.i = -99999;
		PointOnPlane.j = -99999;
		PointOnPlane.k = -99999;

		ray->t = -99999;
	
		return PointOnPlane;
	}

	// Now the point actually does intersect the plane

//	plane.d = -( ( plane.normal.i * plane.position.i ) + ( plane.normal.j * plane.position.j ) + ( plane.normal.k * plane.position.k ) );
	plane.d = -DotProductVector3(plane.normal, plane.position);

	ray->t = (-( DotProductVector3(ray->origin, plane.normal) + plane.d )) / VdotN;

	// Now we have the t parameter value, we can work out the position on the plane

	PointOnPlane.i = ray->origin.i + (ray->vector.i * ray->t);
	PointOnPlane.j = ray->origin.j + (ray->vector.j * ray->t);
	PointOnPlane.k = ray->origin.k + (ray->vector.k * ray->t);

	return PointOnPlane;
}

// Written 14/11/2004
// Returns a the t-value if there's an intersect, a -1 if there's no intersect
// Tested:  Not yet
// Improvements Available: Not yet
float TriangleIntersectTestBarycentric(const VERTEX3* triangle, VECTOR3 point, RAY ray)
{
	float alpha, beta, gamma, total_area;
	VECTOR3 a_cdash, a_c, a_b, b_cdash, c_cdash;

	// Calculate edge vectors for working out areas.
	a_cdash.i = point.i - triangle[0].position.i;
	a_cdash.j = point.j - triangle[0].position.j;
	a_cdash.k = point.k - triangle[0].position.k;

	a_c.i = triangle[2].position.i - triangle[0].position.i;
	a_c.j = triangle[2].position.j - triangle[0].position.j;
	a_c.k = triangle[2].position.k - triangle[0].position.k;

	a_b.i = triangle[1].position.i - triangle[0].position.i;
	a_b.j = triangle[1].position.j - triangle[0].position.j;
	a_b.k = triangle[1].position.k - triangle[0].position.k;

	b_cdash.i = point.i - triangle[1].position.i;
	b_cdash.j = point.j - triangle[1].position.j;
	b_cdash.k = point.k - triangle[1].position.k;

	c_cdash.i = point.i - triangle[2].position.i;
	c_cdash.j = point.j - triangle[2].position.j;
	c_cdash.k = point.k - triangle[2].position.k;

	total_area = 0.5 * FindLengthVector3(a_b) * FindLengthVector3(a_c);

	alpha = (0.5 * FindLengthVector3(a_cdash) * FindLengthVector3(a_c)) / total_area;
	beta = (0.5 * FindLengthVector3(a_cdash) * FindLengthVector3(a_b)) / total_area;
	gamma = (0.5 * FindLengthVector3(b_cdash) * FindLengthVector3(c_cdash)) / total_area;

	// Now we have the barycentric coordinates of the intersection, we can work out whether it lies inside or outside of the triangle.

	if( (alpha == 0) && (beta > 0) && (gamma > 0) )
	{
		// We are inside the triangle
		return ray.t;
	}
	
	return -1;
}

void SwapVertices(VERTEX3 *point1, VERTEX3 *point2)
{
	VERTEX3 temp;

	temp = *point1;
	*point1 = *point2;
	*point2 = temp;
}

void SortVerticesVertAsc(VERTEX3* points)
{
	int swaps = 1;

	while(swaps != 0)
	{
		swaps = 0;
		if(points[0].position.j > points[1].position.j)
		{
			SwapVertices(&points[0], &points[1]);
			swaps++;
		}
		if(points[1].position.j > points[2].position.j)
		{
			SwapVertices(&points[1], &points[2]);
			swaps++;
		}
	}
}

VERTEX3 InterpolateVerticalVertex(VECTOR3 ref, VERTEX3 v1, VERTEX3 v2)
{
	VERTEX3 D;
	float temp;
	float quickval;

	quickval = v2.position.j - v1.position.j;

	if( quickval != 0.0 )
	{
		temp = (ref.j - v1.position.j) / quickval;
		
		D.position.i = v1.position.i + (temp * (v2.position.i - v1.position.i));
		D.position.j = ref.j;
		
		D.normal.i = v1.normal.i + (temp * (v2.normal.i - v1.normal.i));
		D.normal.j = v1.normal.j + (temp * (v2.normal.j - v1.normal.j));
		D.normal.k = v1.normal.k + (temp * (v2.normal.k - v1.normal.k));

		// Spherical Interpolate the normal
		D.normal = SphericalInterpolateVector3(v1.normal, v2.normal, temp);

		D.tex.u = v1.tex.u + (temp * (v2.tex.u - v1.tex.u));
		D.tex.v = v1.tex.v + (temp * (v2.tex.v - v1.tex.v));
	}
	else
	{
		D.position.i = v1.position.i;
		D.position.j = v1.position.j;
		D.normal = v1.normal;
		D.tex = v1.tex;
	}

	return D;
}

INTERSECTION ReturnIntersectionProjected(const VERTEX3* triangle, VECTOR3 point, RAY ray)
{
	VERTEX3 v[3];
	VECTOR3 normal;
	int i;

	VECTOR3 tangent1, tangent2;
	VERTEX3 left, right, final;
	VECTOR3 test_point;
	float temp, quickval;

	INTERSECTION isect;

	tangent1 = SubtractVector3(triangle[1].position, triangle[0].position);
	tangent2 = SubtractVector3(triangle[2].position, triangle[0].position);

	normal = CrossProductVector3(tangent1, tangent2);

	// Now we work out the largest component of the normal

	// If i is biggest...
	if( (floatabs(normal.i) >= floatabs(normal.j)) && (floatabs(normal.i) >= floatabs(normal.k)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i] = triangle[i];
			v[i].position.i = triangle[i].position.j;
			v[i].position.j = triangle[i].position.k;
			test_point.i = point.j;
			test_point.j = point.k;
		}
	}
	
	// If j is biggest...
	if( (floatabs(normal.j) >= floatabs(normal.i)) && (floatabs(normal.j) >= floatabs(normal.k)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i] = triangle[i];
			v[i].position.i = triangle[i].position.i;
			v[i].position.j = triangle[i].position.k;
			test_point.i = point.i;
			test_point.j = point.k;
		}
	}

	// If k is the biggest...
	if( (floatabs(normal.k) >= floatabs(normal.i)) && (floatabs(normal.k) >= floatabs(normal.j)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i] = triangle[i];
			v[i].position.i = triangle[i].position.i;
			v[i].position.j = triangle[i].position.j;
			test_point.i = point.i;
			test_point.j = point.j;
		}
	}

	// Now we have a triangle projected into an i-j plane.
	// Need to sort it in order vertically
	SortVerticesVertAsc(v);

	if( v[0].position.j == v[1].position.j )
	{
		if( v[0].position.i < v[1].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[2]);
			right = InterpolateVerticalVertex(test_point, v[1], v[2]);
		}
		if( v[0].position.i >= v[1].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[1], v[2]);
			right = InterpolateVerticalVertex(test_point, v[0], v[2]);
		}
	}
	else

	if( v[1].position.j == v[2].position.j )
	{
		if( v[1].position.i < v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[1]);
			right = InterpolateVerticalVertex(test_point, v[0], v[2]);
		}
		if( v[1].position.i >= v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[2]);
			right = InterpolateVerticalVertex(test_point, v[0], v[1]);
		}
	}
	else
	// Now points are sorted, we work out if the point is above v1 or not, and interpolate accordingly

	// If the point is below v1
	if( test_point.j <= v[1].position.j )
	{
		// If v1 is on the left...
		if( v[1].position.i <= v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[1]);
			right = InterpolateVerticalVertex(test_point, v[0], v[2]);
		}
		// If v1 is on the right...
		if( v[1].position.i > v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[2]);
			right = InterpolateVerticalVertex(test_point, v[0], v[1]);
		}

		// Now interpolate between Left and Right to get the Intersection details.
	}
	else
	if( test_point.j > v[1].position.j )
	{
		// If v1 is on the left...
		if( v[1].position.i <= v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[1], v[2]);
			right = InterpolateVerticalVertex(test_point, v[0], v[2]);
		}
		// If v1 is on the right...
		if( v[1].position.i > v[2].position.i )
		{
			left = InterpolateVerticalVertex(test_point, v[0], v[2]);
			right = InterpolateVerticalVertex(test_point, v[1], v[2]);
		}

		// Now interpolate between Left and Right to get the Intersection details.
	}

	quickval = right.position.i - left.position.i;

	if(quickval != 0.0)
	{	
		temp = (test_point.i - left.position.i) / quickval;

		final.normal = SphericalInterpolateVector3(left.normal, right.normal, temp); 

		final.tex.u = left.tex.u + (temp * (right.tex.u - left.tex.u));
		final.tex.v = left.tex.v + (temp * (right.tex.v - left.tex.v));
	}
	else
	{
		final.normal = left.normal;
		final.tex = left.tex;
	}
	NormaliseVector3(&final.normal);

	isect.position = point;
	isect.normal = final.normal;
	isect.tex = final.tex;

	return isect;
}

// Written 20/11/2004
// Returns an INTERSECTION event from a triangle.  Assume triangle definately hits by using TriangleIntersectTestBarycentric() first
// Tested: Not yet
// Improvements Available: Not yet
INTERSECTION ReturnIntersectionBarycentric(VERTEX3 triangle0, VERTEX3 triangle1, VERTEX3 triangle2, VECTOR3 point, RAY ray)
{
	float alpha, beta, gamma, total_area;
	INTERSECTION isect;

	VECTOR3 a_b, b_p, a_p;
	VECTOR3 b_c, c_p, a_c;

	// Calculate edge vectors for working out areas.
	a_b = SubtractVector3(triangle1.position, triangle0.position);
	b_p = SubtractVector3(point, triangle1.position);
	a_p = SubtractVector3(point, triangle0.position);
	b_c = SubtractVector3(triangle2.position, triangle1.position);
	c_p = SubtractVector3(point, triangle2.position);
	a_c = SubtractVector3(triangle2.position, triangle0.position);
	
	// Calculate alpha, beta and gamma based upon the 
	total_area = CalculateTriangleArea(a_b, b_c, a_c);

	alpha = CalculateTriangleArea(a_b, b_p, a_p) / total_area;
	beta = CalculateTriangleArea(b_c, c_p, b_p) / total_area;
	gamma = CalculateTriangleArea(a_c, a_p, c_p) / total_area;

	// Now we can use alpha, beta and gamma to interpolate the normals and colours and texture coords
	
	isect.normal.i =	alpha * triangle0.normal.i + 
						beta * triangle1.normal.i +
						gamma * triangle2.normal.i;
	isect.normal.j =	alpha * triangle0.normal.j +
						beta * triangle1.normal.j +
						gamma * triangle2.normal.j;
	isect.normal.k =	alpha * triangle0.normal.k +
						beta * triangle1.normal.k +
						gamma * triangle2.normal.k;


	NormaliseVector3(&isect.normal);

	isect.position.i = point.i;
	isect.position.j = point.j;
	isect.position.k = point.k;

	isect.tex.u =	alpha * triangle0.tex.u +
					beta * triangle1.tex.u +
					gamma * triangle2.tex.u;
	isect.tex.v =	alpha * triangle0.tex.v +
					beta * triangle1.tex.v +
					gamma * triangle2.tex.v;

	return isect;
}

// Written: 20/11/2004
// Returns a plane for a triangle
// Tested: Not yet
// Improvements Available: Not yet
PLANE CreatePlaneFromTriangle(VECTOR3 triangle1, VECTOR3 triangle2, VECTOR3 triangle3)
{
	// A plane is simply a normal and a point on plane
	PLANE plane;
	VECTOR3 v1, v2;

	plane.position.i = triangle1.i;
	plane.position.j = triangle1.j;
	plane.position.k = triangle1.k;

	v1.i = triangle2.i - triangle1.i;
	v1.j = triangle2.j - triangle1.j;
	v1.k = triangle2.k - triangle1.k;

	v2.i = triangle3.i - triangle1.i;
	v2.j = triangle3.j - triangle1.j;
	v2.k = triangle3.k - triangle1.k;

	plane.normal = CrossProductVector3(v1, v2);

	return plane;
}

void SwapPoints(VECTOR3 *point1, VECTOR3 *point2)
{
	VECTOR3 temp;

	temp = *point1;
	*point1 = *point2;
	*point2 = temp;
}

void SortPointsVertAsc(VECTOR3* points)
{
	int swaps = 1;

	while(swaps != 0)
	{
		swaps = 0;
		if(points[0].j > points[1].j)
		{
			SwapPoints(&points[0], &points[1]);
			swaps++;
		}
		if(points[1].j > points[2].j)
		{
			SwapPoints(&points[1], &points[2]);
			swaps++;
		}
	}
}

VECTOR3 InterpolateVertical(VECTOR3 ref, VECTOR3 v1, VECTOR3 v2)
{
	VECTOR3 D;
	float temp;
	float quickval;

	quickval = v2.j - v1.j;

	if( quickval != 0.0 )
	{
		temp = (ref.j - v1.j) / quickval;

		D.i = v1.i + (temp * (v2.i - v1.i));
		D.j = ref.j;
	}
	else
	{
		D.i = v1.i;
		D.j = v1.j;
	}

	return D;
}


int TriangleIntersectTestProjected(const VECTOR3* triangle, VECTOR3 point, RAY ray)
{
	VECTOR3 v[3];
	VECTOR3 normal;
	int i;

	VECTOR3 tangent1, tangent2;
	VECTOR3 left, right;
	VECTOR3 test_point;

	tangent1 = SubtractVector3(triangle[1], triangle[0]);
	tangent2 = SubtractVector3(triangle[2], triangle[0]);

	normal = CrossProductVector3(tangent1, tangent2);

	// Now we work out the largest component of the normal

	// If i is biggest...
	if( ( floatabs(normal.i) >= floatabs(normal.j)) && (floatabs(normal.i) >= floatabs(normal.k)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i].i = triangle[i].j;
			v[i].j = triangle[i].k;
			test_point.i = point.j;
			test_point.j = point.k;
		}
	}
	
	// If j is biggest...
	if( (floatabs(normal.j) >= floatabs(normal.i)) && (floatabs(normal.j) >= floatabs(normal.k)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i].i = triangle[i].i;
			v[i].j = triangle[i].k;
			test_point.i = point.i;
			test_point.j = point.k;
		}
	}

	// If k is the biggest...
	if( (floatabs(normal.k) >= floatabs(normal.i)) && (floatabs(normal.k) >= floatabs(normal.j)) )
	{
		for(i = 0; i < 3; i++)
		{
			v[i].i = triangle[i].i;
			v[i].j = triangle[i].j;
			test_point.i = point.i;
			test_point.j = point.j;
		}
	}

	// Now we have a triangle projected into an i-j plane.
	// Need to sort it in order vertically
	SortPointsVertAsc(v);

	// Do a quick rejection test
	if( (test_point.j < v[0].j - 0.000001) || (test_point.j > v[2].j + 0.000001) )
	{
		return 0;
	}

	// Now points are sorted, we work out if the point is above v1 or not, and interpolate accordingly

	// Accomodate for flat top and flat bottom triangles
	if( v[0].j == v[1].j )
	{
		if( v[0].i < v[1].i )
		{
			left = InterpolateVertical(test_point, v[0], v[2]);
			right = InterpolateVertical(test_point, v[1], v[2]);
		}
		if( v[0].i >= v[1].i )
		{
			left = InterpolateVertical(test_point, v[1], v[2]);
			right = InterpolateVertical(test_point, v[0], v[2]);
		}
	}
	else

	if( v[1].j == v[2].j )
	{
		if( v[1].i < v[2].i )
		{
			left = InterpolateVertical(test_point, v[0], v[1]);
			right = InterpolateVertical(test_point, v[0], v[2]);
		}
		if( v[1].i >= v[2].i )
		{
			left = InterpolateVertical(test_point, v[0], v[2]);
			right = InterpolateVertical(test_point, v[0], v[1]);
		}
	}
	else

	// If the point is below v1
	if( test_point.j < v[1].j )
	{
		// If v1 is on the left...
		if( v[1].i <= v[2].i )
		{
			left = InterpolateVertical(test_point, v[0], v[1]);
			right = InterpolateVertical(test_point, v[0], v[2]);
		}
		// If v1 is on the right...
		if( v[1].i > v[2].i )
		{
			left = InterpolateVertical(test_point, v[0], v[2]);
			right = InterpolateVertical(test_point, v[0], v[1]);
		}
	}
	else

	if( test_point.j > v[1].j )
	{
		// If v1 is on the left...
		if( v[1].i <= v[2].i )
		{
			left = InterpolateVertical(test_point, v[1], v[2]);
			right = InterpolateVertical(test_point, v[0], v[2]);
		}
		// If v1 is on the right...
		else if( v[1].i >= v[2].i )
		{
			left = InterpolateVertical(test_point, v[0], v[2]);
			right = InterpolateVertical(test_point, v[1], v[2]);
		}
	}

	// 	Check against the left and right vectors
	if( (test_point.i <= right.i + 0.000001) && (test_point.i >= left.i - 0.000001) )
	{
		return 1;
	}

	return 0;
}

// Written:  02/11/2004
// Returns a zero if there is no intersect, returns a one is there is.
// Tested:  OK  
// Improvements available: Yes, do geometric calculations
int TriangleIntersectTestAlgebraic(const VECTOR3* triangle, VECTOR3 point, RAY ray)
{
	// Do Barycentric Calculations
	float alpha, beta, gamma, total_area;
	float total;
	VECTOR3 a_b, b_p, a_p;
	VECTOR3 b_c, c_p, a_c;

	// Calculate edge vectors for working out areas.
	a_b = SubtractVector3(triangle[1], triangle[0]);
	b_p = SubtractVector3(point, triangle[1]);
	a_p = SubtractVector3(point, triangle[0]);
	b_c = SubtractVector3(triangle[2], triangle[1]);
	c_p = SubtractVector3(point, triangle[2]);
	a_c = SubtractVector3(triangle[2], triangle[0]);
	
	// Calculate alpha, beta and gamma based upon the total area of the triangle.
	total_area = CalculateTriangleArea(a_b, b_c, a_c);

	alpha = CalculateTriangleArea(a_b, b_p, a_p); 
	beta = CalculateTriangleArea(b_c, c_p, b_p);
	gamma = CalculateTriangleArea(a_c, a_p, c_p);
	
	alpha = alpha;
	beta = beta;
	gamma = gamma;

	total = alpha + beta + gamma;

	// If this total is bigger than one, the point lies outside of the triangle
	if( total > total_area + 0.00001 )
	{
		return 0;
	}

	return 1;
}

int QuickRejectionTest(VECTOR3 Point, VECTOR3* triangle)
{
	float small_i = 1000000000, small_j = 1000000000, small_k = 1000000000;
	float big_i = -1000000000, big_j = -1000000000, big_k = -1000000000;
	int i;

	for(i = 0; i < 3; i++)
	{
		if(triangle[i].i < small_i)
		{
			small_i = triangle[i].i;
		}
		if(triangle[i].j < small_j)
		{
			small_j = triangle[i].j;
		}
		if(triangle[i].k < small_k)
		{
			small_k = triangle[i].k;
		}
		if(triangle[i].i > big_i)
		{
			big_i = triangle[i].i;
		}
		if(triangle[i].j > big_j)
		{
			big_j = triangle[i].j;
		}
		if(triangle[i].k > big_k)
		{
			big_k = triangle[i].k;
		}
	}

	// Now do the test

	if( ( Point.i <= small_i ) && (Point.i >= big_i) && ( Point.j <= small_j ) && ( Point.j >= big_j ) && ( Point.k <= small_k ) && ( Point.k >= big_k ) )
	{
		return 0;
	}

	return 1;
}



float CheckIfIntersectionProjected(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	PLANE plane;
	VECTOR3 PointOnPlane;
	int test;
	VECTOR3 triangle[3];

	plane = CreatePlaneFromTriangle(v1.position, v2.position, v3.position);

	PointOnPlane = CalculateRayPlaneIntersectPoint(plane, &ray);

	// If the ray is parallel to the plane
	if((PointOnPlane.i == -99999) && (PointOnPlane.j == -99999) && (PointOnPlane.j == -99999) && (ray.t == -99999))
	{
		return 100000;
	}

	// Now we have our intersection with the plane, we can check if it lies in the triangle or not
	triangle[0] = v1.position;
	triangle[1] = v2.position;
	triangle[2] = v3.position;

	if( !QuickRejectionTest(PointOnPlane, triangle) )
	{
		return 100000;
	}

	test = TriangleIntersectTestProjected(triangle, PointOnPlane, ray);

	if(test == 1)
	{
		// Ray intersects.  Return t value of the intersection
		return ray.t;
	}

	return 100000;
}

 
	
	
float CheckIfIntersection(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	PLANE plane;
	VECTOR3 PointOnPlane;
	int test;
	VECTOR3 triangle[3];

	plane = CreatePlaneFromTriangle(v1.position, v2.position, v3.position);

	PointOnPlane = CalculateRayPlaneIntersectPoint(plane, &ray);

	// If the ray is parallel to the plane
	if((PointOnPlane.i == -99999) && (PointOnPlane.j == -99999) && (PointOnPlane.j == -99999) && (ray.t == -99999))
	{
		return 100000;
	}

	

	// Now we have our intersection with the plane, we can check if it lies in the triangle or not
	triangle[0] = v1.position;
	triangle[1] = v2.position;
	triangle[2] = v3.position;

	if( !QuickRejectionTest(PointOnPlane, triangle) )
	{
		return 100000;
	}

	test = TriangleIntersectTestAlgebraic(triangle, PointOnPlane, ray);

	if(test == 1)
	{
		// Ray intersects.  Return t value of the intersection
		return ray.t;
	}

	return 100000;
}

INTERSECTION CalculateRayTriangleIntersection(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	PLANE plane;
	VECTOR3 PointOnPlane;
	int test;
	INTERSECTION isect;
	VECTOR3 triangle[3];

	plane = CreatePlaneFromTriangle(v1.position, v2.position, v3.position);

	PointOnPlane = CalculateRayPlaneIntersectPoint(plane, &ray);

	// float check that the ray-intersection calculation occurs
	if(DEBUG)
	{
		triangle[0] = v1.position;
		triangle[1] = v2.position;
		triangle[2] = v3.position;
		test = TriangleIntersectTestAlgebraic(triangle, PointOnPlane, ray);
		if(test != 1)
		{
			ReportError(RAY_INTERSECT_ERROR, "Warning");
		}
	}

	// Now we have the point on the plane inside the triangle. 
	// We can interpolate the details now so we have an intersection point

	isect = ReturnIntersectionBarycentric(v1,v2, v3, PointOnPlane, ray);

	return isect;
}

INTERSECTION CalculateRayTriangleIntersectionProjected(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	PLANE plane;
	VECTOR3 PointOnPlane;
	INTERSECTION isect;
	VERTEX3 triangle[3];

	plane = CreatePlaneFromTriangle(v1.position, v2.position, v3.position);
	PointOnPlane = CalculateRayPlaneIntersectPoint(plane, &ray);

	triangle[0] = v1;
	triangle[1] = v2;
	triangle[2] = v3;

	isect = ReturnIntersectionProjected(triangle, PointOnPlane, ray);

	return isect;
}

int	CheckAxisAlignedBoundingBoxIntersection(AXISALIGNEDBOUNDBOX aabb, RAY ray)
{
	float ddx, ddy, ddz;
	float txmin, txmax, tymin, tymax, tzmin, tzmax;

	float ray_dest_x = 100000 * ray.vector.i, ray_dest_y = 100000 * ray.vector.j, ray_dest_z = 100000 * ray.vector.k;

	ddx = 1.0 / (ray.origin.i - ray_dest_x);
	ddy = 1.0 / (ray.origin.j - ray_dest_y);

	
	if(ddx >= 0)
	{
		txmin = (aabb.smallest.i - ray.origin.i) * ddx;
		txmax = (aabb.biggest.i - ray.origin.i) * ddx;
	}
	else
	{
		txmin = (aabb.biggest.i - ray.origin.i) * ddx;
		txmax = (aabb.smallest.i - ray.origin.i) * ddx;
	}

	if(ddy >= 0)
	{
		tymin = (aabb.smallest.j - ray.origin.j) * ddy;
		tymax = (aabb.biggest.j - ray.origin.j) * ddy;
	}
	else
	{
		tymin = (aabb.biggest.j - ray.origin.j) * ddy;
		tymax = (aabb.smallest.j - ray.origin.j) * ddy;
	}

	if( (txmin > tymax) || (tymin > txmax) )
		return 0;

	if( tymin > txmin )
		txmin = tymin;

	if( tymax < txmax )
		txmax = tymax;

	ddz = 1.0 / (ray.origin.k - ray_dest_z);

	if( ddz >= 0 )
	{
		tzmin = (aabb.smallest.k - ray.origin.k) * ddz;
		tzmax = (aabb.biggest.k - ray.origin.k) * ddz;
	}
	else
	{
		tzmin = (aabb.biggest.k - ray.origin.k) * ddz;
		tzmax = (aabb.smallest.k - ray.origin.k) * ddz;
	}

	if( (txmin > tzmax) || (tzmin > txmax) )
		return 0;

	return 1;
}

INTERSECTION CalculateRayTriangleIntersectionQuick(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	// Origin of ray, direction of ray
	float orig[3], dir[3];
	// Vertices
	float vert0[3], vert1[3], vert2[3];
	// Calculate U and V and T;
	float t, u, v; 

	INTERSECTION isect;

	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det, gamma;

	// Set up equations accordingly...
	orig[0] = ray.origin.i;
	orig[1] = ray.origin.j;
	orig[2] = ray.origin.k;

	dir[0] = ray.vector.i;
	dir[1] = ray.vector.j;
	dir[2] = ray.vector.k;

	vert0[0] = v1.position.i;
	vert0[1] = v1.position.j;
	vert0[2] = v1.position.k;

	vert1[0] = v2.position.i;
	vert1[1] = v2.position.j;
	vert1[2] = v2.position.k;

	vert2[0] = v3.position.i;
	vert2[1] = v3.position.j;
	vert2[2] = v3.position.k;

	/* find vectors for two edges sharing vert0 */
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);

	/* begin calculating determinant - also used to calculate U parameter */
	CROSS(pvec, dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = DOT(edge1, pvec);

	if (det > EPSILON)
	{
		/* calculate distance from vert0 to ray origin */
		SUB(tvec, orig, vert0);
      
		/* calculate U parameter and test bounds */
		u = DOT(tvec, pvec);
      
		/* prepare to test V parameter */
		CROSS(qvec, tvec, edge1);
      
		/* calculate V parameter and test bounds */
		v = DOT(dir, qvec);
      
	   }
	else if(det < -EPSILON)
	{
		/* calculate distance from vert0 to ray origin */
		SUB(tvec, orig, vert0);
      
		/* calculate U parameter and test bounds */
		u = DOT(tvec, pvec);
		/*      printf("*u=%f\n",(float)*u); */
		/*      printf("det=%f\n",det); */
      
		/* prepare to test V parameter */
		CROSS(qvec, tvec, edge1);
      
		/* calculate V parameter and test bounds */
		v = DOT(dir, qvec) ;
	}

	inv_det = 1.0 / det;

	/* calculate t, ray intersects triangle */
	t = DOT(edge2, qvec) * inv_det;
	(u) *= inv_det;
	(v) *= inv_det;

	// Now we have U and V find gamma

	gamma = 1.0 - (u + v);

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

	NormaliseVector3(&isect.normal);

	isect.u = u;
	isect.v = v;

	return isect;
}

float CheckIfIntersectionQuick(RAY ray, VERTEX3 v1, VERTEX3 v2, VERTEX3 v3)
{
	// Origin of ray, direction of ray
	float orig[3], dir[3];
	// Vertices
	float vert0[3], vert1[3], vert2[3];
	// Calculate U and V and T;
	float t, u, v; 

	// Function specific values
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;

	// Set up equations accordingly...
	orig[0] = ray.origin.i;
	orig[1] = ray.origin.j;
	orig[2] = ray.origin.k;

	dir[0] = ray.vector.i;
	dir[1] = ray.vector.j;
	dir[2] = ray.vector.k;

	vert0[0] = v1.position.i;
	vert0[1] = v1.position.j;
	vert0[2] = v1.position.k;

	vert1[0] = v2.position.i;
	vert1[1] = v2.position.j;
	vert1[2] = v2.position.k;

	vert2[0] = v3.position.i;
	vert2[1] = v3.position.j;
	vert2[2] = v3.position.k;

	 /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

   if (det > EPSILON)
   {
      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      
      /* calculate U parameter and test bounds */
      u = DOT(tvec, pvec);
      if (u < 0.0 || u > det)
	 return 100000;
      
      /* prepare to test V parameter */
      CROSS(qvec, tvec, edge1);
      
      /* calculate V parameter and test bounds */
      v = DOT(dir, qvec);
      if (v < 0.0 || u + v > det)
	 return 100000;
      
   }
   else if(det < -EPSILON)
   {
      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      
      /* calculate U parameter and test bounds */
      u = DOT(tvec, pvec);
/*      printf("*u=%f\n",(float)*u); */
/*      printf("det=%f\n",det); */
      if (u > 0.0 || u < det)
	 return 100000;
      
      /* prepare to test V parameter */
      CROSS(qvec, tvec, edge1);
      
      /* calculate V parameter and test bounds */
      v = DOT(dir, qvec) ;
      if (v > 0.0 || u + v < det)
	 return 100000;
   }
   else return 100000;  /* ray is parallell to the plane of the triangle */


   inv_det = 1.0 / det;

   /* calculate t, ray intersects triangle */
	t = DOT(edge2, qvec) * inv_det;

	ray.t = t;
	return t;
}








#endif
