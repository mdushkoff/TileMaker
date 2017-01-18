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

// Modulus & wrapping macro definitions
#define mod(x,y) (x%y<0?x%y+x:x%y)
#define wrp(x,y) (x%y>=0?x%y:y+x%y)

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
    //image_f tilec; // Tile copy (for non-destructive rotation/scaling)
    image_f mask;  // Gaussian mask
//    image_f fmask; // Full mask for rotation
    image_f acc;   // Accumulator for normalization
    int tH,tW;     // Corrected tile heights and widths
    int h,w,d,v;   // Boundaries
    int o,x,y,z;   // Iterators
    int i;         // Exact coordinate (for reuse)
    int xoff,yoff; // Coordinate offsets (per tile)
    float rot,sca; // Random rotation and scale

    // Save boundaries for easy access
    h = (*src).height; w = (*src).width; d = (*src).depth;
    v = pow(2,args.octave); // Octave square root boundary

    // Create destination image (with initial background)
    alloc_image(dst,h,w,d);
    image_fillChan(&(*dst),args.bgColor.r,0);
    image_fillChan(&(*dst),args.bgColor.g,1);
    image_fillChan(&(*dst),args.bgColor.b,2);
    //image_fillChan(&(*dst),0.0,3);

    // Create tile (scaled source)
    if (args.pHeight <= 0){
        args.pHeight = h/v;
    }
    if (args.pWidth <= 0){
        args.pWidth = w/v;
    }
    /*if (args.pHeight > 0){
        tH  = args.pHeight;
    }
    else{
        tH = h/v;
    }
    if (args.pWidth >0){
        tW = args.pWidth;
    }
    else{
        tW = w/v;
    }
    image_scale(&tile,src,tH,tW,SIMPLE);

    // Create mask
    alloc_image(&mask,tH,tW,d);
    image_gaussmat(&mask,args.blur,1.0);*/

    // Create accumulator
    alloc_image(&acc,h,w,d);
    image_fill(&acc,0.0);

    // Mask the tile
    //*image_mul(&tile,&mask);*/
//    printf("SB: %f\n",args.scaleBase);

    // Perform tiling operation
    for (o=0; o<(v*v); o++){ // Octave iteration
        // Calculate random rotation/scale
        sca = args.scaleBase + randf(args.scaleVar,-args.scaleVar);
        rot = args.rotBase + randf(args.rotVar,-args.rotVar);

        // Apply random scale
        //if (sca != 1.0){
            tH  = (int)((float)args.pHeight*sca);
            tW = (int)((float)args.pWidth*sca);
            image_scale(&tile,src,tH,tW,SIMPLE);
            alloc_image(&mask,tH,tW,d);

        // Create full mask
//        alloc_image(&fmask,tH,tW,d);
//        image_gaussmat(&fmask,args.blur,1.0);

        //}
        //else {
        //    copy_image(tilec,tile);
        //}

        // Apply random rotation
//        if (rot != 0){
//            image_rotate(&tile,src,rot);
//            image_rotate(&mask,&fmask,rot);
//        }
//        else {
            image_gaussmat(&mask,args.blur,1.0);
//        }
        image_mul(&tile,&mask);

        // Calculate coordinate offsets
        xoff = (w/v)*(o%v)-(tW/2)+(w/(v*2));
        yoff = (h/v)*(o/v)-(tH/2)+(h/(v*2));

        // Loop through all tile pixels and place them
        for (y=0; y<tH; y++){
            for (x=0; x<tW; x++){
                for (z=0; z<d; z++){
                    // Calculate output coordinates
                    i = (z*h*w+wrp((y+yoff),h)*w+wrp((x+xoff),w));

                    // Accumulate image
                    (*dst).data[i] += tile.data[z*tH*tW+y*tW+x];

                    // Accumulate divisor
                    acc.data[i] += mask.data[z*tH*tW+y*tW+x];
                }
            }
        }

        // Deallocate tile (for varying tile sizes)
        dealloc_image(&tile);
        dealloc_image(&mask);
//        dealloc_image(&fmask);
    }

    // Divide output image by accumulator
    image_div(dst,&acc);

    // Deallocate
    //dealloc_image(&tile);
    //tile = NULL;
    //dealloc_image(&mask);
    //mask = NULL;
    dealloc_image(&acc);
    //dealloc_image(&fmask);
}
