// We only need include "rose.h" now that ROSE includes optional 
// support for Intel Pin (configure --with-Intel_Pin=<path>).
#include "rose.h"

// Use the namespace defined in Pin (required fix to pin.H)
using namespace LEVEL_PINCLIENT;

using namespace std;

/* ================================================================== */
// Global variables 
/* ================================================================== */

UINT64 ins_count = 0;
FILE *fout;
#define FILENAME_SIZE 128

string filename;

map<unsigned int, unsigned int> writtenAddrs;

unsigned int dumpnum = 0;

set<void*> instructions;
set<pair<void*,void*> > instructionEdges;

map<void*, string> disassembledInstructions;

void *previousInstructionIP = NULL;

unsigned long instructionCounter = 0;

// ADDRINT lowerBoundAddessRange = 0;
// ADDRINT upperBoundAddessRange = 0;
void* lowerBoundAddessRange = NULL;
void* upperBoundAddessRange = NULL;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */


/* ================================================================== */
// ROSE Specific Code
/* ================================================================== */

class TraceStructType
   {
     SgAsmBlock* asmblock;
     public:
          TraceStructType(SgAsmBlock* block);

   };

TraceStructType::TraceStructType(SgAsmBlock* block)
   : asmblock(block)
   {
  // Constructor
   }


class BlockTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

// Maybe this shuld be a pair<SgAsmBlock*,traceStruct>
// map<rose_addr_t,SgAsmBlock*> blockMap;
map<rose_addr_t,pair<SgAsmBlock*,TraceStructType*> > blockMap;

void BlockTraversal::visit(SgNode* n)
   {
     SgAsmBlock* asmBlock = isSgAsmBlock(n);
     if (asmBlock != NULL)
        {
       // Save the address of the SgAsmBlock.
          rose_addr_t asmBlockAddress = asmBlock->get_address();
       // printf ("asmBlockAddress = %zu \n",asmBlockAddress);
          printf ("asmBlockAddress = %p \n",asmBlockAddress);

          bool blockExists = blockMap.find(asmBlockAddress) != blockMap.end();
          ROSE_ASSERT(blockExists == false);

          if (blockExists == false)
             {
            // Need to add the block address.
            // blockMap[asmBlockAddress] = asmBlock;
               blockMap[asmBlockAddress] = pair<SgAsmBlock*,TraceStructType*>(asmBlock,new TraceStructType(asmBlock));
             }
        }
   }


/* ================================================================== */
// Intel Pin Specific Code
/* ================================================================== */

INT32 Usage()
   {
     cerr << "This tool tests static analysis first, and then dynamic analysis " << endl;
     return 1;
   }

#if 0
VOID ImageLoad(IMG img, VOID * v)
   {
  // Detect each image loaded and save the name of the main program.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));
   }
#endif

#if 0
// This is an example of what is required to do the instrumentation of the 
// function level so that 

// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{
    
    // Allocate a counter for this routine
    RTN_COUNT * rc = new RTN_COUNT;

    // The RTN goes away when the image is unloaded, so save it now
    // because we need it in the fini
    rc->_name = RTN_Name(rtn);
    rc->_image = StripPath(IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
    rc->_address = RTN_Address(rtn);
    rc->_icount = 0;
    rc->_rtnCount = 0;

    // Add to list of routines
    rc->_next = RtnList;
    RtnList = rc;
            
    RTN_Open(rtn);
            
    // Insert a call at the entry point of a routine to increment the call count
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)docount, IARG_PTR, &(rc->_rtnCount), IARG_END);
    
    // For each instruction of the routine
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
    {
        // Insert a call to docount to increment the instruction counter for this rtn
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_PTR, &(rc->_icount), IARG_END);
    }

    
    RTN_Close(rtn);
}
#endif

#if 1
VOID graphInstruction(void *ip)
   {
  // This function is executed for every instruction that is executed.

  // printf ("Instruction address = %p \n",ip);

  // Limit the size of the generated dot file (to avoid generating too large of a graph for the layout algorithm).
  // if (instructionCounter < 4000)

  // Make sure these have ben set.
     assert(lowerBoundAddessRange != NULL);
     assert(upperBoundAddessRange != NULL);

  // Now we just limit the instructions to those from the ".text" segment of the main problem (where IMG_IsMainExecutable(img) == true).
     if (ip >= lowerBoundAddessRange && ip <= upperBoundAddessRange)
        {
          assert(disassembledInstructions.find(ip) != disassembledInstructions.end());
          string disassembledInstructionString = disassembledInstructions[ip];

          if (instructions.find(ip) == instructions.end())
             {
               instructions.insert(ip);
            // fprintf(fout,"\"%p\"[label=\"%p\"];\n",ip,ip);
               fprintf(fout,"\"%p\"[label=\"    %p : %s    \" color=blue, fillcolor=\"lightblue\" style=filled, ];\n",ip,ip,disassembledInstructionString.c_str());
             }

          if (previousInstructionIP == NULL)
             {
            // Make the starting node more obvious.
               fprintf(fout,"\"%p\"[label=\"START\" shape=polygon,regular=0,sides=5,peripheries=1,color=Green,fillcolor=Green,fontcolor=black,style=filled];\n",previousInstructionIP);
             }

       // "0x680660" -> "0x2a99aff010"[label="header" dir=both ];
          pair<void*,void*> edge = pair<void*,void*>(previousInstructionIP,ip);

          if (instructionEdges.find(edge) == instructionEdges.end())
             {
               instructionEdges.insert(edge);
               fprintf(fout,"\"%p\" -> \"%p\"[label=\"%lu\" color=\"blue\", decorate dir=forward ]\n",previousInstructionIP,ip,instructionCounter);
             }

          previousInstructionIP = ip;
        }

     instructionCounter++;
   }


VOID Instruction(INS ins, VOID *v)
   {
  // Pin calls this function every time a new instruction is encountered

  // printf ("Executing a new instruction v = %p \n",v);

  // What we really want to do is pass this to the graphInstruction() function (or build a map of strings)...
  // OR, could alternatively use: INS_Disassemble(INS ins)
     void* ip = (void*) INS_Address(ins);
     string disassembledInstructionString = INS_Disassemble(ins);
#if 0
     printf ("Executing a new instruction ins at %p = %s = %s \n",ip,INS_Mnemonic(ins).c_str(),disassembledInstructionString.c_str());
#endif
     if (disassembledInstructions.find(ip) == disassembledInstructions.end())
        {
       // printf ("Adding %p = %s to map \n",ip,disassembledInstructionString.c_str());
          disassembledInstructions[ip] = INS_Disassemble(ins);
        }

  // Insert a call to measureip() before every instruction, and pass it the IP
     INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) graphInstruction, IARG_INST_PTR, IARG_END);
   }


VOID ImageLoad(IMG img, VOID * v)
   {
  // Output information about each image (main application or shared library, 
  // typically) as it is loaded during execution.
     printf("Loading %s, Image id = %d \n", IMG_Name(img).c_str(), IMG_Id(img));

     ADDRINT image_entry         = IMG_Entry(img);
  // Global pointer (GP) of image, if a GP is used to address global data
     ADDRINT image_globalPointer = IMG_Gp(img);
     ADDRINT image_loadOffset    = IMG_LoadOffset(img);
     ADDRINT image_lowAddress    = IMG_LowAddress(img);
     ADDRINT image_highAddress   = IMG_HighAddress(img);
     ADDRINT image_startAddress  = IMG_StartAddress(img);
     USIZE image_sizeMapped      = IMG_SizeMapped(img);
     bool isMainExecutable       = IMG_IsMainExecutable(img);
     IMG_TYPE image_type         = IMG_Type(img);

     if (isMainExecutable == true)
        {
          printf ("   image_entry         = 0x%zx \n",image_entry);
          printf ("   image_globalPointer = 0x%zx \n",image_globalPointer);
          printf ("   image_loadOffset    = 0x%zx \n",image_loadOffset);
          printf ("   image_lowAddress    = 0x%zx \n",image_lowAddress);
          printf ("   image_highAddress   = 0x%zx \n",image_highAddress);
          printf ("   image_startAddress  = 0x%zx \n",image_startAddress);
          printf ("   image_sizeMapped    = %u \n",image_sizeMapped);
          printf ("   isMainExecutable    = %s \n",isMainExecutable ? "true" : "false");
          printf ("   section_type        = %s \n",output(image_type).c_str());
        }

  // Output information about each section
     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
        {
       // trace << "Section " << SEC_Name(sec) << " found" << endl;

          string section_name = SEC_Name(sec);
          if (isMainExecutable == true)
             {
               printf ("***section_name = %s \n",section_name.c_str());
             }

          bool isReadable   = SEC_IsReadable(sec);
          bool isWritable   = SEC_IsWriteable(sec);
          bool isExecutable = SEC_IsExecutable(sec);
          bool isMapped     = SEC_Mapped(sec);

          SEC_TYPE section_type    = SEC_Type(sec);
          ADDRINT section_address  = SEC_Address(sec);
          const VOID* section_data = SEC_Data(sec);
          USIZE section_size       = SEC_Size(sec);

          bool listAvailableFunctions = false;
          if (isMainExecutable == true && section_name == ".text")
             {
               lowerBoundAddessRange = (void*) section_address;
               upperBoundAddessRange = (void*) (section_address + section_size);

#if 0
               RTN rtn = LEVEL_PINCLIENT::RTN_FindByName (img,"main");
               if (RTN_Valid(rtn) == true)
                  {
                    ADDRINT function_address = RTN_Address(rtn);
                    USIZE function_size      = RTN_Size(rtn);

                    cout << "      main routine: " << RTN_Name(rtn) << " address: " << function_address << " size: " << function_size << endl;

                 // If we find function "main" then set the range to that function.
                    lowerBoundAddessRange = (void*) function_address;
                    upperBoundAddessRange = (void*) (function_address + function_size);

                    listAvailableFunctions = true;
                  }
                 else
                  {
                    printf ("No main function found \n");
                  }
#else
               RTN rtn = LEVEL_PINCLIENT::RTN_FindByName (img,"frame_dummy");
               if (RTN_Valid(rtn) == true)
                  {
                    ADDRINT function_address = RTN_Address(rtn);
                    USIZE function_size      = RTN_Size(rtn);

                    cout << "      frame_dummy routine: " << RTN_Name(rtn) << " address: " << function_address << " size: " << function_size << endl;

                 // Make the lower bound just after the end of the function that appears before user-defined functions.
                    lowerBoundAddessRange = (void*) (function_address + function_size + 1);
                    listAvailableFunctions = true;
                  }
                 else
                  {
                    printf ("No frame_dummy function found \n");
                  }

               rtn = LEVEL_PINCLIENT::RTN_FindByName (img,"main");
               if (RTN_Valid(rtn) == true)
                  {
                    ADDRINT function_address = RTN_Address(rtn);
                    USIZE function_size      = RTN_Size(rtn);

                    cout << "      frame_dummy routine: " << RTN_Name(rtn) << " address: " << function_address << " size: " << function_size << endl;

                 // Make the upper bound the end of the "main" function.
                    upperBoundAddessRange = (void*) (function_address + function_size);
                    listAvailableFunctions = true;
                  }
                 else
                  {
                    printf ("No main function found \n");
                  }
#endif
             }

          if (isMainExecutable == true && isExecutable == true)
             {
               printf ("      isReadable   = %s \n",isReadable   ? "true" : "false");
               printf ("      isWritable   = %s \n",isWritable   ? "true" : "false");
               printf ("      isExecutable = %s \n",isExecutable ? "true" : "false");
               printf ("      isMapped     = %s \n",isMapped     ? "true" : "false");
               printf ("      section_type = %s \n",output(section_type).c_str());
               printf ("      section_address = 0x%zx \n",section_address);
               printf ("      section_data = %p \n",section_data);
               printf ("      section_size = %u \n",section_size);
             }

          if (listAvailableFunctions == true)
             {
               for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
                  {
                    cout << "      routine: " << RTN_Name(rtn) << " address: " << RTN_Address(rtn) << " size: " << RTN_Size(rtn) << endl;

                    RTN_Open(rtn);
            
                    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
                       {
                      // Just print first and last
                         if (!INS_Valid(INS_Prev(ins)) || !INS_Valid(INS_Next(ins)))
                            {
                              cout << "         " << INS_Address(ins);
#if 0
                              cout << " " << INS_Disassemble(ins) << " read:";
                    
                              for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
                                 {
                                   cout << " " << REG_StringShort(INS_RegR(ins, i));
                                 }
                              cout << " writes:";
                              for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
                                 {
                                   cout << " " << REG_StringShort(INS_RegW(ins, i));
                                 }
#endif
                              cout << endl;
                            }
                       }

                    RTN_Close(rtn);
                  }
             }
        }
   }
#endif



#if 1
// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every block
VOID PIN_FAST_ANALYSIS_CALL blockAddress(ADDRINT c)
   {
     icount += c;

#if 1
  // printf ("In PIN_FAST_ANALYSIS_CALL docount(c = %zu) \n",c);
     printf ("In PIN_FAST_ANALYSIS_CALL blockAddress(c = %p) \n",c);

  // DQ (5/9/2009): I think we have to subtract something from the address to normalize it properly.

     bool blockExists = blockMap.find((rose_addr_t)c) != blockMap.end();
     ROSE_ASSERT(blockExists == true);

     ROSE_ASSERT(false);
#endif
   }

// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID *v)
   {
  // Visit every basic block  in the trace
     for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
       // Insert a call to docount for every bbl, passing the number of instructions.
       // IPOINT_ANYWHERE allows Pin to schedule the call anywhere in the bbl to obtain best performance.
       // Use a fast linkage for the call.

       // BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(docount), IARG_FAST_ANALYSIS_CALL, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
          BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(blockAddress), IARG_FAST_ANALYSIS_CALL, IARG_UINT32, BBL_Address(bbl), IARG_END);
        }
   }
#endif


VOID Fini(INT32 code, VOID *v)
   {
  // This is not really needed.

     printf ("icount = %zu \n",icount);
   }




int main(int argc, char *argv[])
   {
#if 0
     printf ("Starting Intel Pin tool to generate a graph of the memory trace \n");

  // Must call this, before PIN_Init(), so that function symbols can be interpreted.
     PIN_InitSymbols();

  // Calling initialization for Pin.
     if( PIN_Init(argc,argv) )
        {
          return Usage();
        }

     filename = argv[argc-1];
     printf ("Processing filename = %s \n",filename.c_str());

     string outputFilename = "trace.dot";
     fout = fopen(outputFilename.c_str(), "w");

  // Start the dot graph
     fprintf(fout,"digraph \"G./%s.dot\" {\n", argv[argc-1]);

  // Set the verbose level of ROSE (for debugging).
  // SgProject::set_verbose(2);

     printf ("argc = %d \n",argc);
     for (int i=0; i < argc; i++)
        {
          printf ("argv[%d] = %s \n",i,argv[i]);
        }
#endif

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

  // Add any interesting static binary analysis here...

  // Build the traversal object (later will be also build the CFG)
     BlockTraversal blockTraversal;

  // Call the traversal starting at the project node of the AST
     blockTraversal.traverse(project,preorder);

  // Regenerate binary or source code
     int status = backend(project);
     ROSE_ASSERT(status == 0);

  // Initialize Pin
     if ( PIN_Init(argc,argv) )
        {
          return Usage();
        }

#if 0
  // Register Routine to be called to instrument rtn
     RTN_AddInstrumentFunction(Routine, 0);
#endif

#if 0
  // Setup instrumentation callback function.
     INS_AddInstrumentFunction(Instruction, 0);
#endif

#if 1
  // Register Instruction to be called to instrument instructions
     TRACE_AddInstrumentFunction(Trace, 0);
#endif

  // Specify callback function when each image is loaded (main program or shared libraries).
     IMG_AddInstrumentFunction(ImageLoad, 0);

  // Register function to be called when the application exits
     PIN_AddFiniFunction(Fini, 0);
    
  // Start the program, never returns
     PIN_StartProgram();
    
     return 0;
   }
