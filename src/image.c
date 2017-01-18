/* * * * * * * * * * * * * * * * * * * * * * * * *
 * This defines various image operations that
 * allow for parameterized image manipulations.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
//#include <stdlib.h>
#include <png.h>
#include <math.h>
#include <omp.h>
#include "image.h"

#define chunk (1024)
#define nthreads (128)

void perror_(const char* s){
    fprintf(stderr,"%s\n",s);
    abort();
}

/*
 * This allocates an image structure.
 *
 * Inputs:
 *     img - The input image structure (modified)
 */
void alloc_image(image_f *img, int height, int width, int depth){
    (*img).data = (float*)malloc(sizeof(float)*height*width*depth);
    (*img).height = height;
    (*img).width = width;
    (*img).depth = depth;
}

/*
 * This deallocates a given image pointer.
 *
 * Inputs:
 *     img - The input image to deallocate
 */
void dealloc_image(image_f *img){
    free((*img).data);
}

/*
 * This copies a given image into a new destination
 * by allocating space for the destination.
 *
 * Inputs:
 *     dst - The destination image (modified)
 *     src - The source image
 */
void copy_image(image_f *dst, image_f *src){
    memcpy((*dst).data, (*src).data, sizeof(float)*(*src).height*(*src).width*(*src).depth);
    (*dst).height = (*src).height;
    (*dst).width = (*dst).width;
    (*dst).depth = (*dst).depth;
}

/*
 * Reads a PNG file into a struct.
 *
 * Inputs:
 *     filename - The name of the PNG file
 * Outputs:
 *     out - The png_structp of the inputted file
 */
image_f read_png(char *filename){
    int w, h, d;             // Boundaries
    int row, col, dep;       // Iterators
    png_byte color_type;     // Determines number of channels
    //png_byte bit_depth;      // Number of bits per color
    image_f out;             // Output image
    png_structp pngP;        // PNG data pointer
    png_infop info_ptr;      // PNG info pointer
    unsigned char header[8]; // Header is a maximum of 8 bytes
    png_byte *rowBytes;      // PNG row data

    // Open the given file
    FILE *fp = fopen(filename,"rb");

    // Check for NULL file pointer
    if (!fp){
        perror_("ERROR: File could not be opened for reading.");
    }

    // Check for valid PNG file
    fread(header,1,8,fp);
    if (png_sig_cmp(header,0,8)){
        perror_("ERROR: File is not recognized as a PNG file.");
    }

    // Initialize structure
    pngP = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);

    // Check for valid structure
    if (!pngP){
        perror_("ERROR: PNG structure allocation failed.");
    }

    // Get info struct and check if it is valid
    info_ptr = png_create_info_struct(pngP);
    if (!info_ptr){
        perror_("ERROR: PNG info structure allocation failed.");
    }

    // Generic I/O error checking
    if (setjmp(png_jmpbuf(pngP))){
        perror_("ERROR: PNG I/O error.");
    }

    // Initialize I/O and read the PNG data
    png_init_io(pngP,fp);
    png_set_sig_bytes(pngP,8);
    png_read_info(pngP,info_ptr);

    // Aggregate information
    w = png_get_image_width(pngP,info_ptr);
    h = png_get_image_height(pngP,info_ptr);
    color_type = png_get_color_type(pngP,info_ptr);
    //bit_depth = png_get_bit_depth(pngP,info_ptr);
    d = color_type == PNG_COLOR_TYPE_RGB_ALPHA ? 4 : 3; // Set depth

    // Set jump point for error catching
    if (setjmp(png_jmpbuf(pngP))){
        perror_("ERROR: PNG read failure.");
    }

    // Allocate space to read a single row
    rowBytes = (png_byte*)malloc(png_get_rowbytes(pngP,info_ptr));

    // Allocate image
    alloc_image(&out,h,w,d);

    // Read file
    for (row=0; row<h; row++){
        // Get current row
        png_read_row(pngP,(png_bytep)rowBytes,NULL);
        for (col=0; col<w; col++){
            for (dep=0; dep<d; dep++){
                out.data[dep*w*h+row*w+col] = (float)((unsigned int)rowBytes[dep+col*d])/255.0;
            }
        }
    }

    // Close the file
    fclose(fp);

    // Deallocate space
    free(rowBytes);
    rowBytes = NULL;
    png_destroy_read_struct(&pngP,&info_ptr,(png_infopp)NULL);
    pngP = NULL;
    info_ptr = NULL;

    return out;
}

/*
 * Writes a PNG struct to a file.
 *
 * Inputs:
 *     img - The image pointer
 *     filename - The name of the output PNG file
 *     bitDepth - The number of bits to represent the output (8,16, or 32)
 */
void write_png(image_f *img, char *filename, unsigned char bitDepth){
    png_structp out_ptr;
    png_infop info_ptr;
    png_byte *rowBytes;
    int row;
    int col;
    int dep;
    int h = (*img).height;
    int w = (*img).width;
    int d = (*img).depth>3 ? 4 : 3; // Only allow RGB or RGBA

    // Open file for writing
    FILE *fp = fopen(filename,"wb");

    // Check for NULL file pointer
    if (!fp){
        perror_("ERROR: File could not be opened for writing.");
    }

    // Initialize structure for writing PNG and check if it was created properly
    out_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if (!out_ptr){
        perror_("ERROR: Could not create PNG structure.");
    }

    // Set up info pointer and check if it was created properly
    info_ptr = png_create_info_struct(out_ptr);
    if (!info_ptr){
        perror_("ERROR: Could not create info structure.");
    }

    // Set up jump point for I/O error catching
    if (setjmp(png_jmpbuf(out_ptr))){
        perror_("ERROR: I/O failure.");
    }

    // Initialize I/O
    png_init_io(out_ptr,fp);

    // Set up jump point for header error catching
    if (setjmp(png_jmpbuf(out_ptr))){
        perror_("ERROR: Header write failure.");
    }

    // Write header information
    png_set_IHDR(out_ptr,info_ptr,w,h,
                 (png_byte)bitDepth, d==3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
    png_write_info(out_ptr,info_ptr);

    // Set up jump point for writing error catching
    if (setjmp(png_jmpbuf(out_ptr))){
        perror_("ERROR: Error during PNG write.");
    }

    // Convert float image to byte image and write it to the file
    rowBytes = (png_byte*)malloc(png_get_rowbytes(out_ptr,info_ptr));
    for (row=0; row<h; row++){
        for (col=0; col<w; col++){
            for (dep=0; dep<d; dep++){
                rowBytes[col*d+dep] = (png_byte)( (255*(*img).data[dep*h*w+row*w+col]) );
            }
        }
        // Write row to PNG
        png_write_row(out_ptr,(png_bytep)rowBytes);
    }

    // Set up jump point for file end error catching
    if (setjmp(png_jmpbuf(out_ptr))){
        perror_("ERROR: End of file write failure.");
    }

    // Write end of file information
    png_write_end(out_ptr,NULL);

    // Free allocated space
    free(rowBytes);
    rowBytes = NULL;
    png_destroy_write_struct(&out_ptr,&info_ptr);
    out_ptr = NULL;
    info_ptr = NULL;

    // Close file
    fclose(fp);
}

/*
 * This rotates a given image around its center.
 *
 * Inputs:
 *     dst - The output image (modified)
 *     src - The given image to rotate
 *     angle - The angle to rotate
 */
void image_rotate(image_f *dst, image_f *src, float angle){
    int h,w,d;       // Source dimensions
    int oh,ow,od;    // Destination dimensions
    int /*row,*/col,dep; // Iterators
    float x,y;       // Float coordinates
    //float **rotcor;  // Rotated coordinate map
    //image_f cpy;     // Copy of the original image

    // Initialize boundaries
    h = (*src).height;
    w = (*src).width;
    d = (*src).depth;
    oh = (*dst).height;
    ow = (*dst).width;
    od = (*dst).depth;

    // Assertions
    if (d != od){
        perror_("ERROR: Destination depth does not match source depth.");
    }

    // Copy the original input image
    //copy_image(&cpy,img);

    // Fill the original image with zeros
    //image_fill(img,0);

    // Allocate and initialize coordinate map
    /*rotcor = (float**)malloc(sizeof(float*)*2);
    for (row=0; row<2; row++){
        rotcor[row] = (float*)malloc(sizeof(float)*h*w*d);
    }*/
    for (col=0; col<h*w*d; col++){
        // Calculate adjusted coordinates
        x = ((float)(col%w)-(float)w/2.0);
        y = (float)((col/w)%h)-(float)h/2.0;

        // Calculate rotation coordinates
        x = x*cos(angle)-y*sin(angle)+(float)w/2.0;
        y = x*sin(angle)+y*cos(angle)+(float)h/2.0;

        // Adjust coordinates for output image
        x = x+((float)ow/2.0-(float)w/2.0);
        y = y+((float)oh/2.0-(float)h/2.0);

        // Assign rotation
        if (x >= 0 && (int)x<ow && y>=0 && (int)y<(float)oh){
            for (dep=0; dep<d; dep++){
                (*dst).data[dep*oh*ow+(int)y*ow+(int)x] = (*src).data[dep*h*w+((col/w)%h)*w+col%w];
            }
        }

        // Assign rotated column coordinates [-w/2,w/2]*cos(angle)-[-h/2,h/2]*sin(angle)+w/2
        //rotcor[0][col] = x*cos(angle)-y*sin(angle)+(float)w/2.0;

        // Assign rotated row coordinates [-w/2,w/2]*sin(angle)+[-h/2,h/2]*cos(angle)+h/2
        //rotcor[1][col] = x*sin(angle)+y*cos(angle)+(float)h/2.0;
    }

    // Apply rotation (with clipping)


    // Deallocate coordinate map
    /*for (row=0; row<2; row++){
        free(rotcor[row]);
    }
    free(rotcor);*/

    // Deallocate copied image
    //dealloc_image(&cpy);
}

/*
 * This scales a given image to match a destination
 * size and then stores that into an unallocated
 * image pointer.
 *
 * Inputs:
 *     dst - The destination image pointer (modified)
 *     src - The source image pointer
 *     dstHeight - The destination height
 *     dstWidth - The destination width
 *     method - The method of interpolation
 */
void image_scale(image_f *dst, image_f *src, int dstHeight, int dstWidth, interp_m method){
    int x,y,z,n; // Iterators
    int h,w,d;   // Boundaries

    // Save boundaries
    h = (*src).height; w = (*src).width; d = (*src).depth;

    // Allocate destination image
    alloc_image(dst,dstHeight,dstWidth,d);

    // Check for interpolation method
    if (method == SIMPLE){
        //#pragma omp parallel for
        omp_set_num_threads(nthreads);
        //#pragma omp parallel //for //private(y,x,z) schedule (dynamic, 1) //collapse(3)
        //#pragma omp parallel for schedule(static) private(x,y,z) //schedule(static, chunk)
        //for (z=0; z<d; z++){
        ///#pragma omp parallel for schedule(static, chunk) private(x,y,z) shared(src,dst) collapse(3)
        //#pragma omp parallel for schedule(static) private(x,y,z) shared(src,dst) collapse(3)
        #pragma omp parallel for private(x,y,z)
        for (n=0; n<(dstHeight*dstWidth*d); n++){
            x = (n%dstWidth);
            y = (n/dstWidth)%(dstHeight);
            z = (n/(dstWidth*dstHeight));
        //#pragma omp for nowait
        //for (y=0; y<dstHeight; y++){
            //for (x=0; x<dstWidth; x++){
                //for (z=0; z<d; z++){
                    (*dst).data[z*dstHeight*dstWidth+y*dstWidth+x] =
                        (*src).data[z*h*w + (int)((float)y*((float)h/(float)dstHeight))*w +
                                            (int)((float)x*((float)w/(float)dstWidth))];
//                }
//            }
        }
    }
    else {
        perror_("ERROR: Function unimplemented");
    }
}

/*
 * This performs a point-wise addition between
 * two images and stores the result in the first image.
 *
 * Inputs:
 *     img1 - The first image to add (output)
 *     img2 - The second image to add
 */
void image_add(image_f *img1, image_f *img2){
    int i;
    int n1 = (*img1).height*(*img1).width*(*img1).depth;
    int n2 = (*img2).height*(*img2).width*(*img2).depth;

    // Check sizes
    if (n1 != n2){
        perror_("ERROR: Image sizes do not match.");
    }
    #pragma omp parallel for private(i)
    for (i=0; i<n1; i++){
        (*img1).data[i] = (*img1).data[i]+(*img2).data[i];
    }
}

/*
 * This performs a point-wise multiplication between
 * two images and stores the result in the first image.
 *
 * Inputs:
 *     img1 - The first image to multiply (output)
 *     img2 - The second image to multiply
 */
void image_mul(image_f *img1, image_f *img2){
    int i;
    int n1 = (*img1).height*(*img1).width*(*img1).depth;
    int n2 = (*img2).height*(*img2).width*(*img2).depth;

    // Check sizes
    if (n1 != n2){
        perror_("ERROR: Image sizes do not match.");
    }
    #pragma omp parallel for private(i)
    for (i=0; i<n1; i++){
        (*img1).data[i] = (*img1).data[i]*(*img2).data[i];
    }
}

/*
 * This performs a point-wise division between
 * two images and stores the result in the first image.
 *
 * Inputs:
 *     img1 - The first image to divide (output)
 *     img2 - The second image to divide
 */
void image_div(image_f *img1, image_f *img2){
    int i;
    int n1 = (*img1).height*(*img1).width*(*img1).depth;
    int n2 = (*img2).height*(*img2).width*(*img2).depth;

    // Check sizes
    if (n1 != n2){
        perror_("ERROR: Image sizes do not match.");
    }
    #pragma omp parallel for private(i)
    for (i=0; i<n1; i++){
        (*img1).data[i] = (*img1).data[i]/(*img2).data[i];
    }
}

/*
 * This fills a given image with a specified value.
 *
 * Inputs:
 *     img - The given image to fill
 *     num - The number to populate in the image
 */
void image_fill(image_f *img, float num){
    int i; // Iterator
    int n = (*img).height*(*img).width*(*img).depth; // Number of elements

    // Loop through all elements and set them to the given value
    #pragma omp parallel for private(i)
    for (i=0; i<n; i++){
        (*img).data[i] = num;
    }
}

/*
 * This fills a given image's channel with a specified value.
 *
 * Inputs:
 *     img - The given image to fill
 *     num - The number to populate in the image
 *     channel - The channel to set
 */
void image_fillChan(image_f *img, float num, int chan){
    int i; // Iterator
    int n = (*img).height*(*img).width; // Number of elements
    int doffs = n*chan; // Planar offset
    //printf("NUM: %f",num);
    #pragma omp parallel for private(i)
    for (i=0; i<n; i++){
        (*img).data[i+doffs] = num;
    }
}

/*
 * This produces a repeating 2D Gaussian plane
 * for the given input image size and various
 * Gaussian shaping parameters.
 *
 * Inputs:
 *     img - The given image to apply the function to
 *     sigma - The Gaussian's sigma value
 *     gain - A scaling factor for the output
 */
void image_gaussmat(image_f *img, float sigma, float gain){
    int x;
    int y;
    int h = (*img).height;
    int w = (*img).width;
    float xoff = (((float)w-1.0)/2.0);
    float yoff = (((float)h-1.0)/2.0);

    // Loop through all rows and columns of the first plane
    #pragma omp parallel for private(x,y) collapse(2)
    for (x=0; x<w; x++){
        for (y=0; y<h; y++){
            (*img).data[y*w+x] = gain*exp( -( pow(((float)x-xoff)/w,2.0)+pow(((float)y-yoff)/h,2.0) )/(2.0*pow(sigma,2.0)) );
	}
    }

    // Copy first plane to all other planes (avoids duplicate calculations)
    for (x=1; x<(*img).depth; x++){
        memcpy(&((*img).data[w*h*x]), (*img).data, sizeof(float)*w*h);
    }
}
