
/******************************************************************************

NAME:       RGB to HIS
 
PURPOSE     To process red, green, blue bands to hue, intensity, saturation.
 
ALGORITHM:  Get red, green, blue from input buffer
            Create the HIS bands
            Write to output buffer
 
ASSUMPTION: The input images are read to the input buffer.

NOTE:       For GRASS one row from each cell map is passed in and each cell in
            each band is processed and written out.   CWU GIS Lab: DBS 8/90

******************************************************************************/

#include <grass/gis.h>
#include "globals.h"

void rgb2his(DCELL * rowbuffer[3], unsigned int columns, double max_colors)
{
    int column;     // column counter
    double scaled_red;     // red input band, to be scaled to [0,1]
    double scaled_green;   // green input band, likewise
    double scaled_blue;    // blue input output, likewise
    double red;     // red value used for computing hue
    double green;   // green value, likewise
    double blue;    // blue value, likewise
    double min;     // minimum among red, green, blue
    double max;     // maximum among red, green, blue
    double chroma;  // maximum - minimum
    double hue = 0.0L;
    double saturation;
    double intensity;

    for (column = 0; column < columns; column++) {

        if (Rast_is_d_null_value(&rowbuffer[0][column]) ||
            Rast_is_d_null_value(&rowbuffer[1][column]) ||
            Rast_is_d_null_value(&rowbuffer[2][column]))
        {
            Rast_set_d_null_value(&rowbuffer[0][column], 1);
            Rast_set_d_null_value(&rowbuffer[1][column], 1);
            Rast_set_d_null_value(&rowbuffer[2][column], 1);
            continue;
        }

        /* scale red, green, blue to [0.0,1.0] */

        scaled_red = rowbuffer[0][column];
        scaled_red /= max_colors;

        scaled_green = rowbuffer[1][column];
        scaled_green /= max_colors;

        scaled_blue = rowbuffer[2][column];
        scaled_blue /= max_colors;

        /* max of {r,g,b} */

        max = scaled_red;
        if (scaled_green > max)
            max = scaled_green;

        if (scaled_blue > max)
            max = scaled_blue;

        /* min of {r,g,b} */

        min = scaled_red;
        if (scaled_green < min)
            min = scaled_green;

        if (scaled_blue < min)
            min = scaled_blue;

        /* chroma and intensity */

        chroma = max - min;
        intensity = ((max + min) / 2.0);

        /* if R == G == B, then min == max, which is achromatic */

        if (chroma == 0.0)
        {
            saturation = 0.0;
            hue = -1.0; // undefined hue, set to -1.0
        }

        /*  else chromatic */

        else if (chroma != 0.0) {

            if (intensity <= 0.5)
            {
                saturation = chroma / (max + min);
                G_debug(2, "Saturation (for I <= 0.5): %f", saturation);
            }

            else if (intensity > 0.5)
            {
                saturation = chroma / (2 - max - min);
                G_debug(2, "Saturation (for I > 0.5): %f", saturation);
            }

            /* set red, green, blue for computing hue */

            red = (max - scaled_red) / chroma;
            green = (max - scaled_green) / chroma;
            blue = (max - scaled_blue) / chroma;

            /* resulting color between yelmin and magenta */

            if (scaled_red == max)
            {
                hue = blue - green;
                G_debug(2, "Hue (blue - green): %f", hue);
            }

            /* resulting color between cyan and yelmin */

            else if (scaled_green == max)
            {
                hue = 2 + red - blue;
                G_debug(2, "Hue (red - blue): %f", hue);
            }
            /* resulting color between magenta and cyan */

            else if (scaled_blue == max)
            {
                hue = 4 + green - red;
                G_debug(2, "Hue (green - red): %f", hue);
            }
            /* convert to degrees */

            hue *= 60.0;

            /* make nonnegative */

            if (hue < 0.0)
                hue += 360.0;


        }

        G_debug(2, "Minimum and Maximum levels among r, g, b: [%f, %f]", min, max);
        G_debug(2, "HIS: %f, %f, %f", hue, intensity, saturation);

        /* HIS output values */

        /* set hue = -1.0 to NULL */

        if (hue == -1.0)
            Rast_set_d_null_value(&rowbuffer[0][column], 1);

        else
            rowbuffer[0][column] = (FCELL)hue;

        rowbuffer[1][column] = (FCELL)intensity;
        rowbuffer[2][column] = (FCELL)saturation;

        /* for debugging purposes */
        G_debug(3, "Output rowbuffers 0, 1, 2: %f, %f, %f\n",
                rowbuffer[0][column], rowbuffer[1][column], rowbuffer[2][column]);
    }
}
