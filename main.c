
/****************************************************************************
 *
 * MODULE:      i.rgb.his
 *
 * AUTHOR(S):   David Satnik, GIS Laboratory, Central Washington University
 *              Nikos Alexandris, support for user defined image bit depth
 *
 *              with acknowledgements to Ali Vali,
 *              Univ. of Texas Space Research Center, for the core routine.
 *
 * PURPOSE:     Red-Green-Blue (rgb) to Hue-Intensity-Saturation (his)
 *              raster map color transformation function
 *
 * COPYRIGHT:   (C) 2007-2016 by the GRASS Development Team
 *
 *              This program is free software under the GNU General Public
 *              License (>=v2). Read the file COPYING that comes with GRASS
 *              for details.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <grass/gis.h>
#include <grass/raster.h>
#include <grass/glocale.h>
#include "globals.h"

int main(int argc, char **argv)
{
    unsigned int band;
    unsigned long row;
    unsigned int rows;
    unsigned int cols;
    DCELL *rowbuffer[3];
    struct Option *opt_red;
    struct Option *opt_green;
    struct Option *opt_blue;
    struct Option *opt_hue;
    struct Option *opt_intensity;
    struct Option *opt_saturation;
    struct Option *opt_bits;
    struct GModule *module;
    int fd_input[3];
    int fd_output[3];
    int bits;   // bitness of input raster maps
    double max_colors;  // maximum level based on input bitness

    G_gisinit(argv[0]);  // initialize GIS engine

    /* Set description */
    module = G_define_module();
    G_add_keyword(_("imagery"));
    G_add_keyword(_("color transformation"));
    G_add_keyword("RGB");
    G_add_keyword("HIS");
    G_add_keyword("IHS");
    module->description =
        _("Transforms raster maps from RGB (Red-Green-Blue) color space to "
                "HIS (Hue-Intensity-Saturation) color space.");

    /* Define the different options */
    opt_red = G_define_standard_option(G_OPT_R_INPUT);
    opt_red->key = "red";
    opt_red->description = _("Name of input raster map (red)");

    opt_green = G_define_standard_option(G_OPT_R_INPUT);
    opt_green->key = "green";
    opt_green->description = _("Name of input raster map (green)");

    opt_blue = G_define_standard_option(G_OPT_R_INPUT);
    opt_blue->key = "blue";
    opt_blue->description = _("Name of input raster map (blue)");

    opt_hue = G_define_standard_option(G_OPT_R_OUTPUT);
    opt_hue->key = "hue";
    opt_hue->description = _("Name for output raster map (hue)");

    opt_intensity = G_define_standard_option(G_OPT_R_OUTPUT);
    opt_intensity->key = "intensity";
    opt_intensity->description = _("Name for output raster map (intensity)");

    opt_saturation = G_define_standard_option(G_OPT_R_OUTPUT);
    opt_saturation->key = "saturation";
    opt_saturation->description = _("Name for output raster map (saturation)");

    opt_bits = G_define_option();
    opt_bits->key = "bits";
    opt_bits->type = TYPE_INTEGER;
    opt_bits->required = NO;
    opt_bits->answer = "8";
    opt_bits->options = "2-16";
    opt_bits->description = _("Bits per input image");

    if (G_parser(argc, argv))
        exit(EXIT_FAILURE);

    /* bit depth, should be > 0 */
    bits = atoi(opt_bits->answer);
    if (bits <= 0)
        G_fatal_error(_("Invalid bit depth definition!"));

    /* open half ended range for maximum level */
    max_colors = pow(2, bits) - 1.0;
    G_debug(1, "%d-bit data ranging in [0,%.0f)", bits, max_colors);

    /* get dimension of the image */
    rows = Rast_window_rows();
    cols = Rast_window_cols();

    openfiles(opt_red->answer, opt_green->answer, opt_blue->answer,
            opt_hue->answer, opt_intensity->answer, opt_saturation->answer,
            fd_input, fd_output, rowbuffer);

    for (row = 0; row < rows; row++) {

        /* read in a row from each cell map */
        G_percent(row, rows, 2);

        for (band = 0; band < 3; band++)
            Rast_get_d_row(fd_input[band], rowbuffer[band], row);

        /* process this row of the map */
        rgb2his(rowbuffer, cols, max_colors);

        /* write out the new row for each cell map */
        for (band = 0; band < 3; band++)
            Rast_put_row(fd_output[band], rowbuffer[band], DCELL_TYPE);
    }

    /* progress */
    G_percent(row, rows, 2);

    /* close output files */
    closefiles(opt_hue->answer, opt_intensity->answer, opt_saturation->answer,
            fd_output, rowbuffer);

    exit(EXIT_SUCCESS);
}
