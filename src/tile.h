/* * * * * * * * * * * * * * * * * * * * * * * * *
 * This represents predeclarations for tiling
 * operations.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image.h"

// TILE_H_
#ifndef TILE_H_
#define TILE_H_

/**** Structure declarations ****/
typedef struct{
    char *inFile;
    char *outFile;
    rgb_f bgColor;
    int octave;
    int pHeight;
    int pWidth;
    float blur;
    float rotBase;
    float rotVar;
    float scaleBase;
    float scaleVar;
    int seed;
} tile_args;


#endif // END TILE_H_
