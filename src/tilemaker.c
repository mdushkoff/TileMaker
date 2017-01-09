/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This program allows for a simplified tiling approach
 * that creates continuous seamless textures.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>
#include "image.h"
#include "tile.h"

// Definitions
#define NUM_FLAGS (12)

// Basic enumeration of flags
typedef enum{
    NONE,
    COLOR,
    OCTAVE,
    HEIGHT,
    WIDTH,
    BLUR,
    ROTATION,
    ROTVAR,
    SCALE,
    SCALEVAR,
    SEED,
    HELP
} FlagType;

// Corresponding flag definitions
const char *flagDefs[] = {"","-c","-o","-h","-w","-m","-R","-r","-S","-s","-x","--help"};

/*
 * Print the program usage to the user.
 */
void usage(){
    printf("Usage:\n");
    printf("    tilemaker input.png output.png [options]\n");
    printf("Options:\n");
    printf("  -c [R,G,B]   Background color\n");
    printf("  -o           Octave\n");
    printf("  -h           Patch Height\n");
    printf("  -w           Patch Width\n");
    printf("  -m           Mask Blur\n");
    printf("  -R           Base Rotation (radians)\n");
    printf("  -r           Rotation Variance\n");
    printf("  -S           Base Scale Multiplier\n");
    printf("  -s           Scale Variance\n");
    printf("  -x           Seed\n");
    printf("  --help       Show usage information\n");
}

/*
 * Check a given string to determine if it is a flag.
 *
 * Inputs:
 *     str - The string to check
 * Outputs:
 *     flag - An enumeration of the flag type
 */
FlagType check_flag(char *str){
    int i;

    // Loop through all flag types
    for (i=1; i<NUM_FLAGS; i++){
        //printf("%s ?== %s\n",str,flagDefs[i]);
        if (strcmp(str,flagDefs[i]) == 0){
            return i;
        }
    }
    return 0;
}


/*
 * This executes the main program with various inputs:
 *
 */
int main(int argc, char *argv[], char **envp){
    int i;                    // Iterator
    tile_args args;           // Tile arguments
    FlagType flag = NONE;     // Flag type mapped in order
    FlagType prevFlag = NONE; // Previously encountered flag
    image_f imgIn;            // Input image
    image_f imgOut;           // Output image

    // Initial argument number check
    if (argc < 3){
        usage();
        return -1;
    }

    // Set tile input and output files
    args.inFile = argv[1];
    args.outFile = argv[2];

    // Parse arguments (skipping command name and files)
    for (i=3; i<argc; i++){
        // Get current flag type
        flag = check_flag(argv[i]);

        // Only parse details if the previous flag is valid
        if (prevFlag != NONE && flag == NONE){
            
        }
        else if ((prevFlag != NONE && flag != NONE) || flag == HELP){
            usage();
            return -1;
        }
        prevFlag = flag;
        printf("%s -> %d\n",argv[i],flag);
    }
    //usage();

    // Allocate dummy matrix
    alloc_image(&imgIn,256,256,4);
    image_gaussmat(&imgIn,0.5,1.0);
    //image_fillChan(&imgIn,1.0,0);
    //image_fillChan(&imgIn,0.0,1);
    //image_fillChan(&imgIn,0.5,2);
    image_fillChan(&imgIn,1.0,3);

    // Write PNG file
    write_png(&imgIn,args.outFile,8);

    // Perform tiling operation
    //tile(args);
    /*for (i=0; i<3*3*4; i++){
        printf("%f ",(imgIn).data[i]);
        if ((i+1)%3 == 0){
            printf("\n");
        }
    }
    printf("\n");*/

    // Deallocate dummy matrix
    dealloc_image(&imgIn);

    return 0;
}
