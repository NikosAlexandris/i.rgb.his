
/******************************************************************************

NAME:       RGB2HIS
 
PURPOSE     To process red,green,blue bands to hue,intensity,saturation.
 
ALGORITHM:
            Get red, green, blue from input buffer
            Create the HIS bands
            Write to output buffer
 
ASSUMPTION:
            The input images are read to the input buffer.

NOTE:       For GRASS one row from each cell map is passed in and each cell in
            each band is processed and written out.   CWU GIS Lab: DBS 8/90

******************************************************************************/

#include <grass/gis.h>
#include "globals.h"

void rgb2his(DCELL * rowbuffer[3], unsigned int columns, double max_colors)
{
    int column;     // column counter
    double red;     // red input band
    double green;   // green input band
    double blue;    // blue input output
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

        red = rowbuffer[0][column];
        red /= max_colors;

        green = rowbuffer[1][column];
        green /= max_colors;

        blue = rowbuffer[2][column];
        blue /= max_colors;

        /* max of {r,g,b} */

        max = red;
        if (green > max)
            max = green;

        if (blue > max)
            max = blue;

        /* min of {r,g,b} */

        min = red;
        if (green < min)
            min = green;

        if (blue < min)
            min = blue;

        /* chroma and intensity */

        chroma = max - min;
        intensity = ((max + min) / 2.0);

        /* if R == G == B, then min == max, which is achromatic */

        if (chroma == 0.0)
        {
            saturation = 0.0;
            hue = -1.0; // undefined hue, set to -1.0

            rowbuffer[0][column] = hue;
            rowbuffer[1][column] = intensity;
            rowbuffer[2][column] = saturation;
        }

        /*  else chromatic */

        else if (chroma != 0.0) {

            if (intensity <= 0.5)
                saturation = chroma / (max + min);

            else if (intensity > 0.5)
                saturation = chroma / (2 - chroma);

            /* set red, green, blue for computing hue */

            red = (max - red) / chroma;
            green = (max - green) / chroma;
            blue = (max - blue) / chroma;

            /* resulting color between yelmin and magenta */

            if (red = max)
                hue = blue - green;

            /* resulting color between cyan and yelmin */

            else if (green == max)
                hue = 2 + red - blue;

            /* resulting color between magenta and cyan */

            else if (blue == max)
                hue = 4 + green - red;

            /* convert to degrees */

            hue *= 60.0;

            /* make nonnegative */

            if (hue < 0.0)
                hue += 360.0;

            /* HIS output values */

            rowbuffer[0][column] = hue;
            rowbuffer[1][column] = intensity;
            rowbuffer[2][column] = saturation;
        }
    }
}
