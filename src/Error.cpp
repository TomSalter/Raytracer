#ifndef ERROR_C
#define ERROR_C

#include "Error.h"

// Written 09/01/2004
// Reports an error to the system and performs appropriate action
// Tested: YUS!
// Improvements Available: No
// Message is the error message code, details is error specific report passed by the calling function
void ReportError(int message, char* details)
{
	switch(message)
	{
	case WORLD_OPEN_FAILED:
		printf("FATAL ERROR:  World file %s failed to load.  Exiting ray-tracer.\n", details);
		exit(0);
		break;
	case MESH_MALLOC_FAILED:
		printf("FATAL ERROR:  Failed to allocate memory for the mesh list.\n");
		exit(0);
		break;
	case RAY_INTERSECT_ERROR:
		printf("WARNING:  Reported an invalid ray-triangle intersect.\n");
		break;
	case IRRADIANCE_COORDS_OUT_OF_RANGE:
		printf("FATAL ERROR:  Reported incorrect irradiance map coordinates.\n");
		exit(0);
		break;
	case IRRADIANCE_MAP_OUT_OF_MEMORY:
		printf("FATAL ERROR: Failed to allocate memory for the irradiance map.\n");
		exit(0);
		break;
	default:
		printf("WARNING:  Incorrect error handling code.\n");
		break;
	}
}


#endif