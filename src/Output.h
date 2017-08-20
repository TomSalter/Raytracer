#ifndef OUTPUT_H
#define OUTPUT_H

#include "glut/glut.h"

int TGA_Write(const char *filename, int width, int height, const float *pixels);
void WritePixel(COLOUR, int, int);
void CreateImage(const char*);

#endif