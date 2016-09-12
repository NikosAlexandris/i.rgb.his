#include <stdio.h>
#include <grass/gis.h>
#include <grass/raster.h>
#include <grass/glocale.h>
#include "globals.h"

void openfiles(char *red, char *green, char *blue,
               char *hue, char *intensity, char *saturation,
               int fd_input[3], int fd_output[3],
               DCELL *rowbuffer[3])
{
    /* input file descriptors */
    fd_input[0] = Rast_open_old(red, "");
    fd_input[1] = Rast_open_old(green, "");
    fd_input[2] = Rast_open_old(blue, "");

    /* open output files */
    fd_output[0] = Rast_open_fp_new(hue);
    fd_output[1] = Rast_open_fp_new(intensity);
    fd_output[2] = Rast_open_fp_new(saturation);

    /* allocate the cell row buffer */
    rowbuffer[0] = Rast_allocate_d_buf();
    rowbuffer[1] = Rast_allocate_d_buf();
    rowbuffer[2] = Rast_allocate_d_buf();
}
