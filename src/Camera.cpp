#ifndef CAMERA_C
#define CAMERA_C

#include "Camera.h"

// Written 09/01/2004
// Populates a CAMERA object with the C vertex and the ray given from the eyepoint to the screen
// Tested: Yes
// Improvements Available: Z-value not calculated correctly.  Therefore only works for some values of Q, B and S.  NEED TO FIX
void ReturnInitialRayFromCamera(CAMERA *cam, int x, int y)
{
	float t;

	VECTOR3 xdash;
	VECTOR3 ydash;

	// Calculate xdash
	t = (float)x / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = (float)y / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	cam->C.i = cam->B.i + xdash.i + ydash.i;
	cam->C.j = cam->B.j + xdash.j + ydash.j;
	cam->C.k = cam->B.k + xdash.k + ydash.k;

	cam->ray.origin.i = cam->eyepoint.i;
	cam->ray.origin.j = cam->eyepoint.j;
	cam->ray.origin.k = cam->eyepoint.k;

	cam->ray.vector.i = cam->C.i - cam->eyepoint.i;
	cam->ray.vector.j = cam->C.j - cam->eyepoint.j;
	cam->ray.vector.k = cam->C.k - cam->eyepoint.k;

	NormaliseVector3(&cam->ray.vector);
	cam->ray.current_refractive_index = REFRACTIVE_INDEX_AIR;
}

void ReturnRandomInitialRayFromCamera(CAMERA* cam, int x, int y)
{
	float t;

	VECTOR3 xdash;
	VECTOR3 ydash;

	float x_extra, y_extra;

	x_extra = rand() / RAND_MAX;
	y_extra = rand() / RAND_MAX;

	// Calculate xdash
	t = ( ((float)x - 0.5) + x_extra) / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = ( ((float)y - 0.5) + y_extra) / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	cam->C.i = cam->B.i + xdash.i + ydash.i;
	cam->C.j = cam->B.j + xdash.j + ydash.j;
	cam->C.k = cam->B.k + xdash.k + ydash.k;

	cam->ray.origin.i = cam->eyepoint.i;
	cam->ray.origin.j = cam->eyepoint.j;
	cam->ray.origin.k = cam->eyepoint.k;

	cam->ray.vector.i = cam->C.i - cam->eyepoint.i;
	cam->ray.vector.j = cam->C.j - cam->eyepoint.j;
	cam->ray.vector.k = cam->C.k - cam->eyepoint.k;

	NormaliseVector3(&cam->ray.vector);
	cam->ray.current_refractive_index = REFRACTIVE_INDEX_AIR;
}

void ReturnJitteredRayFromCamera(CAMERA* cam, int x, int y, int i, int number_of_subpixels)
{
	float t;

	VECTOR3 xdash, ydash;
	VECTOR3 Qdash, Sdash;
	VECTOR3 Pq, Pb, Ps;
	VECTOR3 left_corner, right_corner;

	t = (float)x / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = (float)y / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	cam->C.i = cam->B.i + xdash.i + ydash.i;
	cam->C.j = cam->B.j + xdash.j + ydash.j;
	cam->C.k = cam->B.k + xdash.k + ydash.k;

	// Now we have C, we can work out Q' and S'
	// S'

	t = (float)(x + 1.0) / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = (float)y / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	Sdash.i = cam->B.i + xdash.i + ydash.i;
	Sdash.j = cam->B.j + xdash.j + ydash.j;
	Sdash.k = cam->B.k + xdash.k + ydash.k;

	//Q'

	t = (float)x / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = (float)(y + 1) / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	Qdash.i = cam->B.i + xdash.i + ydash.i;
	Qdash.j = cam->B.j + xdash.j + ydash.j;
	Qdash.k = cam->B.k + xdash.k + ydash.k;

	// Now we have C, Q' and S', we can work out left_corner and right_corner, Ps and Pb

	// Left corner
	t = (i % (number_of_subpixels - 1)) / (number_of_subpixels - 1);

	left_corner.i = cam->C.i + (t * (Sdash.i - cam->C.i));
	left_corner.j = cam->C.j + (t * (Sdash.j - cam->C.j));
	left_corner.k = cam->C.k + (t * (Sdash.k - cam->C.k));

	left_corner.i = left_corner.i - cam->C.i;
	left_corner.j = left_corner.j - cam->C.j;
	left_corner.k = left_corner.k - cam->C.k;

		// Right Corner
	t = (i / (number_of_subpixels - 1)) / (number_of_subpixels - 1);

	right_corner.i = cam->C.i = (t * (Qdash.i - cam->C.i));
	right_corner.j = cam->C.j = (t * (Qdash.j - cam->C.j));
	right_corner.k = cam->C.k = (t * (Qdash.k - cam->C.k));

	right_corner.i = right_corner.i - cam->C.i;
	right_corner.j = right_corner.j - cam->C.j;
	right_corner.k = right_corner.k - cam->C.k;

	// Now we can calculate Pb, the bottom left corner of the jittered pixel

	Pb.i = cam->C.i + left_corner.i + right_corner.i;
	Pb.j = cam->C.j + left_corner.j + right_corner.j;
	Pb.k = cam->C.k + left_corner.k + right_corner.k;

	// Ps
	t = ( (i + 1) % (number_of_subpixels - 1)) / (number_of_subpixels - 1);

	Ps.i = cam->C.i + (t * (Sdash.i - cam->C.i));
	Ps.j = cam->C.j + (t * (Sdash.j - cam->C.j));
	Ps.k = cam->C.k + (t * (Sdash.k - cam->C.k));

	Ps.i = Ps.i - Pb.i;
	Ps.j = Ps.j - Pb.j;
	Ps.k = Ps.k - Pb.k;



	// Pq
	t = ( (i + 1) / (number_of_subpixels - 1)) / (number_of_subpixels - 1);

	Pq.i = cam->C.i + (t * (Qdash.i - cam->C.i));
	Pq.j = cam->C.j + (t * (Qdash.j - cam->C.j));
	Pq.k = cam->C.k + (t * (Qdash.k - cam->C.k));

	Pq.i = Pq.i - Pb.i;
	Pq.j = Pq.j - Pb.j;
	Pq.k = Pq.k - Pb.k;

	// Now we have Pq, Pb and Ps of the pixel, we can randomly interpolate across it.

	// Need to re-use left_corner and right_corner for this....then form a final position from them two, after they
	// have been interpolated between PqPb, and PbPs

	t = rand()/RAND_MAX;

	left_corner.i = Pb.i + (t * (Ps.i - Pb.i));
	left_corner.j = Pb.j + (t * (Ps.j - Pb.j));
	left_corner.k = Pb.k + (t * (Ps.k - Pb.k));

	left_corner.i = left_corner.i - Pb.i;
	left_corner.j = left_corner.j - Pb.j;
	left_corner.k = left_corner.k - Pb.k;


	t = rand()/RAND_MAX;

	right_corner.i = Pb.i + (t * (Pq.i - Pb.i));
	right_corner.j = Pb.j + (t * (Pq.j - Pb.j));
	right_corner.k = Pb.k + (t * (Pq.k - Pb.k));

	right_corner.i = right_corner.i - Pb.i;
	right_corner.j = right_corner.j - Pb.j;
	right_corner.k = right_corner.k - Pb.k;

	// Now the final position is...

	cam->C.i = Pb.i + right_corner.i + left_corner.i;
	cam->C.j = Pb.j + right_corner.j + left_corner.j;
	cam->C.k = Pb.k + right_corner.k + left_corner.k;

	cam->ray.origin.i = cam->eyepoint.i;
	cam->ray.origin.j = cam->eyepoint.j;
	cam->ray.origin.k = cam->eyepoint.k;

	cam->ray.vector.i = cam->C.i - cam->eyepoint.i;
	cam->ray.vector.j = cam->C.j - cam->eyepoint.j;
	cam->ray.vector.k = cam->C.k - cam->eyepoint.k;

	NormaliseVector3(&cam->ray.vector);
	cam->ray.current_refractive_index = REFRACTIVE_INDEX_AIR;

}





/*

void ReturnJitteredRayFromCamera(CAMERA *cam, int x, int y, int i, int number_of_subpixels)
{
	float t;

	VECTOR3 xdash;
	VECTOR3 ydash;

	float length_of_pixel_subunit;
	float jitter_shift_x, jitter_shift_y;
	float Ox, Oy;

	float newX, newY;

	length_of_pixel_subunit = 1 / (float)number_of_subpixels;
	
	jitter_shift_x = ( (rand() / RAND_MAX) / (length_of_pixel_subunit / 2)) - 0.5 * length_of_pixel_subunit;
	jitter_shift_y = ( (rand() / RAND_MAX) / (length_of_pixel_subunit / 2)) - 0.5 * length_of_pixel_subunit;

	Ox = x - (0.5 * sqrt(number_of_subpixels) * length_of_pixel_subunit);
	newX = Ox + (i / sqrt(number_of_subpixels) ) + 0.5 * length_of_pixel_subunit;
	newX += jitter_shift_x;

	Oy = y - (0.5 * sqrt(number_of_subpixels) * length_of_pixel_subunit);
	newY = Oy - ((i / sqrt(number_of_subpixels) ) + 0.5 * length_of_pixel_subunit);
	newY += jitter_shift_y;


	// Calculate xdash
	t = newX / WIDTH;

	xdash.i = cam->B.i + (t * (cam->S.i - cam->B.i));
	xdash.j = cam->B.j + (t * (cam->S.j - cam->B.j));
	xdash.k = cam->B.k + (t * (cam->S.k - cam->B.k));

	xdash.i = xdash.i - cam->B.i;
	xdash.j = xdash.j - cam->B.j;
	xdash.k = xdash.k - cam->B.k;

	// Calculate ydash

	t = newY / HEIGHT;

	ydash.i = cam->B.i + (t * (cam->Q.i - cam->B.i));
	ydash.j = cam->B.j + (t * (cam->Q.j - cam->B.j));
	ydash.k = cam->B.k + (t * (cam->Q.k - cam->B.k));

	ydash.i = ydash.i - cam->B.i;
	ydash.j = ydash.j - cam->B.j;
	ydash.k = ydash.k - cam->B.k;

	cam->C.i = cam->B.i + xdash.i + ydash.i;
	cam->C.j = cam->B.j + xdash.j + ydash.j;
	cam->C.k = cam->B.k + xdash.k + ydash.k;

	cam->ray.origin.i = cam->eyepoint.i;
	cam->ray.origin.j = cam->eyepoint.j;
	cam->ray.origin.k = cam->eyepoint.k;

	cam->ray.vector.i = cam->C.i - cam->eyepoint.i;
	cam->ray.vector.j = cam->C.j - cam->eyepoint.j;
	cam->ray.vector.k = cam->C.k - cam->eyepoint.k;

	NormaliseVector3(&cam->ray.vector);
	cam->ray.current_refractive_index = REFRACTIVE_INDEX_AIR;
}
*/

#endif