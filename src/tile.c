/*
 * This performs advanced tiling operations
 * utilizing image functions.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 */

#include <stdio.h>
#include <math.h>
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

/*
 * This creates a tiled output image given an input image
 * and various shaping parameters.
 *
 * Inputs:
 *     dst - The output tiled image (modified)
 *     src - The input image
 *     args - Shaping arguments described by:
 *         bgColor - Background color (for non-overlapped regions)
 *         pHeight - Tile height
 *         pWidth - Tile width
 *         blur - Gaussian sigma value
 *         rotBase - Base image rotation
 *         rotVar - Rotation variance
 *         scaleBase - Base image scale
 *         scaleVar - Scale variance
 *         seed - Seed
 */
void tileImage(image_f *dst, image_f *src, tile_args args){
    image_f tile;  // Scaled tile
    image_f mask;  // Gaussian mask
    int h,w,d,v;   // Boundaries
    int o,x,y,z;   // Iterators

    // Save boundaries for easy access
    h = (*src).height; w = (*src).width; d = (*src).depth;
    v = pow(2,args.octave); // Octave boundary

    // Create destination image (with initial background)
    alloc_image(dst,h,w,d);
    image_fillChan(dst,args.bgColor.r,0);
    image_fillChan(dst,args.bgColor.g,1);
    image_fillChan(dst,args.bgColor.b,2);
    image_fillChan(dst,0.0,3);

    // Create tile (scaled source)
    image_scale(&tile,src,args.pHeight,args.pWidth,SIMPLE);

    // Create mask
    alloc_image(&mask,args.pHeight,args.pWidth,d);
    image_gaussmat(&mask,args.blur,1.0);

    // Mask the tile
    image_mul(&tile,&mask);

    // Perform tiling operation
    for (o=0; o<v; o++){ // Octave iteration
        for (y=0; y<args.pHeight; y++){
            for (x=0; x<args.pWidth; x++){
                for (z=0; z<d; z++){
                    
                }
            }
        }
    }

    // Deallocate
    dealloc_image(&tile);
    //tile = NULL;
    dealloc_image(&mask);
    //mask = NULL;
}
