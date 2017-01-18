/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This program allows for a simplified tiling approach
 * that creates continuous seamless textures.
 *
 * Author: Michael Dushkoff (mad1841@rit.edu)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
        if (strcmp(str,flagDefs[i]) == 0){
            return i;
        }
    }
    return 0;
}

/*
 * Parse given arguments and corresponding flag.
 *
 * Inputs:
 *     args - The argument pointer (modified)
 *     str - The string value to parse
 *     flag - The given FlagType to apply
 */
void parseArgs(tile_args *args, char *str, FlagType flag){
    switch(flag){
        case COLOR:
            // TODO: Parse color information
            //(*args).bgColor.r = 
            //(*args).bgColor.g = 
            //(*args).bgColor.b = 
            break;
        case OCTAVE:
            (*args).octave = atoi(str);
            //printf("Octave: %d\n",(*args).octave);
            break;
        case HEIGHT:
            (*args).pHeight = atoi(str);
            //printf("Height: %d\n",(*args).pHeight);
            break;
        case WIDTH:
            (*args).pWidth = atoi(str);
            //printf("Width: %d\n",(*args).pWidth);
            break;
        case BLUR:
            (*args).blur = atof(str);
            //printf("Blur: %f\n",(*args).blur);
            break;
        case ROTATION:
            (*args).rotBase = atof(str);
            //printf("Rotation: %f\n",(*args).rotBase);
            break;
        case ROTVAR:
            (*args).rotVar = atof(str);
            //printf("Rotation Variance: %f\n",(*args).rotVar);
            break;
        case SCALE:
            (*args).scaleBase = atof(str);
            //printf("Scale: %f\n",(*args).scaleBase);
            break;
        case SCALEVAR:
            (*args).scaleVar = atof(str);
            //printf("Scale Variance: %f\n",(*args).scaleVar);
            break;
        case SEED:
            (*args).seed = atoi(str);
            //printf("SEED: %d\n",(*args).seed);
            break;
        default:
            break;
    }
}


/*
 * This executes the main program with various inputs according
 * to the usage statement.
 */
int main(int argc, char *argv[], char **envp){
    int i;                    // Iterator
    tile_args args;           // Tile arguments
    FlagType flag = NONE;     // Flag type mapped in order
    FlagType prevFlag = NONE; // Previously encountered flag
    image_f imgIn;            // Input image
    image_f imgOut;           // Output image
    char *inFile;             // Input filename
    char *outFile;            // Output filename

    // Initial argument number check
    if (argc < 3){
        usage();
        return -1;
    }

    // Set tile input, output files and default arguments
    setDefaultArgs(&args);
    inFile = argv[1];
    outFile = argv[2];

    // Parse arguments (skipping command name and files)
    for (i=3; i<argc; i++){
        // Get current flag type
        flag = check_flag(argv[i]);

        // Only parse details if the previous flag is valid
        if (prevFlag != NONE && flag == NONE){
            parseArgs(&args,argv[i],prevFlag);
        }
        else if ((prevFlag != NONE && flag != NONE) || flag == HELP){
            usage();
            return -1;
        }
        prevFlag = flag;
    }

    // Read input file
    imgIn = read_png(inFile);

    // Set seed if it is provided
    if (args.seed != 0){
        srand((unsigned int)args.seed);
    }
    else{
        srand((unsigned int)time(NULL));
    }

    // Perform tiling operation
    tileImage(&imgOut,&imgIn,args);

    // Write output
    write_png(&imgOut,outFile,8);

    // Deallocate images
    dealloc_image(&imgIn);
    dealloc_image(&imgOut);

    return 0;
}
