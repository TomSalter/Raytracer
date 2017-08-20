/ Entry point for the application

#include <stdio.h>
#include "Types.h"
#include "World.h"

#define WIDTH 1024
#define HEIGHT 768

float framebuffer[WIDTH * HEIGHT * 3];

int main()
{
	WORLD world;

	world = InitWorldFromFile("settings.dat");

	return 1;
}