// This program does a static analysis first (builds the binary AST for the 
// input executable) and then does a dynamic analysis.  At any point
// in the dynamic analysis a static analysis of any part of the binary AST
// can easily be done.

// We only need include "rose.h" now that ROSE includes optional 
// support for Intel Pin (configure --with-Intel_Pin=<path>).
#include "rose.h"

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

INT32 Usage()
   {
     cerr << "This tool tests static analysis first, and then dynamic analysis " << endl;
     return 1;
   }

VOID ImageLoad(IMG img, VOID * v)
   {
  // Detect each image loaded (note that the binary AST has already been constructed).
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));
   }

VOID Fini(INT32 code, VOID *v)
   {
  // This is not really needed, we could remove this
   }

using namespace std;

int main(int argc, char *argv[])
   {
  // Set the verbose level of ROSE (for debugging).
  // SgProject::set_verbose(2);

     printf ("argc = %d \n",argc);
     for (int i=0; i < argc; i++)
        {
          printf ("argv[%d] = %s \n",i,argv[i]);
        }

  // This is the typical location of the executable file name in a simple Pin commandline
     string image_name = (char*) argv[argc-1];
     printf ("Executable to analyze: %s \n",image_name.c_str());

  // Need to process the commandline...
     vector<string> sourceCommandline;
     sourceCommandline.push_back("intel_pin_wrapper");
     sourceCommandline.push_back(image_name);

     printf ("Now build AST for binary executable: %s \n",image_name.c_str());

  // After gathering dynamic information, run the static analysis...using ROSE
     SgProject* project = frontend(sourceCommandline);
     ROSE_ASSERT (project != NULL);

     printf ("Running tests on AST for binary executable: %s \n",image_name.c_str());
  // Internal AST consistancy tests.
     AstTests::runAllTests(project);


  // Add any interesting static binary analysis here...or anywhere else...


  // Regenerate binary or source code
     int status = backend(project);
     ROSE_ASSERT(status == 0);

  // Initialize Pin
     if ( PIN_Init(argc,argv) )
        {
          return Usage();
        }

  // Specify callback function when each image is loaded (main program or shared libraries).
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Register function to be called when the application exits
     PIN_AddFiniFunction(Fini, 0);
    
  // Start the program, never returns
     PIN_StartProgram();
    
     return 0;
   }
