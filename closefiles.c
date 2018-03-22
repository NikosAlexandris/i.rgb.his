#include <stdlib.h>
#include <grass/gis.h>
#include <grass/raster.h>
#include "globals.h"

/* This routine closes up the cell maps, frees up the row buffers and
   uses a less than perfect way of setting the color maps for the output
   to grey scale.  */

int closefiles(char *hue, char *intensity, char *saturation,
               int fd_output[3],
               DCELL * rowbuffer[3])

{
    unsigned int property;  // color properties: hue, intensity, saturation
    struct Colors colors;
    struct FPRange range;
    struct History history;
    double min;
    double max;
    const char *mapset;

    /* free allocated memory, close raster maps */
    for (property = 0; property < 3; property++)
    {
        Rast_close(fd_output[property]);
        G_free(rowbuffer[property]);
    }

    mapset = G_mapset();

    /* write colors */
    Rast_read_fp_range(hue, mapset, &range);
    Rast_get_fp_range_min_max(&range, &min, &max);
    Rast_make_grey_scale_colors(&colors, min, max); // set to [0,max] instead ?
    Rast_write_colors(hue, mapset, &colors);

    Rast_read_fp_range(intensity, mapset, &range);
    Rast_get_fp_range_min_max(&range, &min, &max);
    Rast_make_grey_scale_colors(&colors, min, max);
    Rast_write_colors(intensity, mapset, &colors);

    Rast_read_fp_range(saturation, mapset, &range);
    Rast_get_fp_range_min_max(&range, &min, &max);
    Rast_make_grey_scale_colors(&colors, min, max);
    Rast_write_colors(saturation, mapset, &colors);

    /* write metadata */
    Rast_short_history(hue, "raster", &history);
    Rast_command_history(&history);
    Rast_write_history(hue, &history);
    Rast_put_cell_title(hue, "Image hue");
    Rast_write_units(hue, "degrees");

    Rast_short_history(intensity, "raster", &history);
    Rast_command_history(&history);
    Rast_write_history(intensity, &history);
    Rast_put_cell_title(intensity, "Image intensity");

    Rast_short_history(saturation, "raster", &history);
    Rast_command_history(&history);
    Rast_write_history(saturation, &history);
    Rast_put_cell_title(saturation, "Image saturation");

    return 0;
}
