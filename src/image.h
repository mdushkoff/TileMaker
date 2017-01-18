/* * * * * * * * * * * * * * * * * * * * * * * * *
 * Predefinitions of basic image functions.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 * * * * * * * * * * * * * * * * * * * * * * * * */

// IMAGE_H_
#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdlib.h>

/**** Definitions ****/
#define randf(m,n) ( ((float)rand()/(float)RAND_MAX)*(m-n)+n )

/**** Basic floating point image structure ****/
typedef struct {
    float *data;
    int height;
    int width;
    int depth;
} image_f;

/**** Basic floating point RGB structure ****/
typedef struct{
    float r;
    float g;
    float b;
} rgb_f;

/**** Interpolation method enumeration ****/
typedef enum{
    SIMPLE,
    BILINEAR,
    BICUBIC
} interp_m;


/**** Basic struct operations ****/
void alloc_image(image_f *img, int height, int width, int depth);
void dealloc_image(image_f *img);
void copy_image(image_f *dst, image_f *src);

/**** Image operations ****/
image_f read_png(char *filename);
void write_png(image_f *img, char *filename, unsigned char bit_depth);
void image_rotate(image_f *dst, image_f *src, float angle);
void image_scale(image_f *dst, image_f *src, int dstHeight, int dstWidth, interp_m method);
void image_add(image_f *img1, image_f *img2);
void image_mul(image_f *img1, image_f *img2);
void image_div(image_f *img1, image_f *img2);
void image_fill(image_f *img, float num);
void image_fillChan(image_f *img, float num, int chan);
void image_unifrnd(image_f *img);
void image_gaussmat(image_f *img, float sigma, float gain);
//image_f gaussian_mat(int height, int width, int depth, float var

#endif // END IMAGE_H_
