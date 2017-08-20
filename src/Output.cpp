#ifndef OUTPUT_C
#define OUTPUT_C

#include <stdio.h>
#include <assert.h>
#include "Types.h"
#include "Output.h"
#include "main.h"

float colourbuffer[WIDTH * HEIGHT * 3];

int TGA_Write(const char *filename, int width, int height, const float *pixels)
{
	FILE *fp;
	struct TGA header;
	const float *ptr, *end;

	// assert parameters
	assert(filename != 0);
	assert(width > 0);
	assert(height > 0);
	assert(pixels != 0);

	// open destination file
	fp = fopen(filename, "wb");
	if (fp == 0) {
		fprintf(stderr, "TGA_Write: failed to open file %s\n", filename);
		return 0;
	}

	// fill in header structure
	header.idLength = 0; // no id provided
	header.colourMapType = 0; // no colour map
	header.imageType = 2; // uncompressed RGB
	header.colourMapFirstEntryIndex = 0;
	header.colourMapLength = 0;
	header.colourMapEntrySize = 0;
	header.xOrigin = 0;
	header.yOrigin = 0;
	header.width = width;
	header.height = height;
	header.pixelDepth = 24; // 24bit
	header.imageDescriptor = 0;

	// write header to destination file (separately to avoid padding problems with a struct and bit-length of short)
	fwrite(&header.idLength, 1, 1, fp);
	fwrite(&header.colourMapType, 1, 1, fp);
	fwrite(&header.imageType, 1, 1, fp);
	fwrite(&header.colourMapFirstEntryIndex, 2, 1, fp);
	fwrite(&header.colourMapLength, 2, 1, fp);
	fwrite(&header.colourMapEntrySize, 1, 1, fp);
	fwrite(&header.xOrigin, 2, 1, fp);
	fwrite(&header.yOrigin, 2, 1, fp);
	fwrite(&header.width, 2, 1, fp);
	fwrite(&header.height, 2, 1, fp);
	fwrite(&header.pixelDepth, 1, 1, fp);
	fwrite(&header.imageDescriptor, 1, 1, fp);

	// convert to 24bit and write image data
	ptr = pixels; // points at pixel to be written
	end = pixels + width * height * 3; // points past last pixel to write
	while (ptr < end) {
		byte bgr[3];
		bgr[2] = (int)(255 * *(ptr++)); // R
		bgr[1] = (int)(255 * *(ptr++)); // G
		bgr[0] = (int)(255 * *(ptr++)); // B
		fwrite(bgr, 1, 3, fp);
	}

	// close file
	fclose(fp);

	return 1;
}

void CreateImage(const char* filename)
{
	TGA_Write(filename, WIDTH, HEIGHT, colourbuffer);
}


void WritePixel(COLOUR color, int x, int y)
{
	// Make sure we are writing the color buffer correctly
	assert(x < WIDTH);
	assert(y < HEIGHT);
	assert(x >= 0);
	assert(y >= 0);

	// Write red
	colourbuffer[(y * WIDTH * 3) + (x * 3) + 0] = color.r;
	colourbuffer[(y * WIDTH * 3) + (x * 3) + 1] = color.g;
	colourbuffer[(y * WIDTH * 3) + (x * 3) + 2] = color.b;
}

void glutRender()
{
	int i, j;
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			glColor3f(	colourbuffer[(i * WIDTH * 3) + (j * 3)],
						colourbuffer[(i * WIDTH * 3) + (j * 3) + 1],
						colourbuffer[(i * WIDTH * 3) + (j * 3) + 2]);
			glVertex2i(j, i);
		}
	}

	glEnd();

	glFlush();
}
	



#endif

