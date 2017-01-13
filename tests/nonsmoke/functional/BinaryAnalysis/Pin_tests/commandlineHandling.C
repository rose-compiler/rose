// We only need include "rose.h" now that ROSE includes optional 
// support for Intel Pin (configure --with-Intel_Pin=<path>).
#include "rose.h"

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

/* ================================================================== */
// Global variables 
/* ================================================================== */

string image_name;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */

// Example use of KNOBs (command line options) this adds an option to 
// the commandline support for Pin Tools. When used with Pin, ROSE can
// likely only support a limited number of options (each has to be expressed
// as a Pin knob.
KNOB<UINT32> KnobSize(KNOB_MODE_WRITEONCE, "pintool", "size","42", "example option (for testing commandline handling)");

INT32 Usage()
   {
     cerr << "This tool tests command line handling " << endl;
     cerr << KNOB_BASE::StringKnobSummary() << endl;
     return 1;
   }

VOID ImageLoad(IMG img, VOID * v)
   {
  // Detect each image loaded and save the name of the main program.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));
     bool isMainExecutable       = IMG_IsMainExecutable(img);
     if (isMainExecutable == true)
          image_name = IMG_Name(img);
   }

VOID Fini(INT32 code, VOID *v)
   {
  // This function calls ROSE specific code to support static analysis.

  // Set the verbose level of ROSE (for debugging).
  // SgProject::set_verbose(2);

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

  // Add any interesting static binary analysis here...


  // Regenerate binary or source code
     int status = backend(project);
     ROSE_ASSERT(status == 0);
   }


int main(int argc, char *argv[])
   {
  // global_argc = argc;
  // global_argv = argv;

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
