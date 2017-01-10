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

/**** Basic functions ****/
void setDefaultArgs(tile_args *args);

/**** Full tiling operations ****/
void tileImage(image_f *dst, image_f *src, tile_args args);

#endif // END TILE_H_
