#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <grass/raster.h>

/*
 * openfiles.c
 * red, green, blue
 * hue, saturation, intensity,
 * input file descriptors, output file descriptors
 * input rowbuffer
 */
void openfiles(char *, char *, char *, char *, char *, char *, int[3], int[3],
        DCELL *[3]);

/*
 * rgb2hsl.c
 * input rowbuffer, columns, maximum range value
 */
void rgb2his(DCELL *[3], unsigned int, double);

/*
 * closefiles.c
 * hue, saturation, intensity
 * output file descriptors
 * output rowbuffers
 */
int closefiles(char *, char *, char *, int[3], DCELL *[3]);

#endif /* __GLOBALS_H__ */
