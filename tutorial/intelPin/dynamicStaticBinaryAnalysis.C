// This built upon an example from the Intel Pin distribution.
// It adds static analysis to the pin tool by constructing the 
// binary AST of the input executable.  In principal any static
// analysis could then be done using the dynamically derived 
// information.

// However, we only need include "rose.h" now that ROSE includes 
// optional support for Intel Pin.
#include "rose.h"

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

/* ================================================================== */
// Global variables 
/* ================================================================== */

UINT64 insCount = 0;        // number of dynamically executed instructions
UINT64 bblCount = 0;        // number of dynamically executed basic blocks
UINT64 threadCount = 0;     // total number of threads, including main thread

string image_name;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool", "o", "", "specify file name for MyPinTool output");
KNOB<BOOL>   KnobCount(KNOB_MODE_WRITEONCE,  "pintool", "count", "1", "count instructions, basic blocks and threads in the application");

/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32 Usage()
   {
     cerr << "This tool prints out the number of dynamically executed " << endl <<
             "instructions, basic blocks and threads in the application." << endl << endl;

     cerr << KNOB_BASE::StringKnobSummary() << endl;

     return -1;
   }

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

/*!
 * Increase counter of the executed basic blocks and instructions.
 * This function is called for every basic block when it is about to be executed.
 * @param[in]   numInstInBbl    number of instructions in the basic block
 * @note use atomic operations for multi-threaded applications
 */
VOID CountBbl(UINT32 numInstInBbl)
   {
     bblCount++;
     insCount += numInstInBbl;
   }

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

/*!
 * Insert call to the CountBbl() analysis routine before every basic block 
 * of the trace.
 * This function is called every time a new trace is encountered.
 * @param[in]   trace    trace to be instrumented
 * @param[in]   v        value specified by the tool in the TRACE_AddInstrumentFunction
 *                       function call
 */
VOID Trace(TRACE trace, VOID *v)
   {
  // Visit every basic block in the trace
     for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
       // Insert a call to CountBbl() before every basic bloc, passing the number of instructions
          BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)CountBbl, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
        }
   }

/*!
 * Increase counter of threads in the application.
 * This function is called for every thread created by the application when it is
 * about to start running (including the root thread).
 * @param[in]   threadIndex     ID assigned by PIN to the new thread
 * @param[in]   ctxt            initial register state for the new thread
 * @param[in]   flags           thread creation flags (OS specific)
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddThreadStartFunction function call
 */
VOID ThreadStart(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, VOID *v)
   {
     threadCount++;
   }

VOID ImageLoad(IMG img, VOID * v)
   {
  // Detect each image loaded and save the name of the main program.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));
     bool isMainExecutable       = IMG_IsMainExecutable(img);
     if (isMainExecutable == true)
          image_name = IMG_Name(img);
   }

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddFiniFunction function call
 */
VOID Fini(INT32 code, VOID *v)
   {
     std::ostream * out = &cerr;
     string fileName = KnobOutputFile.Value();

     if (!fileName.empty()) { out = new std::ofstream(fileName.c_str());}

     *out <<  "===============================================" << endl;
     *out <<  "MyPinTool analysis results: " << endl;
     *out <<  "Number of instructions: " << insCount  << endl;
     *out <<  "Number of basic blocks: " << bblCount  << endl;
     *out <<  "Number of threads: " << threadCount  << endl;
     *out <<  "===============================================" << endl;

  // We can set the verbose level of ROSE even before calling the constructor (data member is a static data member).
  // Set the verbose level of ROSE.
     SgProject::set_verbose(0);

  // Need to process the commandline...
     vector<string> sourceCommandline;
     sourceCommandline.push_back("intel_pin_wrapper");
     sourceCommandline.push_back(image_name);

     printf ("Now build AST for binary executable: %s \n",image_name.c_str());

  // After gathering dynamic information, run the static analysis...using ROSE
  // SgProject* project = frontend(global_argc,global_argv);
     SgProject* project = frontend(sourceCommandline);
     ROSE_ASSERT (project != NULL);

  // Internal AST consistancy tests.
     AstTests::runAllTests(project);


  // Add any interesting static analysis here...


     int status = backend(project);
     ROSE_ASSERT(status == 0);
   }

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char *argv[])
   {
#if 0
     string internalState = LEVEL_BASE::KNOB_BASE::StringLongAll();
     printf ("In main: internalState:\n%s \n",internalState.c_str());
#endif
#if 0
     string knobSummary = LEVEL_BASE::KNOB_BASE::StringKnobSummary();
     printf ("In main: knobSummary:\n%s \n",knobSummary.c_str());
#endif

  // Initialize PIN library. Print help message if -h(elp) is specified
  // in the command line or the command line is invalid 
     if ( PIN_Init(argc,argv) )
        {
          printf ("Commandline error in pin tool! \n");
          return Usage();
        }
    
     if (KnobCount)
        {
       // Specify callback function when each image is loaded (main program or shared libraries).
          IMG_AddInstrumentFunction(ImageLoad, 0);

       // Register function to be called to instrument traces
          TRACE_AddInstrumentFunction(Trace, 0);

       // Register function to be called for every thread before it starts running
          PIN_AddThreadStartFunction(ThreadStart, 0);

       // Register function to be called when the application exits
          PIN_AddFiniFunction(Fini, 0);
        }
    
     cerr <<  "===============================================" << endl;
     cerr <<  "This application is instrumented by MyPinTool" << endl;
     if (!KnobOutputFile.Value().empty()) 
        {
          cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
        }
     cerr <<  "===============================================" << endl;

  // Start the program, never returns
     PIN_StartProgram();
    
     return 0;
   }
