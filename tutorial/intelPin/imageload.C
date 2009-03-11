//
// This tool prints a trace of image load and unload events
//

// If we don't include "rose.h" then at least include "rose_config.h" so that
// the configure support can be seen and the tests in IntelPinSupport.h will not fail.
#include <rose_config.h>
#include "IntelPinSupport.h"

#include <stdio.h>

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

FILE * trace;

// Pin calls this function every time a new img is loaded
// It can instrument the image, but this example does not
// Note that imgs (including shared libraries) are loaded lazily

VOID ImageLoad(IMG img, VOID *v)
   {
     printf("Loading %s, Image id = %d\n", IMG_Name(img).c_str(), IMG_Id(img));
     fprintf(trace, "Loading %s, Image id = %d\n", IMG_Name(img).c_str(), IMG_Id(img));
   }

// Pin calls this function every time a new img is unloaded
// You can't instrument an image that is about to be unloaded
VOID ImageUnload(IMG img, VOID *v)
   {
     printf("Unloading %s\n", IMG_Name(img).c_str());
     fprintf(trace, "Unloading %s\n", IMG_Name(img).c_str());
   }

// This function is called when the application exits
// It prints the name and count for each procedure
VOID Fini(INT32 code, VOID *v)
   {
     fclose(trace);
   }

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int
main(int argc, char * argv[])
   {
     trace = fopen("imageload.out", "w");

  // Initialize symbol processing
     PIN_InitSymbols();
    
  // Initialize pin
     PIN_Init(argc, argv);
    
  // Register ImageLoad to be called when an image is loaded
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Register ImageUnload to be called when an image is unloaded
     IMG_AddUnloadFunction(ImageUnload, 0);

  // Register Fini to be called when the application exits
     PIN_AddFiniFunction(Fini, 0);
    
  // Start the program, never returns
     PIN_StartProgram();
    
     return 0;
   }
