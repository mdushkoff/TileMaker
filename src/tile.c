/*
 * This performs advanced tiling operations
 * utilizing image functions.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 */

#include <stdio.h>
#include "tile.h"
#include "image.h"

/*
 * This sets the arguments of a given
 * argument structure to their defaults.
 *
 * Inputs:
 *     args - The given argument structure (modified)
 */
void setDefaultArgs(tile_args *args){
    // Set all default arguments
    (*args).bgColor.r = 0; (*args).bgColor.g = 0; (*args).bgColor.b = 0;
    (*args).octave = 2;
    (*args).pHeight = -1; (*args).pWidth = -1; // Implies adaptive
    (*args).blur = 0.5;
    (*args).rotBase = 0.0; (*args).rotVar = 0.0;
    (*args).scaleBase = 1.0; (*args).scaleVar = 0.0;
    (*args).seed = 0; // Implies always random
}
